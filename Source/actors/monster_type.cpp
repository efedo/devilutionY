/**
 * @file monster_type.cpp
 *
 * Monster type class
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

void MonsterType::InitMonsterTRN(bool special)
{
	BYTE *f;
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
		if (i != 1 || data.mtype < MT_COUNSLR || data.mtype > MT_ADVOCATE) {
			for (j = 0; j < 8; j++) {
				Cl2ApplyTrans(
				    data.Anims[i].Data[j],
				    data.trans_file,
				    data.Anims[i].Frames);
			}
		}
	}
}

void MonsterType::InitMonsterGFX()
{
	int mtype, anim, i;
	char strBuff[256];
	BYTE *celBuf;

	mtype = data.mtype;

	for (anim = 0; anim < 6; anim++) {
		if ((animletter[anim] != 's' || monsterdata[mtype].has_special) && monsterdata[mtype].Frames[anim] > 0) {
			sprintf(strBuff, monsterdata[mtype].GraphicType, animletter[anim]);

			celBuf = LoadFileInMem(strBuff, NULL);
			data.Anims[anim].CMem = celBuf;

			if (data.mtype != MT_GOLEM || (animletter[anim] != 's' && animletter[anim] != 'd')) {

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

	if (mtype >= MT_NMAGMA && mtype <= MT_WMAGMA && !(MissileFileFlag & 1)) {
		MissileFileFlag |= 1;
		LoadMissileGFX(MFILE_MAGBALL);
	}
	if (mtype >= MT_STORM && mtype <= MT_MAEL && !(MissileFileFlag & 2)) {
		MissileFileFlag |= 2;
		LoadMissileGFX(MFILE_THINLGHT);
	}
	if (mtype == MT_SUCCUBUS && !(MissileFileFlag & 4)) {
		MissileFileFlag |= 4;
		LoadMissileGFX(MFILE_FLARE);
		LoadMissileGFX(MFILE_FLAREEXP);
	}
	if (mtype == MT_SNOWWICH && !(MissileFileFlag & 0x20)) {
		MissileFileFlag |= 0x20;
		LoadMissileGFX(MFILE_SCUBMISB);
		LoadMissileGFX(MFILE_SCBSEXPB);
	}
	if (mtype == MT_HLSPWN && !(MissileFileFlag & 0x40)) {
		MissileFileFlag |= 0x40;
		LoadMissileGFX(MFILE_SCUBMISD);
		LoadMissileGFX(MFILE_SCBSEXPD);
	}
	if (mtype == MT_SOLBRNR && !(MissileFileFlag & 0x80)) {
		MissileFileFlag |= 0x80;
		LoadMissileGFX(MFILE_SCUBMISC);
		LoadMissileGFX(MFILE_SCBSEXPC);
	}
	if (mtype >= MT_INCIN && mtype <= MT_HELLBURN && !(MissileFileFlag & 8)) {
		MissileFileFlag |= 8;
		LoadMissileGFX(MFILE_KRULL);
	}
	if (mtype >= MT_NACID && mtype <= MT_XACID && !(MissileFileFlag & 0x10)) {
		MissileFileFlag |= 0x10;
		LoadMissileGFX(MFILE_ACIDBF);
		LoadMissileGFX(MFILE_ACIDSPLA);
		LoadMissileGFX(MFILE_ACIDPUD);
	}
	if (mtype == MT_DIABLO) {
		LoadMissileGFX(MFILE_FIREPLAR);
	}
}

DEVILUTION_END_NAMESPACE
