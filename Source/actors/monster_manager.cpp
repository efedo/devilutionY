/**
 * @file monster_manager.cpp
 *
 * Monster manager class (manages monster instances, not types)
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

void MonsterManager::ClrAllMonsters()
{
	int i;
	MonsterStruct *Monst;

	for (i = 0; i < MAXMONSTERS; i++) {
		Monst = &list[i].data;
		list[i].ClearMVars();
		Monst->mName = "Invalid Monster";
		Monst->_mgoal = 0;
		Monst->_mmode = MM_STAND;
		Monst->_mVar1 = 0;
		Monst->_mVar2 = 0;
		Monst->_m = { 0, 0 };
		Monst->_mfut = { 0, 0 };
		Monst->_mold = { 0, 0 };
		Monst->_mdir = Dir(random_(89, 8));
		Monst->_mvel = { 0, 0 };
		Monst->_mAnimData = NULL;
		Monst->_mAnimDelay = 0;
		Monst->_mAnimCnt = 0;
		Monst->_mAnimLen = 0;
		Monst->_mAnimFrame = 0;
		Monst->_mFlags = 0;
		Monst->_mDelFlag = FALSE;
		Monst->_menemy = random_(89, gbActivePlayers);
		Monst->_menemypos = plr[Monst->_menemy].futpos();
	}
}

MonsterInstance &MonsterManager::operator[](size_t n)
{
	if ((DWORD)n >= MAXMONSTERS) {
		app_fatal("Invalid monster %d selection", n);
	}
	return list[n];
}

void MonsterManager::PlaceMonster(int i, int mtype, V2Di pos)
{
	grid.at(pos).setMonster(i);
	Dir rd = Dir(random_(90, 8));
	list[i] = MonsterInstance(i, rd, mtype, pos);
}

void MonsterManager::PlaceUniqueMonster(int uniqindex, int miniontype, int unpackfilesize)
{
	V2Di p, n;
	int i;
	int uniqtype;
	int count2;
	char filestr[64];
	bool zharflag, done;
	UniqMonstStruct *Uniq;
	MonsterStruct *Monst;
	int count;

	//Monst = monster + nummonsters;
	Monst = &monsters[nummonsters].data;
	count = 0;
	Uniq = UniqMonst + uniqindex;

	if ((uniquetrans + 19) << 8 >= LIGHTSIZE)
		return;

	for (uniqtype = 0; uniqtype < nummtypes; uniqtype++) {
		if (beastiary[uniqtype].data.mtype == UniqMonst[uniqindex].mtype) {
			break;
		}
	}

	while (1) {
		p.x = random_(91, 80) + 16;
		p.y = random_(91, 80) + 16;
		count2 = 0;
		for (n.x = p.x - 3; n.x < p.x + 3; n.x++) {
			for (n.y = p.y - 3; n.y < p.y + 3; n.y++) {
				if (n.y >= 0 && n.y < MAXDUNY && n.x >= 0 && n.x < MAXDUNX && MonstPlace(n)) {
					count2++;
				}
			}
		}

		if (count2 < 9) {
			count++;
			if (count < 1000)
				continue;
		}

		if (MonstPlace(p))
			break;
	}

	if (uniqindex == UMT_SNOTSPIL) {
		p.x = 2 * lvl.getpc().x + 24;
		p.y = 2 * lvl.getpc().y + 28;
	}
	if (uniqindex == UMT_WARLORD) {
		p.x = 2 * lvl.getpc().x + 22;
		p.y = 2 * lvl.getpc().y + 23;
	}
	if (uniqindex == UMT_ZHAR) {
		zharflag = TRUE;
		for (i = 0; i < lvl.themeCount; i++) {
			if (i == zharlib && zharflag == TRUE) {
				zharflag = FALSE;
				p.x = 2 * lvl.themeLoc[i].x + 20;
				p.y = 2 * lvl.themeLoc[i].y + 20;
			}
		}
	}
	if (plr.isSingleplayer()) {
		if (uniqindex == UMT_LAZURUS) {
			p = { 32, 46 };
		}
		if (uniqindex == UMT_RED_VEX) {
			p = { 40, 45 };
		}
		if (uniqindex == UMT_BLACKJADE) {
			p = { 38, 49 };
		}
		if (uniqindex == UMT_SKELKING) {
			p = { 35, 47 };
		}
	} else {
		if (uniqindex == UMT_LAZURUS) {
			p.x = 2 * lvl.getpc().x + 19;
			p.y = 2 * lvl.getpc().y + 22;
		}
		if (uniqindex == UMT_RED_VEX) {
			p.x = 2 * lvl.getpc().x + 21;
			p.y = 2 * lvl.getpc().y + 19;
		}
		if (uniqindex == UMT_BLACKJADE) {
			p.x = 2 * lvl.getpc().x + 21;
			p.y = 2 * lvl.getpc().y + 25;
		}
	}
	if (uniqindex == UMT_BUTCHER) {
		done = FALSE;
		for (p.y = 0; p.y < MAXDUNY && !done; p.y++) {
			for (p.x = 0; p.x < MAXDUNX && !done; p.x++) {
				done = grid[p.x][p.y].getPiece() == 367;
			}
		}
	}

	PlaceMonster(nummonsters, uniqtype, p);
	Monst->_uniqtype = uniqindex + 1;

	if (Uniq->mlevel) {
		Monst->mLevel = 2 * Uniq->mlevel;
	} else {
		Monst->mLevel += 5;
	}

	Monst->mExp *= 2;
	Monst->mName = Uniq->mName;
	Monst->_mmaxhp = Uniq->mmaxhp << 6;

	if (plr.isSingleplayer()) {
		Monst->_mmaxhp = Monst->_mmaxhp >> 1;
		if (Monst->_mmaxhp < 64) {
			Monst->_mmaxhp = 64;
		}
	}

	Monst->_mhitpoints = Monst->_mmaxhp;
	Monst->_mAi = Uniq->mAi;
	Monst->_mint = Uniq->mint;
	Monst->mMinDamage = Uniq->mMinDamage;
	Monst->mMaxDamage = Uniq->mMaxDamage;
	Monst->mMinDamage2 = Uniq->mMinDamage;
	Monst->mMaxDamage2 = Uniq->mMaxDamage;
	Monst->mMagicRes = Uniq->mMagicRes;
	Monst->mtalkmsg = Uniq->mtalkmsg;
	Monst->mlid = AddLight(Monst->_m, 3);

	if (plr.isMultiplayer()) {
		if (Monst->_mAi == AI_LAZHELP)
			Monst->mtalkmsg = 0;
		if (Monst->_mAi != AI_LAZURUS || quests[Q_BETRAYER]._qvar1 <= 3) {
			if (Monst->mtalkmsg) {
				Monst->_mgoal = MGOAL_INQUIRING;
			}
		} else {
			Monst->_mgoal = MGOAL_NORMAL;
		}
	} else if (Monst->mtalkmsg)
		Monst->_mgoal = MGOAL_INQUIRING;

	if (gnDifficulty == DIFF_NIGHTMARE) {
		Monst->_mmaxhp = 3 * Monst->_mmaxhp + 64;
		Monst->mLevel += 15;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 2 * (Monst->mExp + 1000);
		Monst->mMinDamage = 2 * (Monst->mMinDamage + 2);
		Monst->mMaxDamage = 2 * (Monst->mMaxDamage + 2);
		Monst->mMinDamage2 = 2 * (Monst->mMinDamage2 + 2);
		Monst->mMaxDamage2 = 2 * (Monst->mMaxDamage2 + 2);
	}

	if (gnDifficulty == DIFF_HELL) {
		Monst->_mmaxhp = 4 * Monst->_mmaxhp + 192;
		Monst->mLevel += 30;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 4 * (Monst->mExp + 1000);
		Monst->mMinDamage = 4 * Monst->mMinDamage + 6;
		Monst->mMaxDamage = 4 * Monst->mMaxDamage + 6;
		Monst->mMinDamage2 = 4 * Monst->mMinDamage2 + 6;
		Monst->mMaxDamage2 = 4 * Monst->mMaxDamage2 + 6;
	}

	sprintf(filestr, "monsttypes\\monsttypes\\%s.TRN", Uniq->mTrnName);
	LoadFileWithMem(filestr, &pLightTbl[256 * (uniquetrans + 19)]);

	Monst->_uniqtrans = uniquetrans++;

	if (Uniq->mUnqAttr & 4) {
		Monst->mHit = Uniq->mUnqVar1;
		Monst->mHit2 = Uniq->mUnqVar1;
	}
	if (Uniq->mUnqAttr & 8) {
		Monst->mArmorClass = Uniq->mUnqVar1;
	}

	nummonsters++;

	if (Uniq->mUnqAttr & 1) {
		PlaceGroup(miniontype, unpackfilesize, Uniq->mUnqAttr, nummonsters - 1);
	}

	if (Monst->_mAi != AI_GARG) {
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[int(Monst->_mdir)];
		Monst->_mAnimFrame = random_(88, Monst->_mAnimLen - 1) + 1;
		Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		Monst->_mmode = MM_STAND;
	}
}

void MonsterManager::PlaceQuestMonsters()
{
	int skeltype;
	uint8_t *setp;

	if (!lvl.setlevel) {
		if (QuestStatus(Q_BUTCHER)) {
			PlaceUniqueMonster(UMT_BUTCHER, 0, 0);
		}

		if (lvl.currlevel == quests[Q_SKELKING]._qlevel && plr.isMultiplayer()) {
			skeltype = 0;

			for (skeltype = 0; skeltype < nummtypes; skeltype++) {
				if (IsSkel(beastiary[skeltype].data.mtype)) {
					break;
				}
			}

			PlaceUniqueMonster(UMT_SKELKING, skeltype, 30);
		}

		if (QuestStatus(Q_LTBANNER)) {
			setp = LoadFileInMem("Levels\\L1Data\\Banner1.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_BLOOD)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_BLIND)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_ANVIL)) {
			setp = LoadFileInMem("Levels\\L3Data\\Anvil.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x + 2, 2 * lvl.getpc().y + 2 });
			mem_free_dbg(setp);
		}
		if (QuestStatus(Q_WARLORD)) {
			setp = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(setp);
			beastiary.AddMonsterType(UniqMonst[UMT_WARLORD].mtype, 1);
		}
		if (QuestStatus(Q_VEIL)) {
			beastiary.AddMonsterType(UniqMonst[UMT_LACHDAN].mtype, 1);
		}
		if (QuestStatus(Q_ZHAR) && zharlib == -1) {
			quests[Q_ZHAR]._qactive = QUEST_NOTAVAIL;
		}

		if (lvl.currlevel == quests[Q_BETRAYER]._qlevel && plr.isMultiplayer()) {
			beastiary.AddMonsterType(UniqMonst[UMT_LAZURUS].mtype, 4);
			beastiary.AddMonsterType(UniqMonst[UMT_RED_VEX].mtype, 4);
			PlaceUniqueMonster(UMT_LAZURUS, 0, 0);
			PlaceUniqueMonster(UMT_RED_VEX, 0, 0);
			PlaceUniqueMonster(UMT_BLACKJADE, 0, 0);
			setp = LoadFileInMem("Levels\\L4Data\\Vile1.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(setp);
		}
	} else if (lvl.setlvlnum == SetLvl::SkelKing) {
		PlaceUniqueMonster(UMT_SKELKING, 0, 0);
	}
}

void MonsterManager::PlaceGroup(int mtype, int num, int leaderf, int leader)
{
	int j;
	V2Di p, v;
	int placed = 0;

	for (int try1 = 0; try1 < 10; try1++) {
		while (placed) {
			nummonsters--;
			placed--;
			grid.at(list[nummonsters].data._m).clearMonster();
		}

		if (leaderf & 1) {
			int off = random_(92, 8);
			v = p = list[leader].data._m + offset(Dir(off));
		} else {
			do {
				v.x = p.x = random_(93, 80) + 16;
				v.y = p.y = random_(93, 80) + 16;
			} while (!MonstPlace(p));
		}

		if (num + nummonsters > totalmonsters) {
			num = totalmonsters - nummonsters;
		}

		j = 0;
		for (int try2 = 0; j < num && try2 < 100; p.x += offset(Dir(random_(94, 8))).x, p.y += offset(Dir(random_(94, 8))).y) { /// BUGFIX: `yp += offset_y`
			if (!MonstPlace(p)
			    || (grid.at(p).dTransVal != grid.at(v).dTransVal)
			    || (leaderf & 2) && ((p-v).maxabs() >= 4)) {
				try2++;
				continue;
			}

			PlaceMonster(nummonsters, mtype, p);
			if (leaderf & 1) {
				list[nummonsters].data._mmaxhp *= 2;
				list[nummonsters].data._mhitpoints = list[nummonsters].data._mmaxhp;
				list[nummonsters].data._mint = list[leader].data._mint;

				if (leaderf & 2) {
					list[nummonsters].data.leader = leader;
					list[nummonsters].data.leaderflag = 1;
					list[nummonsters].data._mAi = list[leader].data._mAi;
				}

				if (list[nummonsters].data._mAi != AI_GARG) {
					list[nummonsters].data._mAnimData = list[nummonsters].data.MType->Anims[MA_STAND].Data[int(list[nummonsters].data._mdir)];
					list[nummonsters].data._mAnimFrame = random_(88, list[nummonsters].data._mAnimLen - 1) + 1;
					list[nummonsters].data._mFlags &= ~MFLAG_ALLOW_SPECIAL;
					list[nummonsters].data._mmode = MM_STAND;
				}
			}
			nummonsters++;
			placed++;
			j++;
		}

		if (placed >= num) {
			break;
		}
	}

	if (leaderf & 2) {
		list[leader].data.packsize = placed;
	}
}

void MonsterManager::DeleteMonster(int i)
{
	int temp;

	nummonsters--;
	temp = monstactive[nummonsters];
	monstactive[nummonsters] = monstactive[i];
	monstactive[i] = temp;
}

DEVILUTION_END_NAMESPACE
