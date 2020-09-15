/**
 * @file dead.cpp
 *
 * Implementation of functions for placing dead monsters.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

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
				dead[nd]._deadData[d] = beastiary[i].data.Anims[MA_DEATH].Data[d];
			dead[nd]._deadFrame = beastiary[i].data.Anims[MA_DEATH].Frames;
			dead[nd]._deadWidth = beastiary[i].data.width;
			dead[nd]._deadWidth2 = beastiary[i].data.width2;
			dead[nd]._deadtrans = 0;
			beastiary[i].data.mdeadval = nd + 1;
			mtypes[beastiary[i].data.mtype] = nd + 1;
			nd++;
		}
	}

	for (d = 0; d < 8; d++)
		dead[nd]._deadData[d] = misfiledata[MFILE_BLODBUR].mAnimData[0];
	dead[nd]._deadFrame = 8;
	dead[nd]._deadWidth = 128;
	dead[nd]._deadWidth2 = 32;
	dead[nd]._deadtrans = 0;
	spurtndx = nd + 1;
	nd++;

	for (d = 0; d < 8; d++)
		dead[nd]._deadData[d] = misfiledata[MFILE_SHATTER1].mAnimData[0];
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
				dead[nd]._deadData[d] = monsters[mi].data.MType->Anims[MA_DEATH].Data[d];
			dead[nd]._deadFrame = monsters[mi].data.MType->Anims[MA_DEATH].Frames;
			dead[nd]._deadWidth = monsters[mi].data.MType->width;
			dead[nd]._deadWidth2 = monsters[mi].data.MType->width2;
			dead[nd]._deadtrans = monsters[mi].data._uniqtrans + 4;
			monsters[mi].data._udeadval = nd + 1;
			nd++;
		}
	}

	assert(nd <= MAXDEAD);
}

void AddDead(int dx, int dy, char dv, int ddir)
{
	grid[dx][dy].dDead = (dv & 0x1F) + (ddir << 5);
}

void SetDead()
{
	int i, mi;
	int dx, dy;

	for (i = 0; i < nummonsters; i++) {
		mi = monstactive[i];
		if (monsters[mi].data._uniqtype != 0) {
			for (dx = 0; dx < MAXDUNX; dx++) {
				for (dy = 0; dy < MAXDUNY; dy++) {
					if ((grid[dx][dy].dDead & 0x1F) == monsters[mi].data._udeadval)
						ChangeLightXY(monsters[mi].data.mlid, dx, dy);
				}
			}
		}
	}
}

DEVILUTION_END_NAMESPACE
