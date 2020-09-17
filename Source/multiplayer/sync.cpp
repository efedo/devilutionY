#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

WORD sync_word_6AA708[MAXMONSTERS];
int sgnMonsters;
WORD sgwLRU[MAXMONSTERS];
int sgnSyncItem;
int sgnSyncPInv;

DWORD sync_all_monsters(const BYTE *pbBuf, DWORD dwMaxLen)
{
	TSyncHeader *pHdr;
	int i;
	BOOL sync;

	if (nummonsters < 1) {
		return dwMaxLen;
	}
	if (dwMaxLen < sizeof(*pHdr) + sizeof(TSyncMonster)) {
		return dwMaxLen;
	}

	pHdr = (TSyncHeader *)pbBuf;
	pbBuf += sizeof(*pHdr);
	dwMaxLen -= sizeof(*pHdr);

	pHdr->bCmd = CMD_SYNCDATA;
	pHdr->bLevel = level.currlevel;
	pHdr->wLen = 0;
	SyncPlrInv(pHdr);
	assert(dwMaxLen <= 0xffff);
	sync_one_monster();

	for (i = 0; i < nummonsters && dwMaxLen >= sizeof(TSyncMonster); i++) {
		sync = FALSE;
		if (i < 2) {
			sync = sync_monster_active2((TSyncMonster *)pbBuf);
		}
		if (!sync) {
			sync = sync_monster_active((TSyncMonster *)pbBuf);
		}
		if (!sync) {
			break;
		}
		pbBuf += sizeof(TSyncMonster);
		pHdr->wLen += sizeof(TSyncMonster);
		dwMaxLen -= sizeof(TSyncMonster);
	}

	return dwMaxLen;
}

void sync_one_monster()
{
	int i, m;

	for (i = 0; i < nummonsters; i++) {
		m = monstactive[i];
		sync_word_6AA708[m] = abs(myplr().data._px - monsters[m].data._mx) + abs(myplr().data._py - monsters[m].data._my);
		if (monsters[m].data._msquelch == 0) {
			sync_word_6AA708[m] += 0x1000;
		} else if (sgwLRU[m] != 0) {
			sgwLRU[m]--;
		}
	}
}

BOOL sync_monster_active(TSyncMonster *p)
{
	int i, m, ndx;
	DWORD lru;

	ndx = -1;
	lru = 0xFFFFFFFF;

	for (i = 0; i < nummonsters; i++) {
		m = monstactive[i];
		if (sync_word_6AA708[m] < lru && sgwLRU[m] < 0xFFFE) {
			lru = sync_word_6AA708[m];
			ndx = monstactive[i];
		}
	}

	if (ndx == -1) {
		return FALSE;
	}

	sync_monster_pos(p, ndx);
	return TRUE;
}

void sync_monster_pos(TSyncMonster *p, int ndx)
{
	p->_mndx = ndx;
	p->_mx = monsters[ndx].data._mx;
	p->_my = monsters[ndx].data._my;
	p->_menemy = encode_enemy(ndx);
	p->_mdelta = sync_word_6AA708[ndx] > 255 ? 255 : sync_word_6AA708[ndx];

	sync_word_6AA708[ndx] = 0xFFFF;
	sgwLRU[ndx] = monsters[ndx].data._msquelch == 0 ? 0xFFFF : 0xFFFE;
}

BOOL sync_monster_active2(TSyncMonster *p)
{
	int i, m, ndx;
	DWORD lru;

	ndx = -1;
	lru = 0xFFFE;

	for (i = 0; i < nummonsters; i++) {
		if (sgnMonsters >= nummonsters) {
			sgnMonsters = 0;
		}
		m = monstactive[sgnMonsters];
		if (sgwLRU[m] < lru) {
			lru = sgwLRU[m];
			ndx = monstactive[sgnMonsters];
		}
		sgnMonsters++;
	}

	if (ndx == -1) {
		return FALSE;
	}

	sync_monster_pos(p, ndx);
	return TRUE;
}

void SyncPlrInv(TSyncHeader *pHdr)
{
	int ii;
	ItemStruct *pItem;

	if (numitems > 0) {
		if (sgnSyncItem >= numitems) {
			sgnSyncItem = 0;
		}
		ii = itemactive[sgnSyncItem++];
		pHdr->bItemI = ii;
		pHdr->bItemX = item[ii]._ix;
		pHdr->bItemY = item[ii]._iy;
		pHdr->wItemIndx = item[ii].IDidx;
		if (item[ii].IDidx == IDI_EAR) {
			pHdr->wItemCI = (item[ii]._iName[7] << 8) | item[ii]._iName[8];
			pHdr->dwItemSeed = (item[ii]._iName[9] << 24) | (item[ii]._iName[10] << 16) | (item[ii]._iName[11] << 8) | item[ii]._iName[12];
			pHdr->bItemId = item[ii]._iName[13];
			pHdr->bItemDur = item[ii]._iName[14];
			pHdr->bItemMDur = item[ii]._iName[15];
			pHdr->bItemCh = item[ii]._iName[16];
			pHdr->bItemMCh = item[ii]._iName[17];
			pHdr->wItemVal = (item[ii]._iName[18] << 8) | ((item[ii]._iCurs - 19) << 6) | item[ii]._ivalue;
			pHdr->dwItemBuff = (item[ii]._iName[19] << 24) | (item[ii]._iName[20] << 16) | (item[ii]._iName[21] << 8) | item[ii]._iName[22];
		} else {
			pHdr->wItemCI = item[ii]._iCreateInfo;
			pHdr->dwItemSeed = item[ii]._iSeed;
			pHdr->bItemId = item[ii]._iIdentified;
			pHdr->bItemDur = item[ii]._iDurability;
			pHdr->bItemMDur = item[ii]._iMaxDur;
			pHdr->bItemCh = item[ii]._iCharges;
			pHdr->bItemMCh = item[ii]._iMaxCharges;
			if (item[ii].IDidx == IDI_GOLD) {
				pHdr->wItemVal = item[ii]._ivalue;
			}
		}
	} else {
		pHdr->bItemI = -1;
	}

	assert((DWORD)sgnSyncPInv < NUM_INVLOC);
	pItem = &myplr().data.InvBody[sgnSyncPInv];
	if (pItem->_itype != ITYPE_NONE) {
		pHdr->bPInvLoc = sgnSyncPInv;
		pHdr->wPInvIndx = pItem->IDidx;
		pHdr->wPInvCI = pItem->_iCreateInfo;
		pHdr->dwPInvSeed = pItem->_iSeed;
		pHdr->bPInvId = pItem->_iIdentified;
	} else {
		pHdr->bPInvLoc = -1;
	}

	sgnSyncPInv++;
	if (sgnSyncPInv >= NUM_INVLOC) {
		sgnSyncPInv = 0;
	}
}

DWORD sync_update(int pnum, const BYTE *pbBuf)
{
	TSyncHeader *pHdr;
	WORD wLen;

	pHdr = (TSyncHeader *)pbBuf;
	pbBuf += sizeof(*pHdr);

	if (pHdr->bCmd != CMD_SYNCDATA) {
		app_fatal("bad sync command");
	}

	/// ASSERT: assert(gbBufferMsgs != BUFFER_PROCESS);

	if (gbBufferMsgs == 1) {
		return pHdr->wLen + sizeof(*pHdr);
	}
	if (pnum == myplr()) {
		return pHdr->wLen + sizeof(*pHdr);
	}

	for (wLen = pHdr->wLen; wLen >= sizeof(TSyncMonster); wLen -= sizeof(TSyncMonster)) {
		if (level.currlevel == pHdr->bLevel) {
			sync_monster(pnum, (TSyncMonster *)pbBuf);
		}
		delta_sync_monster((TSyncMonster *)pbBuf, pHdr->bLevel);
		pbBuf += sizeof(TSyncMonster);
	}

	assert(wLen == 0);

	return pHdr->wLen + sizeof(*pHdr);
}

void sync_monster(int pnum, const TSyncMonster *p)
{
	int i, ndx, md, mdx, mdy;
	DWORD delta;

	ndx = p->_mndx;

	if (monsters[ndx].data._mhitpoints == 0) {
		return;
	}

	for (i = 0; i < nummonsters; i++) {
		if (monstactive[i] == ndx) {
			break;
		}
	}

	delta = abs(myplr().data._px - monsters[ndx].data._mx) + abs(myplr().data._py - monsters[ndx].data._my);
	if (delta > 255) {
		delta = 255;
	}

	if (delta < p->_mdelta || (delta == p->_mdelta && pnum > myplr())) {
		return;
	}
	if (monsters[ndx].data._mfutx == p->_mx && monsters[ndx].data._mfuty == p->_my) {
		return;
	}
	if (monsters[ndx].data._mmode == MM_CHARGE || monsters[ndx].data._mmode == MM_STONE) {
		return;
	}

	mdx = abs(monsters[ndx].data._mx - p->_mx);
	mdy = abs(monsters[ndx].data._my - p->_my);
	if (mdx <= 2 && mdy <= 2) {
		if (monsters[ndx].data._mmode < MM_WALK || monsters[ndx].data._mmode > MM_WALK3) {
			md = GetDirection(monsters[ndx].data._mx, monsters[ndx].data._my, p->_mx, p->_my);
			if (monsters[ndx].DirOK(md)) {
				monsters[ndx].M_ClearSquares();
				grid[monsters[ndx].data._mx][monsters[ndx].data._my].dMonster = ndx + 1;
				monsters[ndx].M_WalkDir(md);
				monsters[ndx].data._msquelch = UCHAR_MAX;
			}
		}
	} else if (grid[p->_mx][p->_my].dMonster == 0) {
		monsters[ndx].M_ClearSquares();
		grid[p->_mx][p->_my].dMonster = ndx + 1;
		monsters[ndx].data._mx = p->_mx;
		monsters[ndx].data._my = p->_my;
		decode_enemy(ndx, p->_menemy);
		md = GetDirection(p->_mx, p->_my, monsters[ndx].data._menemyx, monsters[ndx].data._menemyy);
		monsters[ndx].M_StartStand(md);
		monsters[ndx].data._msquelch = UCHAR_MAX;
	}

	decode_enemy(ndx, p->_menemy);
}

void sync_init()
{
	sgnMonsters = 16 * myplr();
	memset(sgwLRU, 255, sizeof(sgwLRU));
}

DEVILUTION_END_NAMESPACE
