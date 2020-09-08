#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

/** In-game state of portals. */
PortalStruct portal[MAXPORTAL];
/** Current portal number (a portal array index). */
int portalindex;

/** X-coordinate of each players portal in town. */
int WarpDropX[MAXPORTAL] = { 57, 59, 61, 63 };
/** Y-coordinate of each players portal in town. */
int WarpDropY[MAXPORTAL] = { 40, 40, 40, 40 };

void InitPortals()
{
	int i;
	for (i = 0; i < MAXPORTAL; i++) {
		if (delta_portal_inited(i))
			portal[i].open = FALSE;
	}
}

void SetPortalStats(int i, BOOL o, int x, int y, int lvl, int lvltype)
{
	portal[i].open = o;
	portal[i].x = x;
	portal[i].y = y;
	portal[i].level = lvl;
	portal[i].ltype = lvltype;
	portal[i].setlvl = FALSE;
}

void AddWarpMissile(int i, int x, int y)
{
	int mi;

	missiledata[MIS_TOWN].mlSFX = -1;
	grid[x][y].dMissile = 0;
	mi = AddMissile(0, 0, x, y, 0, MIS_TOWN, 0, i, 0, 0);

	if (mi != -1) {
		SetMissDir(mi, 1);

		if (level.currlevel != 0)
			missile[mi]._mlid = AddLight(missile[mi]._mix, missile[mi]._miy, 15);

		missiledata[MIS_TOWN].mlSFX = LS_SENTINEL;
	}
}

void SyncPortals()
{
	int i;

	for (i = 0; i < MAXPORTAL; i++) {
		if (!portal[i].open)
			continue;
		if (level.currlevel == 0)
			AddWarpMissile(i, WarpDropX[i], WarpDropY[i]);
		else {
			int lvl = level.currlevel;
			if (level.setlevel)
				lvl = level.setlvlnum;
			if (portal[i].level == lvl)
				AddWarpMissile(i, portal[i].x, portal[i].y);
		}
	}
}

void AddInTownPortal(int i)
{
	AddWarpMissile(i, WarpDropX[i], WarpDropY[i]);
}

void ActivatePortal(int i, int x, int y, int lvl, int lvltype, BOOL sp)
{
	portal[i].open = TRUE;

	if (lvl != 0) {
		portal[i].x = x;
		portal[i].y = y;
		portal[i].level = lvl;
		portal[i].ltype = lvltype;
		portal[i].setlvl = sp;
	}
}

void DeactivatePortal(int i)
{
	portal[i].open = FALSE;
}

BOOL PortalOnLevel(int i)
{
	if (portal[i].level == level.currlevel)
		return TRUE;
	else
		return level.currlevel == 0;
}

void RemovePortalMissile(int id)
{
	int i;
	int mi;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		if (missile[mi]._mitype == MIS_TOWN && missile[mi]._misource == id) {
			grid[missile[mi]._mix][missile[mi]._miy].dFlags &= ~BFLAG_MISSILE;
			grid[missile[mi]._mix][missile[mi]._miy].dMissile = 0;

			if (portal[id].level != 0)
				AddUnLight(missile[mi]._mlid);

			DeleteMissile(mi, i);
		}
	}
}

void SetCurrentPortal(int p)
{
	portalindex = p;
}

void GetPortalLevel()
{
	if (level.currlevel) {
		level.setlevel = FALSE;
		level.currlevel = 0;
		plr[myplr].plrlevel = 0;
		level.leveltype = DTYPE_TOWN;
	} else {
		if (portal[portalindex].setlvl) {
			level.setlevel = TRUE;
			level.setlvlnum = portal[portalindex].level;
			level.currlevel = portal[portalindex].level;
			plr[myplr].plrlevel = level.setlvlnum;
			level.leveltype = portal[portalindex].ltype;
		} else {
			level.setlevel = FALSE;
			level.currlevel = portal[portalindex].level;
			plr[myplr].plrlevel = level.currlevel;
			level.leveltype = portal[portalindex].ltype;
		}
		if (portalindex == myplr) {
			NetSendCmd(TRUE, CMD_DEACTIVATEPORTAL);
			DeactivatePortal(portalindex);
		}
	}
}

void GetPortalLvlPos()
{
	if (level.currlevel == 0) {
		ViewX = WarpDropX[portalindex] + 1;
		ViewY = WarpDropY[portalindex] + 1;
	} else {
		ViewX = portal[portalindex].x;
		ViewY = portal[portalindex].y;

		if (portalindex != myplr) {
			ViewX++;
			ViewY++;
		}
	}
}

BOOL PosOkPortal(int lvl, int x, int y)
{
	int i;

	for (i = 0; i < MAXPORTAL; i++) {
		if (portal[i].open && portal[i].level == lvl && ((portal[i].x == x && portal[i].y == y) || (portal[i].x == x - 1 && portal[i].y == y - 1)))
			return TRUE;
	}
	return FALSE;
}

DEVILUTION_END_NAMESPACE
