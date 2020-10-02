/**
 * @file monster_type.cpp
 *
 * Monster type class
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

void MonsterClass::InitMonsterTRN(bool special)
{
	uint8_t *f;
	int i, n, j;

	f = data.trans_file;
	for (i = 0; i < 256; i++) {
		if (*f == 255) {
			*f = 0;
		}
		f++;
	}

	n = special ? 6 : 5;
	for (i = 0; i < n; i++) {
		if (i != 1 || data.mtype < MonsterType::COUNSLR || data.mtype > MonsterType::ADVOCATE) {
			for (j = 0; j < 8; j++) {
				Cl2ApplyTrans(
				    data.Anims[i].Data[j],
				    data.trans_file,
				    data.Anims[i].Frames);
			}
		}
	}
}

void MonsterClass::InitMonsterGFX()
{
	int mtype, anim, i;
	char strBuff[256];
	uint8_t *celBuf;

	mtype = data.mtype;

	for (anim = 0; anim < 6; anim++) {
		if ((animletter[anim] != 's' || monsterdata[mtype].has_special) && monsterdata[mtype].Frames[anim] > 0) {
			sprintf(strBuff, monsterdata[mtype].GraphicType, animletter[anim]);

			celBuf = LoadFileInMem(strBuff, NULL);
			data.Anims[anim].CMem = celBuf;

			if (data.mtype != MonsterFlag::golem || (animletter[anim] != 's' && animletter[anim] != 'd')) {

				for (i = 0; i < 8; i++) {
					data.Anims[anim].Data[i] = CelGetFrameStart(celBuf, i);
				}
			} else {
				for (i = 0; i < 8; i++) {
					data.Anims[anim].Data[i] = celBuf;
				}
			}
		}

		// TODO: either the AnimStruct members have wrong naming or the MonsterData ones it seems
		data.Anims[anim].Frames = monsterdata[mtype].Frames[anim];
		data.Anims[anim].Rate = monsterdata[mtype].Rate[anim];
	}

	data.width = monsterdata[mtype].width;
	data.width2 = (monsterdata[mtype].width - 64) >> 1;
	data.mMinHP = monsterdata[mtype].mMinHP;
	data.mMaxHP = monsterdata[mtype].mMaxHP;
	data.has_special = monsterdata[mtype].has_special;
	data.mAFNum = monsterdata[mtype].mAFNum;
	data.MData = &monsterdata[mtype];

	if (monsterdata[mtype].has_trans) {
		data.trans_file = LoadFileInMem(monsterdata[mtype].TransFile, NULL);
		InitMonsterTRN(monsterdata[mtype].has_special);
		MemFreeDbg(data.trans_file);
	}

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

DEVILUTION_END_NAMESPACE
