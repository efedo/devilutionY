/**
 * @file monster_manager.cpp
 *
 * Monster manager class (manages monster instances, not types)
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include <string>

namespace dvl {

void ActorManager::clearAllPlayers()
{
	for (auto & i : _actors) {
		if (i.get() && i.get()->type == ActorType::player) {
			i.reset();
		}
	}
	_players.clear();
}

void ActorManager::clearAllMonsters()
{
	for (auto& i : _actors) {
		if (i.get() && i.get()->type == ActorType::monster) {
			i.reset();
		}
	}
	_monsters.clear();
}

Monster& ActorManager::AddMonster(MonsterType mtype, V2Di pos, Dir dir, bool InMap)
{
	Monster* monster = new Monster(dir, mtype, pos);
	if (InMap) grid.at(pos).setActor(*monster);
	return *monster;
}

Monster & ActorManager::PlaceMonster(MonsterType mtype, V2Di pos)
{
	Dir rd = Dir(random_(90, 8));
	return AddMonster(mtype, pos, rd, true);
}

Monster & ActorManager::PlaceUniqueMonster(UniqueMonsterType uniqtype,
	MonsterType miniontype, int unpackfilesize)
{
	int i;
	bool zharflag, done;

	const UniqMonstStruct & uniq = beastiary.getUnique(uniqtype);

	if ((uniquetrans + 19) << 8 >= LIGHTSIZE) return;

	// Appears to check that there is room to place?
	int count, count2;
	V2Di p, n;
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
			if (count < 1000) continue;
		}

		if (MonstPlace(p)) break;
	}

	if (uniqtype == UniqueMonsterType::SNOTSPIL) {
		p.x = 2 * lvl.getpc().x + 24;
		p.y = 2 * lvl.getpc().y + 28;
	}
	if (uniqtype == UniqueMonsterType::WARLORD) {
		p.x = 2 * lvl.getpc().x + 22;
		p.y = 2 * lvl.getpc().y + 23;
	}
	if (uniqtype == UniqueMonsterType::ZHAR) {
		zharflag = true;
		for (i = 0; i < lvl.themeCount; i++) {
			if (i == zharlib && zharflag == true) {
				zharflag = false;
				p.x = 2 * lvl.themeLoc[i].x + 20;
				p.y = 2 * lvl.themeLoc[i].y + 20;
			}
		}
	}
	if (game.isSingleplayer()) {
		if (uniqtype == UniqueMonsterType::LAZURUS) {
			p = { 32, 46 };
		}
		if (uniqtype == UniqueMonsterType::RED_VEX) {
			p = { 40, 45 };
		}
		if (uniqtype == UniqueMonsterType::BLACKJADE) {
			p = { 38, 49 };
		}
		if (uniqtype == UniqueMonsterType::SKELKING) {
			p = { 35, 47 };
		}
	} else {
		if (uniqtype == UniqueMonsterType::LAZURUS) {
			p.x = 2 * lvl.getpc().x + 19;
			p.y = 2 * lvl.getpc().y + 22;
		}
		if (uniqtype == UniqueMonsterType::RED_VEX) {
			p.x = 2 * lvl.getpc().x + 21;
			p.y = 2 * lvl.getpc().y + 19;
		}
		if (uniqtype == UniqueMonsterType::BLACKJADE) {
			p.x = 2 * lvl.getpc().x + 21;
			p.y = 2 * lvl.getpc().y + 25;
		}
	}
	if (uniqtype == UniqueMonsterType::BUTCHER) {
		done = false;
		for (p.y = 0; p.y < MAXDUNY && !done; p.y++) {
			for (p.x = 0; p.x < MAXDUNX && !done; p.x++) {
				done = grid[p.x][p.y].getPiece() == 367;
			}
		}
	}

	const MonsterType mtype = uniq.mtype;
	Monster & monster = PlaceMonster(mtype, p);
	monster.data._uniqtype = uniqtype;

	if (uniq.mlevel) {
		monster.data.mLevel = 2 * uniq.mlevel;
	} else {
		monster.data.mLevel += 5;
	}

	monster.data.mExp *= 2;
	monster.data.mName = uniq.mName;
	monster.data._mmaxhp = uniq.mmaxhp << 6;

	if (game.isSingleplayer()) {
		monster.data._mmaxhp = monster.data._mmaxhp >> 1;
		if (monster.data._mmaxhp < 64) {
			monster.data._mmaxhp = 64;
		}
	}

	monster.data._mhitpoints = monster.data._mmaxhp;
	monster.data._mAi = uniq.mAi;
	monster.data._mint = uniq.mint;
	monster.data.mMinDamage = uniq.mMinDamage;
	monster.data.mMaxDamage = uniq.mMaxDamage;
	monster.data.mMinDamage2 = uniq.mMinDamage;
	monster.data.mMaxDamage2 = uniq.mMaxDamage;
	monster.data.mMagicRes = uniq.mMagicRes;
	monster.data.mtalkmsg = uniq.mtalkmsg;
	monster.data.mlid = AddLight(monster.data._m, 3);

	if (game.isMultiplayer()) {
		if (monster.data._mAi == MonstAi::LAZHELP)
			monster.data.mtalkmsg = 0;
		if (monster.data._mAi != MonstAi::LAZURUS || quests[QuestId::betrayer]._qvar1 <= 3) {
			if (monster.data.mtalkmsg) {
				monster.data._mgoal = MonsterGoal::INQUIRING;
			}
		} else {
			monster.data._mgoal = MonsterGoal::NORMAL;
		}
	} else if (monster.data.mtalkmsg)
		monster.data._mgoal = MonsterGoal::INQUIRING;

	if (gnDifficulty == Difficulty::NIGHTMARE) {
		monster.data._mmaxhp = 3 * monster.data._mmaxhp + 64;
		monster.data.mLevel += 15;
		monster.data._mhitpoints = monster.data._mmaxhp;
		monster.data.mExp = 2 * (monster.data.mExp + 1000);
		monster.data.mMinDamage = 2 * (monster.data.mMinDamage + 2);
		monster.data.mMaxDamage = 2 * (monster.data.mMaxDamage + 2);
		monster.data.mMinDamage2 = 2 * (monster.data.mMinDamage2 + 2);
		monster.data.mMaxDamage2 = 2 * (monster.data.mMaxDamage2 + 2);
	}

	if (gnDifficulty == Difficulty::HELL) {
		monster.data._mmaxhp = 4 * monster.data._mmaxhp + 192;
		monster.data.mLevel += 30;
		monster.data._mhitpoints = monster.data._mmaxhp;
		monster.data.mExp = 4 * (monster.data.mExp + 1000);
		monster.data.mMinDamage = 4 * monster.data.mMinDamage + 6;
		monster.data.mMaxDamage = 4 * monster.data.mMaxDamage + 6;
		monster.data.mMinDamage2 = 4 * monster.data.mMinDamage2 + 6;
		monster.data.mMaxDamage2 = 4 * monster.data.mMaxDamage2 + 6;
	}
	std::string filestr = "monsttypes\\monsttypes\\" + uniq.mTrnName + ".TRN";
	LoadFileWithMem(filestr, &pLightTbl[256 * (uniquetrans + 19)]);

	monster.data._uniqtrans = uniquetrans++;

	if (uniq.mUnqAttr & 4) {
		monster.data.mHit = uniq.mUnqVar1;
		monster.data.mHit2 = uniq.mUnqVar1;
	}
	if (uniq.mUnqAttr & 8) {
		monster.data.mArmorClass = uniq.mUnqVar1;
	}

	if (uniq.mUnqAttr & 1) {
		PlaceGroup(miniontype, unpackfilesize, uniq.mUnqAttr, monster);
	}

	if (monster.data._mAi != MonstAi::GARG) {
		monster.data._mAnimData = monster.data.MType->getAnim(MonstAnim::STAND).Data[int(monster.data._mdir)];
		monster.data._mAnimFrame = random_(88, monster.data._mAnimLen - 1) + 1;
		monster.data._mFlags &= ~MonsterFlag::allow_special;
		monster.data._mmode = MonsterMode::STAND;
	}
}

void ActorManager::PlaceQuestMonsters()
{
	int skeltype;
	uint8_t *setp;

	if (!lvl.setlevel) {
		if (QuestStatus(QuestId::butcher)) {
			PlaceUniqueMonster(UniqueMonsterType::BUTCHER, MonsterType::INVALID, 0);
		}

		if (lvl.currlevel == quests[QuestId::skelking]._qlevel && game.isMultiplayer()) {
			MonsterType skeltype = beastiary.getRandomActiveSkelType();
			if (skeltype == MonsterType::INVALID) app_fatal("No valid skeleton type for skeleton king creation ");
			PlaceUniqueMonster(UniqueMonsterType::SKELKING, skeltype, 30);
		}

		if (QuestStatus(QuestId::ltbanner)) {
			setp = LoadFileInMem("Levels\\L1Data\\Banner1.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(setp);
		}
		if (QuestStatus(QuestId::blood)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(setp);
		}
		if (QuestStatus(QuestId::blind)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(setp);
		}
		if (QuestStatus(QuestId::anvil)) {
			setp = LoadFileInMem("Levels\\L3Data\\Anvil.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x + 2, 2 * lvl.getpc().y + 2 });
			mem_free_dbg(setp);
		}
		if (QuestStatus(QuestId::warlord)) {
			setp = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(setp);
			beastiary.AddMonsterType(UniqMonst[UniqueMonsterType::WARLORD].mtype, 1);
		}
		if (QuestStatus(QuestId::veil)) {
			beastiary.AddMonsterType(UniqMonst[UniqueMonsterType::LACHDAN].mtype, 1);
		}
		if (QuestStatus(QuestId::zhar) && zharlib == -1) {
			quests[QuestId::zhar]._qactive = QuestState::not_available;
		}

		if (lvl.currlevel == quests[QuestId::betrayer]._qlevel && game.isMultiplayer()) {
			beastiary.AddMonsterType(UniqMonst[UniqueMonsterType::LAZURUS].mtype, 4);
			beastiary.AddMonsterType(UniqMonst[UniqueMonsterType::RED_VEX].mtype, 4);
			PlaceUniqueMonster(UniqueMonsterType::LAZURUS, 0, 0);
			PlaceUniqueMonster(UniqueMonsterType::RED_VEX, 0, 0);
			PlaceUniqueMonster(UniqueMonsterType::BLACKJADE, 0, 0);
			setp = LoadFileInMem("Levels\\L4Data\\Vile1.DUN", NULL);
			SetMapMonsters(setp, { 2 * lvl.getpc().x, 2 * lvl.getpc().y });
			mem_free_dbg(setp);
		}
	} else if (lvl.setlvlnum == SetLvl::SkelKing) {
		PlaceUniqueMonster(UniqueMonsterType::SKELKING, 0, 0);
	}
}

void ActorManager::PlaceGroup(MonsterType mtype, int num, int leaderf, Monster & leader)
{
	int j;
	V2Di p, v;
	int placed = 0;

	for (int try1 = 0; try1 < 10 && placed < num; try1++) {
		if (leaderf & 1) {
			int off = random_(92, 8);
			v = p = leader.data._m + offset(Dir(off));
		} else {
			do {
				v.x = p.x = random_(93, 80) + 16;
				v.y = p.y = random_(93, 80) + 16;
			} while (!MonstPlace(p));
		}

		j = 0;
		for (int try2 = 0; j < num && try2 < 100; p.x += offset(Dir(random_(94, 8))).x, p.y += offset(Dir(random_(94, 8))).y) { /// BUGFIX: `yp += offset_y`
			if (!MonstPlace(p)
			    || (grid.at(p).dTransVal != grid.at(v).dTransVal)
			    || (leaderf & 2) && ((p-v).maxabs() >= 4)) {
				try2++;
				continue;
			}

			Monster & plMonst = PlaceMonster(mtype, p);
			if (leaderf & 1) {
				plMonst.data._mmaxhp *= 2;
				plMonst.data._mhitpoints = plMonst.data._mmaxhp;
				plMonst.data._mint = plMonst.data._mint;

				if (leaderf & 2) {
					plMonst.data.leader = leader.id();
					plMonst.data.leaderflag = 1;
					plMonst.data._mAi = plMonst.data._mAi;
				}

				if (plMonst.data._mAi != MonstAi::GARG) {
					plMonst.data._mAnimData = plMonst.data.MType->getAnim(MonstAnim::STAND).Data[int(plMonst.data._mdir)];
					plMonst.data._mAnimFrame = random_(88, plMonst.data._mAnimLen - 1) + 1;
					plMonst.data._mFlags &= ~MonsterFlag::allow_special;
					plMonst.data._mmode = MonsterMode::STAND;
				}
			}
			placed++;
			j++;
		}
	}

	if (leaderf & 2) {
		leader.data.packsize = placed;
	}
}


Monster * ActorManager::M_SpawnSkel(V2Di pos, Dir dir)
{
	MonsterType skeltype = beastiary.getRandomActiveSkelType(false);
	if (skeltype != MonsterType::INVALID) return &AddMonster(skeltype, pos, dir, true);
	return 0;
}

Monster* ActorManager::PreSpawnSkeleton()
{
	MonsterType skeltype = beastiary.getRandomActiveSkelType(false);
	if (skeltype != MonsterType::INVALID) {
		Monster& monster = AddMonster(skeltype, { 0, 0 }, Dir(0), false);
		monster.M_StartStand(Dir(0));
		return &monster;
	}
	return 0;
}

bool ActorManager::SpawnSkeleton(int ii, V2Di pos)
{
	V2Di d, n;
	Dir dir;
	int j, k, rs;
	bool savail;
	int monstok[3][3];

	if (ii == -1) return false;

	if (PosOkMonst(-1, pos)) {
		dir = GetDirection(pos, pos); // ??? Gimmick to get random dir?
		monsters[ii].ActivateSpawn(pos, dir);
		return true;
	}

	savail = false;
	n.y = 0;
	for (j = pos.y - 1; j <= pos.y + 1; j++) {
		n.x = 0;
		for (k = pos.x - 1; k <= pos.x + 1; k++) {
			monstok[n.x][n.y] = PosOkMonst(-1, { k, j });
			savail |= monstok[n.x][n.y];
			n.x++;
		}
		n.y++;
	}
	if (!savail) {
		return false;
	}

	rs = random_(137, 15) + 1;
	n = { 0, 0 };
	while (rs > 0) {
		if (monstok[n.x][n.y])
			rs--;
		if (rs > 0) {
			n.x++;
			if (n.x == 3) {
				n.x = 0;
				n.y++;
				if (n.y == 3)
					n.y = 0;
			}
		}
	}

	d = pos + n - V2Di(1, 1);
	dir = GetDirection(d, pos);
	monsters[ii].ActivateSpawn(d, dir);
	return true;
}

void ActorManager::ProcessMonsters()
{
	for (auto & i : this->_monsters) {
		if (!i) continue;
		Monster& monst = *i;

		bool raflag = false;
		if (game.isMultiplayer()) {
			SetRndSeed(monst.data._mAISeed);
			monst.data._mAISeed = GetRndSeed();
		}
		if (!(monst.data._mFlags & MonsterFlag::noheal) && monst.data._mhitpoints < monst.data._mmaxhp && monst.data._mhitpoints >> 6 > 0) {
			if (monst.data.mLevel > 1) {
				monst.data._mhitpoints += monst.data.mLevel >> 1;
			}
			else {
				monst.data._mhitpoints += monst.data.mLevel;
			}
		}
		const V2Di m = monst.data._m;
		if (grid.at(m).dFlags & DunTileFlag::VISIBLE && monst.data._msquelch == 0) {
			if (monst.data.MType->mtype == MonsterType::CLEAVER) {
				PlaySFX(USFX_CLEAVER);
			}
		}
		Actor* enemy = this->getActor(monst.data._menemy);
		if (!enemy) app_fatal("Illegal enemy %d for monster \"%s\"", enemy, monst.data.mName);
		if (monst.data._mFlags & MonsterFlag::targets_monster) {
			if (enemy->type != ActorType::monster) app_fatal("Unexpected enemy type (enemy %d for monster \"%s\")", enemy, monst.data.mName);
			monst.data._last = static_cast<Monster *>(enemy)->data._mfut;
			monst.data._menemypos = monst.data._last;
		}
		else {
			if (enemy->type != ActorType::player) app_fatal("Unexpected enemy type (enemy %d for monster \"%s\")", enemy, monst.data.mName);
			monst.data._menemypos = static_cast<Player*>(enemy)->futpos();
			if (grid.at(m).dFlags & DunTileFlag::VISIBLE) {
				monst.data._msquelch = 255;
				monst.data._last = static_cast<Player*>(enemy)->futpos();
			}
			else if (monst.data._msquelch != 0 && monst.data.MType->mtype != MonsterType::DIABLO) { /// BUGFIX: change '_mAi' to 'MType->mtype'
				monst.data._msquelch--;
			}
		}
		do {
			if (!monst.ai_Path() || !(monst.data._mFlags & MonsterFlag::search)) {
				std::invoke(AiProc[int(monst.data._mAi)], &monst);
				//AiProc[monst.data._mAi](monsters[mi]);
			}
			switch (monst.data._mmode) {
			case MonsterMode::STAND:
				raflag = monst.M_DoStand();
				break;
			case MonsterMode::WALK:
				raflag = monst.M_DoWalk();
				break;
			case MonsterMode::WALK2:
				raflag = monst.M_DoWalk2();
				break;
			case MonsterMode::WALK3:
				raflag = monst.M_DoWalk3();
				break;
			case MonsterMode::ATTACK:
				raflag = monst.M_DoAttack();
				break;
			case MonsterMode::GOTHIT:
				raflag = monst.M_DoGotHit();
				break;
			case MonsterMode::DEATH:
				raflag = monst.M_DoDeath();
				break;
			case MonsterMode::SATTACK:
				raflag = monst.M_DoSAttack();
				break;
			case MonsterMode::FADEIN:
				raflag = monst.M_DoFadein();
				break;
			case MonsterMode::FADEOUT:
				raflag = monst.M_DoFadeout();
				break;
			case MonsterMode::RATTACK:
				raflag = monst.M_DoRAttack();
				break;
			case MonsterMode::SPSTAND:
				raflag = monst.M_DoSpStand();
				break;
			case MonsterMode::RSPATTACK:
				raflag = monst.M_DoRSpAttack();
				break;
			case MonsterMode::DELAY:
				raflag = monst.M_DoDelay();
				break;
			case MonsterMode::CHARGE:
				raflag = false;
				break;
			case MonsterMode::STONE:
				raflag = monst.M_DoStone();
				break;
			case MonsterMode::HEAL:
				raflag = monst.M_DoHeal();
				break;
			case MonsterMode::TALK:
				raflag = monst.M_DoTalk();
				break;
			}
			if (raflag) {
				monst.GroupUnity();
			}
		} while (raflag);
		if (monst.data._mmode != MonsterMode::STONE) {
			monst.data._mAnimCnt++;
			if (!(monst.data._mFlags & MonsterFlag::allow_special) && monst.data._mAnimCnt >= monst.data._mAnimDelay) {
				monst.data._mAnimCnt = 0;
				if (monst.data._mFlags & MonsterFlag::lock_animation) {
					monst.data._mAnimFrame--;
					if (monst.data._mAnimFrame == 0) {
						monst.data._mAnimFrame = monst.data._mAnimLen;
					}
				}
				else {
					monst.data._mAnimFrame++;
					if (monst.data._mAnimFrame > monst.data._mAnimLen) {
						monst.data._mAnimFrame = 1;
					}
				}
			}
		}
	}
}


}
