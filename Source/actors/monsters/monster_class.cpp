/**
 * @file monster_type.cpp
 *
 * Monster type class
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

namespace dvl {

const char MonsterClass::animletter[7] = "nwahds";

void MonsterClass::InitMonsterTRN(bool special)
{
	uint8_t* f = data.trans_file;
	for (int i = 0; i < 256; i++) {
		if (*f == 255) {
			*f = 0;
		}
		f++;
	}

	int n = special ? 6 : 5;
	for (int i = 0; i < n; i++) {
		if (i != 1 || data.mtype < MonsterType::COUNSLR || data.mtype > MonsterType::ADVOCATE) {
			for (int j = 0; j < 8; j++) {
				Cl2ApplyTrans(
					data.getAnim(MonstAnim(i)).Data[j],
				    data.trans_file,
				    data.getAnim(MonstAnim(i)).Frames);
			}
		}
	}
}

char MonsterClass::animLetter(MonstAnim anim) {
	return animletter[int(anim)];
}

void MonsterClass::InitMonsterGFX()
{
	int i;
	char strBuff[256];
	uint8_t *celBuf;

	for (int j = 0; j < 6; j++) {
		MonstAnim anim = MonstAnim(j);

		if ((animLetter(anim) != 's' || cdata().has_special) && cdata().Frames[int(anim)] > 0) {
			sprintf(strBuff, cdata().GraphicType, animLetter(anim));

			celBuf = LoadFileInMem(strBuff, NULL);
			data.getAnim(anim).CMem = celBuf;

			if (data.mtype != MonsterType::GOLEM || (animLetter(anim) != 's' && animLetter(anim) != 'd')) {

				for (i = 0; i < 8; i++) {
					data.getAnim(anim).Data[i] = CelGetFrameStart(celBuf, i);
				}
			} else {
				for (i = 0; i < 8; i++) {
					data.getAnim(anim).Data[i] = celBuf;
				}
			}
		}

		// TODO: either the AnimStruct members have wrong naming or the MonsterData ones it seems
		data.getAnim(anim).Frames = cdata().Frames[int(anim)];
		data.getAnim(anim).Rate = cdata().Rate[int(anim)];
	}

	data.width = cdata().width;
	data.width2 = (cdata().width - 64) >> 1;
	data.mMinHP = cdata().mMinHP;
	data.mMaxHP = cdata().mMaxHP;
	data.has_special = cdata().has_special;
	data.mAFNum = cdata().mAFNum;
	data.MData = &cdata();

	if (cdata().has_trans) {
		data.trans_file = LoadFileInMem(cdata().TransFile, NULL);
		InitMonsterTRN(cdata().has_special);
		MemFreeDbg(data.trans_file);
	}

	const MonsterType mtype = data.mtype;
	if (mtype >= MonsterType::NMAGMA && mtype <= MonsterType::WMAGMA && !(MissileFileFlag & 1)) {
		MissileFileFlag |= 1;
		LoadMissileGFX(MissileGraphic::MAGBALL);
	}
	if (mtype >= MonsterType::STORM && mtype <= MonsterType::MAEL && !(MissileFileFlag & 2)) {
		MissileFileFlag |= 2;
		LoadMissileGFX(MissileGraphic::THINLGHT);
	}
	if (mtype == MonsterType::SUCCUBUS && !(MissileFileFlag & 4)) {
		MissileFileFlag |= 4;
		LoadMissileGFX(MissileGraphic::FLARE);
		LoadMissileGFX(MissileGraphic::FLAREEXP);
	}
	if (mtype == MonsterType::SNOWWICH && !(MissileFileFlag & 0x20)) {
		MissileFileFlag |= 0x20;
		LoadMissileGFX(MissileGraphic::SCUBMISB);
		LoadMissileGFX(MissileGraphic::SCBSEXPB);
	}
	if (mtype == MonsterType::HLSPWN && !(MissileFileFlag & 0x40)) {
		MissileFileFlag |= 0x40;
		LoadMissileGFX(MissileGraphic::SCUBMISD);
		LoadMissileGFX(MissileGraphic::SCBSEXPD);
	}
	if (mtype == MonsterType::SOLBRNR && !(MissileFileFlag & 0x80)) {
		MissileFileFlag |= 0x80;
		LoadMissileGFX(MissileGraphic::SCUBMISC);
		LoadMissileGFX(MissileGraphic::SCBSEXPC);
	}
	if (mtype >= MonsterType::INCIN && mtype <= MonsterType::HELLBURN && !(MissileFileFlag & 8)) {
		MissileFileFlag |= 8;
		LoadMissileGFX(MissileGraphic::KRULL);
	}
	if (mtype >= MonsterType::NACID && mtype <= MonsterType::XACID && !(MissileFileFlag & 0x10)) {
		MissileFileFlag |= 0x10;
		LoadMissileGFX(MissileGraphic::ACIDBF);
		LoadMissileGFX(MissileGraphic::ACIDSPLA);
		LoadMissileGFX(MissileGraphic::ACIDPUD);
	}
	if (mtype == MonsterType::DIABLO) {
		LoadMissileGFX(MissileGraphic::FIREPLAR);
	}
}

CMonster MonsterClass::vdata() {
	return data;
}

const MonsterData MonsterClass::cdata() {
	return Beastiary::monsterdata[int(data.mtype)];
}


void MonsterClass::InitMonsterSND()
{
	if (!gbSndInited) return;
	MonsterType mtype = data.mtype;
	for (int i = 0; i < 4; i++) {
		if (MonstSndChar[i] != 's' || cdata().snd_special) {
			for (int j = 0; j < 2; j++) {
				char name[MAX_PATH];
				sprintf(name, cdata().sndfile, MonstSndChar[i], j + 1);
				char* path = (char*)DiabloAllocPtr(strlen(name) + 1);
				strcpy(path, name);
				TSnd* pSnd = sound_file_load(path);
				data.Snds[i][j] = pSnd;
				if (!pSnd)
					mem_free_dbg(path);
			}
		}
	}
}


bool MonsterClass::IsSkel()
{
	const MonsterType mt = data.mtype;
	return mt >= MonsterType::WSKELAX && mt <= MonsterType::XSKELAX
		|| mt >= MonsterType::WSKELBW && mt <= MonsterType::XSKELBW
		|| mt >= MonsterType::WSKELSD && mt <= MonsterType::XSKELSD;
}

bool MonsterClass::IsGoat()
{
	const MonsterType mt = data.mtype;
	return mt >= MonsterType::NGOATMC && mt <= MonsterType::GGOATMC
		|| mt >= MonsterType::NGOATBW && mt <= MonsterType::GGOATBW;
}

void MonsterClass::PrintMonstHistory()
{
	int minHP, maxHP, res;

	sprintf(tempstr, "Total kills : %i", this->kills);
	AddPanelString(tempstr, true);
	if (this->kills >= 30) {
		minHP = cdata().mMinHP;
		maxHP = cdata().mMaxHP;
		if (game.isSingleplayer()) {
			minHP = cdata().mMinHP >> 1;
			maxHP = cdata().mMaxHP >> 1;
		}
		if (minHP < 1)
			minHP = 1;
		if (maxHP < 1)
			maxHP = 1;
		if (gnDifficulty == Difficulty::NIGHTMARE) {
			minHP = 3 * minHP + 1;
			maxHP = 3 * maxHP + 1;
		}
		if (gnDifficulty == Difficulty::HELL) {
			minHP = 4 * minHP + 3;
			maxHP = 4 * maxHP + 3;
		}
		sprintf(tempstr, "Hit Points : %i-%i", minHP, maxHP);
		AddPanelString(tempstr, true);
	}
	if (this->kills >= 15) {
		if (gnDifficulty != Difficulty::HELL)
			res = cdata().mMagicRes;
		else
			res = cdata().mMagicRes2;
		res = res & (MonsterResistFlag::resist_magic | MonsterResistFlag::resist_fire | MonsterResistFlag::resist_lightning | MonsterResistFlag::imune_magic | MonsterResistFlag::imune_fire | MonsterResistFlag::imune_lightning);
		if (!res) {
			strcpy(tempstr, "No magic resistance");
			AddPanelString(tempstr, true);
		}
		else {
			if (res & (MonsterResistFlag::resist_magic | MonsterResistFlag::resist_fire | MonsterResistFlag::resist_lightning)) {
				strcpy(tempstr, "Resists : ");
				if (res & MonsterResistFlag::resist_magic)
					strcat(tempstr, "Magic ");
				if (res & MonsterResistFlag::resist_fire)
					strcat(tempstr, "Fire ");
				if (res & MonsterResistFlag::resist_lightning)
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, true);
			}
			if (res & (MonsterResistFlag::imune_magic | MonsterResistFlag::imune_fire | MonsterResistFlag::imune_lightning)) {
				strcpy(tempstr, "Immune : ");
				if (res & MonsterResistFlag::imune_magic)
					strcat(tempstr, "Magic ");
				if (res & MonsterResistFlag::imune_fire)
					strcat(tempstr, "Fire ");
				if (res & MonsterResistFlag::imune_lightning)
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, true);
			}
		}
	}
	pinfoflag = true;
}


}
