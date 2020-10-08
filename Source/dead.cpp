/**
 * @file dead.cpp
 *
 * Implementation of functions for placing dead monsters.
 */
#include "all.h"

namespace dvl {

/** unused, this was probably for blood boil/burn */
int spurtndx;
DeadStruct dead[MAXDEAD];
int stonendx;

void InitDead()
{
	int i, d, nd, mi;
	int mtypes[MAXMONSTERS];

	for (i = 0; i < MAXMONSTERS; i++)
		mtypes[i] = 0;

	nd = 0;

	for (i = 0; i < nummtypes; i++) {
		if (mtypes[beastiary[i].data.mtype] == 0) {
			for (d = 0; d < 8; d++)
				dead[nd]._deadData[d] = beastiary[i].data.Anims[MonstAnim::DEATH].Data[d];
			dead[nd]._deadFrame = beastiary[i].data.Anims[MonstAnim::DEATH].Frames;
			dead[nd]._deadWidth = beastiary[i].data.width;
			dead[nd]._deadWidth2 = beastiary[i].data.width2;
			dead[nd]._deadtrans = 0;
			beastiary[i].data.mdeadval = nd + 1;
			mtypes[beastiary[i].data.mtype] = nd + 1;
			nd++;
		}
	}

	for (d = 0; d < 8; d++)
		dead[nd]._deadData[d] = misfiledata[MissileGraphic::BLODBUR].mAnimData[0];
	dead[nd]._deadFrame = 8;
	dead[nd]._deadWidth = 128;
	dead[nd]._deadWidth2 = 32;
	dead[nd]._deadtrans = 0;
	spurtndx = nd + 1;
	nd++;

	for (d = 0; d < 8; d++)
		dead[nd]._deadData[d] = misfiledata[MissileGraphic::SHATTER1].mAnimData[0];
	dead[nd]._deadFrame = 12;
	dead[nd]._deadWidth = 128;
	dead[nd]._deadWidth2 = 32;
	dead[nd]._deadtrans = 0;
	stonendx = nd + 1;
	nd++;

	for (i = 0; i < nummonsters; i++) {
		mi = monstactive[i];
		if (monsters[mi].data._uniqtype != 0) {
			for (d = 0; d < 8; d++)
				dead[nd]._deadData[d] = monsters[mi].data.MType->Anims[MonstAnim::DEATH].Data[d];
			dead[nd]._deadFrame = monsters[mi].data.MType->Anims[MonstAnim::DEATH].Frames;
			dead[nd]._deadWidth = monsters[mi].data.MType->width;
			dead[nd]._deadWidth2 = monsters[mi].data.MType->width2;
			dead[nd]._deadtrans = monsters[mi].data._uniqtrans + 4;
			monsters[mi].data._udeadval = nd + 1;
			nd++;
		}
	}

	assert(nd <= MAXDEAD);
}

void AddDead(V2Di pos, char dv, Dir ddir)
{
	grid[pos.x][pos.y].dDead = (dv & 0x1F) + (int(ddir) << 5);
}

void SetDead()
{
	int i, mi;
	V2Di d;

	for (i = 0; i < nummonsters; i++) {
		mi = monstactive[i];
		if (monsters[mi].data._uniqtype != 0) {
			for (d.x = 0; d.x < MAXDUNX; d.x++) {
				for (d.y = 0; d.y < MAXDUNY; d.y++) {
					if ((grid[d.x][d.y].dDead & 0x1F) == monsters[mi].data._udeadval)
						ChangeLightXY(monsters[mi].data.mlid, d);
				}
			}
		}
	}
}

}
