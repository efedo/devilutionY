#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

static void PackItem(PkItemStruct *id, ItemStruct *is)
{
	if (is->_itype == ItemType::none) {
		id->idx = 0xFFFF;
	} else {
		id->idx = SwapLE16(is->IDidx);
		if (is->IDidx == ItemIndex::EAR) {
			id->iCreateInfo = is->_iName[8] | (is->_iName[7] << 8);
			id->iSeed = SwapLE32(is->_iName[12] | ((is->_iName[11] | ((is->_iName[10] | (is->_iName[9] << 8)) << 8)) << 8));
			id->bId = is->_iName[13];
			id->bDur = is->_iName[14];
			id->bMDur = is->_iName[15];
			id->bCh = is->_iName[16];
			id->bMCh = is->_iName[17];
			id->wValue = SwapLE16(is->_ivalue | (is->_iName[18] << 8) | ((is->_iCurs - 19) << 6));
			id->dwBuff = SwapLE32(is->_iName[22] | ((is->_iName[21] | ((is->_iName[20] | (is->_iName[19] << 8)) << 8)) << 8));
		} else {
			id->iSeed = SwapLE32(is->_iSeed);
			id->iCreateInfo = SwapLE16(is->_iCreateInfo);
			id->bId = is->_iIdentified + 2 * is->_iMagical;
			id->bDur = is->_iDurability;
			id->bMDur = is->_iMaxDur;
			id->bCh = is->_iCharges;
			id->bMCh = is->_iMaxCharges;
			if (is->IDidx == ItemIndex::GOLD)
				id->wValue = SwapLE16(is->_ivalue);
		}
	}
}

void PackPlayer(PkPlayerStruct *pPack, int pnum, bool manashield)
{
	PlayerStruct *pPlayer;
	int i;
	ItemStruct *pi;
	PkItemStruct *pki;

	memset(pPack, 0, sizeof(*pPack));
	pPlayer = &plr[pnum].data;
	pPack->destAction = pPlayer->destAction;
	pPack->destParam1 = pPlayer->destParam1;
	pPack->destParam2 = pPlayer->destParam2;
	pPack->plrlevel = pPlayer->plrlevel;
	pPack->p = pPlayer->_pos;
	pPack->targ = pPlayer->_pathtarg;
	strcpy(pPack->pName, pPlayer->_pName);
	pPack->pClass = pPlayer->_pClass;
	pPack->pBaseStr = pPlayer->_pBaseStr;
	pPack->pBaseMag = pPlayer->_pBaseMag;
	pPack->pBaseDex = pPlayer->_pBaseDex;
	pPack->pBaseVit = pPlayer->_pBaseVit;
	pPack->pLevel = pPlayer->_pLevel;
	pPack->pStatPts = pPlayer->_pStatPts;
	pPack->pExperience = SwapLE32(pPlayer->_pExperience);
	pPack->pGold = SwapLE32(pPlayer->_pGold);
	pPack->pHPBase = SwapLE32(pPlayer->_pHPBase);
	pPack->pMaxHPBase = SwapLE32(pPlayer->_pMaxHPBase);
	pPack->pManaBase = SwapLE32(pPlayer->_pManaBase);
	pPack->pMaxManaBase = SwapLE32(pPlayer->_pMaxManaBase);
	pPack->pMemSpells = SDL_SwapLE64(pPlayer->_pMemSpells);

	for (i = 0; i < MAX_SPELLS; i++)
		pPack->pSplLvl[i] = pPlayer->_pSplLvl[i];

	pki = &pPack->InvBody[0];
	pi = &pPlayer->InvBody[0];

	for (i = 0; i < MAXINVITEMS; i++) {
		PackItem(pki, pi);
		pki++;
		pi++;
	}

	pki = &pPack->InvList[0];
	pi = &pPlayer->InvList[0];

	for (i = 0; i < MAXINVITEMS; i++) {
		PackItem(pki, pi);
		pki++;
		pi++;
	}

	for (i = 0; i < MAXINVITEMS; i++)
		pPack->InvGrid[i] = pPlayer->InvGrid[i];

	pPack->_pNumInv = pPlayer->_pNumInv;
	pki = &pPack->SpdList[0];
	pi = &pPlayer->SpdList[0];

	for (i = 0; i < MAXBELTITEMS; i++) {
		PackItem(pki, pi);
		pki++;
		pi++;
	}

	pPack->pDiabloKillLevel = SwapLE32(pPlayer->pDiabloKillLevel);

	if (plr.isSingleplayer() || manashield)
		pPack->pManaShield = SwapLE32(pPlayer->pManaShield);
	else
		pPack->pManaShield = false;
}

/**
 * Expand a PkItemStruct in to a ItemStruct
 *
 * Note: last slot of item[MAXITEMS+1] used as temporary buffer
 * find real name reference below, possibly [sizeof(item[])/sizeof(ItemStruct)]
 * @param is The source packed item
 * @param id The distination item
 */
static void UnPackItem(PkItemStruct *is, ItemStruct *id)
{
	WORD idx = SwapLE16(is->idx);

	if (idx == 0xFFFF) {
		id->_itype = ItemType::none;
	} else {
		if (idx == ItemIndex::EAR) {
			RecreateEar(
			    MAXITEMS,
			    SwapLE16(is->iCreateInfo),
			    SwapLE32(is->iSeed),
			    is->bId,
			    is->bDur,
			    is->bMDur,
			    is->bCh,
			    is->bMCh,
			    SwapLE16(is->wValue),
			    SwapLE32(is->dwBuff));
		} else {
			RecreateItem(MAXITEMS, idx, SwapLE16(is->iCreateInfo), SwapLE32(is->iSeed), SwapLE16(is->wValue));
			item[MAXITEMS]._iMagical = is->bId >> 1;
			item[MAXITEMS]._iIdentified = is->bId & 1;
			item[MAXITEMS]._iDurability = is->bDur;
			item[MAXITEMS]._iMaxDur = is->bMDur;
			item[MAXITEMS]._iCharges = is->bCh;
			item[MAXITEMS]._iMaxCharges = is->bMCh;
		}
		*id = item[MAXITEMS];
	}
}

void VerifyGoldSeeds(PlayerStruct *pPlayer)
{
	int i, j;

	for (i = 0; i < pPlayer->_pNumInv; i++) {
		if (pPlayer->InvList[i].IDidx == ItemIndex::GOLD) {
			for (j = 0; j < pPlayer->_pNumInv; j++) {
				if (i != j) {
					if (pPlayer->InvList[j].IDidx == ItemIndex::GOLD && pPlayer->InvList[i]._iSeed == pPlayer->InvList[j]._iSeed) {
						pPlayer->InvList[i]._iSeed = GetRndSeed();
						j = -1;
					}
				}
			}
		}
	}
}

void UnPackPlayer(PkPlayerStruct *pPack, int pnum, bool killok)
{
	PlayerStruct *pPlayer;
	int i;
	ItemStruct *pi;
	PkItemStruct *pki;

	if (!plr.isValidPlayer(pnum)) plr.addPlayer(pnum);
	pPlayer = &plr[pnum].data;
	plr[pnum].ClearPlrRVars();
	pPlayer->_pos = pPack->p;
	pPlayer->_posfut = pPack->p;
	pPlayer->_pathtarg = pPack->targ;
	pPlayer->plrlevel = pPack->plrlevel;
	plr[pnum].ClrPlrPath();
	pPlayer->destAction = ACTION_NONE;
	strcpy(pPlayer->_pName, pPack->pName);
	pPlayer->_pClass = pPack->pClass;
	plr[pnum].InitPlayer(true);
	pPlayer->_pBaseStr = pPack->pBaseStr;
	pPlayer->_pStrength = pPack->pBaseStr;
	pPlayer->_pBaseMag = pPack->pBaseMag;
	pPlayer->_pMagic = pPack->pBaseMag;
	pPlayer->_pBaseDex = pPack->pBaseDex;
	pPlayer->_pDexterity = pPack->pBaseDex;
	pPlayer->_pBaseVit = pPack->pBaseVit;
	pPlayer->_pVitality = pPack->pBaseVit;
	pPlayer->_pLevel = pPack->pLevel;
	pPlayer->_pStatPts = pPack->pStatPts;
	pPlayer->_pExperience = SwapLE32(pPack->pExperience);
	pPlayer->_pGold = SwapLE32(pPack->pGold);
	pPlayer->_pMaxHPBase = SwapLE32(pPack->pMaxHPBase);
	pPlayer->_pHPBase = SwapLE32(pPack->pHPBase);
	if (!killok)
		if ((int)(pPlayer->_pHPBase & 0xFFFFFFC0) < 64)
			pPlayer->_pHPBase = 64;

	pPlayer->_pMaxManaBase = SwapLE32(pPack->pMaxManaBase);
	pPlayer->_pManaBase = SwapLE32(pPack->pManaBase);
	pPlayer->_pMemSpells = SDL_SwapLE64(pPack->pMemSpells);

	for (i = 0; i < MAX_SPELLS; i++)
		pPlayer->_pSplLvl[i] = pPack->pSplLvl[i];

	pki = &pPack->InvBody[0];
	pi = &pPlayer->InvBody[0];

	for (i = 0; i < MAXINVITEMS; i++) {
		UnPackItem(pki, pi);
		pki++;
		pi++;
	}

	pki = &pPack->InvList[0];
	pi = &pPlayer->InvList[0];

	for (i = 0; i < MAXINVITEMS; i++) {
		UnPackItem(pki, pi);
		pki++;
		pi++;
	}

	for (i = 0; i < MAXINVITEMS; i++)
		pPlayer->InvGrid[i] = pPack->InvGrid[i];

	pPlayer->_pNumInv = pPack->_pNumInv;
	VerifyGoldSeeds(pPlayer);

	pki = &pPack->SpdList[0];
	pi = &pPlayer->SpdList[0];

	for (i = 0; i < MAXBELTITEMS; i++) {
		UnPackItem(pki, pi);
		pki++;
		pi++;
	}

	if (pnum == myplr()) {
		for (i = 0; i < 20; i++)
			witchitem[i]._itype = ItemType::none;
	}

	CalcPlrInv(pnum, false);
	pPlayer->pTownWarps = 0;
	pPlayer->pDungMsgs = 0;
	pPlayer->pLvlLoad = 0;
	pPlayer->pDiabloKillLevel = SwapLE32(pPack->pDiabloKillLevel);
	pPlayer->pBattleNet = pPack->pBattleNet;
	pPlayer->pManaShield = SwapLE32(pPack->pManaShield);
}

DEVILUTION_END_NAMESPACE
