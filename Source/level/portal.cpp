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

void SetPortalStats(int i, bool o, V2Di p, int lvl, DunType lvltype)
{
	portal[i].open = o;
	portal[i].pos.x = p.x;
	portal[i].pos.y = p.y;
	portal[i].level = lvl;
	portal[i].ltype = lvltype;
	portal[i].setlvl = FALSE;
}

void AddWarpMissile(int i, V2Di p)
{
	int mi;

	missiledata[MIS_TOWN].mlSFX = -1;
	grid.at(p).clearMissile();
	mi = AddMissile({ 0, 0 }, p, Dir(0), MIS_TOWN, 0, i, 0, 0);

	if (mi != -1) {
		SetMissDir(mi, Dir(1));

		if (lvl.currlevel != 0)
			missile[mi]._mlid = AddLight(missile[mi]._mi, 15);

		missiledata[MIS_TOWN].mlSFX = LS_SENTINEL;
	}
}

void SyncPortals()
{
	int i;

	for (i = 0; i < MAXPORTAL; i++) {
		if (!portal[i].open)
			continue;
		if (lvl.currlevel == 0)
			AddWarpMissile(i, { WarpDropX[i], WarpDropY[i] });
		else {
			int curlvl = lvl.currlevel;
			if (lvl.setlevel)
				curlvl = int(lvl.setlvlnum);
			if (portal[i].level == curlvl)
				AddWarpMissile(i, portal[i].pos);
		}
	}
}

void AddInTownPortal(int i)
{
	AddWarpMissile(i, { WarpDropX[i], WarpDropY[i] });
}

void ActivatePortal(int i, V2Di p, int lvl, DunType lvltype, bool sp)
{
	portal[i].open = TRUE;

	if (lvl != 0) {
		portal[i].pos = p;
		portal[i].level = lvl;
		portal[i].ltype = lvltype;
		portal[i].setlvl = sp;
	}
}

void DeactivatePortal(int i)
{
	portal[i].open = FALSE;
}

bool PortalOnLevel(int i)
{
	if (portal[i].level == lvl.currlevel)
		return TRUE;
	else
		return lvl.currlevel == 0;
}

void RemovePortalMissile(int id)
{
	int i;
	int mi;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		if (missile[mi]._mitype == MIS_TOWN && missile[mi]._misource == id) {
			grid.at(missile[mi]._mi).dFlags &= ~BFLAG_MISSILE;
			grid.at(missile[mi]._mi).clearMissile();
			if (portal[id].level != 0) AddUnLight(missile[mi]._mlid);
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
	if (lvl.currlevel) {
		lvl.setlevel = FALSE;
		lvl.currlevel = 0;
		myplr().data.plrlevel = 0;
		lvl.setType(DunType::town);
	} else {
		if (portal[portalindex].setlvl) {
			lvl.setlevel = TRUE;
			lvl.setlvlnum = SetLvl(portal[portalindex].level);
			lvl.currlevel = portal[portalindex].level;
			myplr().data.plrlevel = int(lvl.setlvlnum);
			lvl.setType(portal[portalindex].ltype);
		} else {
			lvl.setlevel = FALSE;
			lvl.currlevel = portal[portalindex].level;
			myplr().data.plrlevel = lvl.currlevel;
			lvl.setType(portal[portalindex].ltype);
		}
		if (portalindex == myplr()) {
			NetSendCmd(TRUE, CMD_DEACTIVATEPORTAL);
			DeactivatePortal(portalindex);
		}
	}
}

void GetPortalLvlPos()
{
	if (lvl.currlevel == 0) {
		View.x = WarpDropX[portalindex] + 1;
		View.y = WarpDropY[portalindex] + 1;
	} else {
		View = portal[portalindex].pos;

		if (portalindex != myplr()) {
			View.x++;
			View.y++;
		}
	}
}

bool PosOkPortal(int lvl, V2Di p)
{
	int i;

	for (i = 0; i < MAXPORTAL; i++) {
		if (portal[i].open && portal[i].level == lvl && ((portal[i].pos.x == p.x && portal[i].pos.y == p.y) ||
			(portal[i].pos.x == p.x - 1 && portal[i].pos.y == p.y - 1)))
			return TRUE;
	}
	return FALSE;
}

DEVILUTION_END_NAMESPACE
