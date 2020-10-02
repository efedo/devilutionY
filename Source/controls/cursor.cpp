/**
 * @file cursor.cpp
 *
 * Implementation of cursor tracking functionality.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int cursW;
int cursH;
int pcursmonst = -1;
int icursW28;
int icursH28;
uint8_t *pCursCels; // Cursor cel image

/** inv_item value */
char pcursinvitem;
int icursW;
int icursH;
Item * pcursitem = 0;
char pcursobj;
char pcursplr;
V2Di cursm;
//int cursmx;
//int cursmy;
int pcurstemp;
int pcurs;

/* rdata */
/** Maps from objcurs.cel frame number to frame width. */
const int InvItemWidth[] = {
	// clang-format off
	// Cursors
	0, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 23,
	// Items
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	// clang-format on
};

/** Maps from objcurs.cel frame number to frame height. */
const int InvItemHeight[] = {
	// clang-format off
	// Cursors
	0, 29, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35,
	// Items
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	// clang-format on
};

void InitCursor()
{
	assert(!pCursCels);
	pCursCels = LoadFileInMem("Data\\Inv\\Objcurs.CEL", NULL);
	ClearCursor();
}

void FreeCursor()
{
	MemFreeDbg(pCursCels);
	ClearCursor();
}

void SetICursor(int i)
{
	icursW = InvItemWidth[i];
	icursH = InvItemHeight[i];
	icursW28 = icursW / 28;
	icursH28 = icursH / 28;
}

void SetCursor_(int i)
{
	pcurs = i;
	cursW = InvItemWidth[i];
	cursH = InvItemHeight[i];
	SetICursor(i);
}

void NewCursor(int i)
{
	SetCursor_(i);
}

void InitLevelCursor()
{
	SetCursor_(CURSOR_HAND);
	cursm = View;
	pcurstemp = -1;
	pcursmonst = -1;
	pcursobj = -1;
	pcursitem = 0;
	pcursplr = -1;
	ClearCursor();
}

void CheckTown()
{
	int i, mx;

	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MissileType::TOWN) {
			if (cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y
			    || cursm.x == missile[mx]._mi.x && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 2 && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 2 && cursm.y == missile[mx]._mi.y - 2
			    || cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y - 2
			    || cursm.x == missile[mx]._mi.x && cursm.y == missile[mx]._mi.y) {
				trigflag = true;
				ClearPanel();
				strcpy(infostr, "Town Portal");
				sprintf(tempstr, "from %s", plr[missile[mx]._misource].data._pName);
				AddPanelString(tempstr, true);
				cursm = missile[mx]._mi;
			}
		}
	}
}

void CheckRportal()
{
	int i, mx;

	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MissileType::RPORTAL) {
			if (cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y
			    || cursm.x == missile[mx]._mi.x && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 2 && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 2 && cursm.y == missile[mx]._mi.y - 2
			    || cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y - 2
			    || cursm.x == missile[mx]._mi.x && cursm.y == missile[mx]._mi.y) {
				trigflag = true;
				ClearPanel();
				strcpy(infostr, "Portal to");
				if (!lvl.setlevel)
					strcpy(tempstr, "The Unholy Altar");
				else
					strcpy(tempstr, "level 15");
				AddPanelString(tempstr, true);
				cursm = missile[mx]._mi;
			}
		}
	}
}

//void _CheckCursMoveMonster(V2Di pos, bool flipflag, int flipflaglogic = 0)
//{
//	Tile &tile = grid.at(pos);
//	if (flipflaglogic == 0) {
//		flipflag = true;
//	} else if (flipflaglogic < 0) {
//		flipflag = !flipflag;
//	}
//	if (flipflag && grid.isValid(pos) && tile.isActor() && tile.dFlags & BFLAG_LIT) {
//		int mi = tile.getActor();
//		if (mi == pcurstemp && monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 4) {
//			cursm = pos;
//			pcursmonst = mi;
//		}
//	}
//}



void CheckCursMove()
{

	int i, mi, columns, rows;
	char bv;

	V2Di mouse = Mouse;

	if (PANELS_COVER) {
		if (chrflag || questlog) {
			if (mouse.x >= SCREEN_WIDTH / 2) { /// BUGFIX: (sx >= SCREEN_WIDTH / 2) (fixed)
				mouse.x -= SCREEN_WIDTH / 4;
			} else {
				mouse.x = 0;
			}
		} else if (gui.invflag || gui.sbookflag) {
			if (mouse.x <= SCREEN_WIDTH / 2) {
				mouse.x += SCREEN_WIDTH / 4;
			} else {
				mouse.x = 0;
			}
		}
	}
	if (mouse.y > PANEL_TOP - 1 && Mouse.x >= PANEL_LEFT && Mouse.x < PANEL_LEFT + PANEL_WIDTH && track_isscrolling()) {
		mouse.y = PANEL_TOP - 1;
	}

	if (!zoomflag) {
		mouse.x >>= 1;
		mouse.y >>= 1;
	}


	// The code below moves the mouse

	// Adjust by player offset and tile grid alignment
	V2Di offset;
	CalcTileOffset(&offset.x, &offset.y);
	mouse -= ScrollInfo._soff - offset;

	// Predict the next frame when walking to avoid input jitter
	V2Di f;
	f.x = myplr().data._pVar6 / 256;
	f.y = myplr().data._pVar7 / 256;
	f.x -= (myplr().data._pVar6 + myplr().data._pvel.x) / 256;
	f.y -= (myplr().data._pVar7 + myplr().data._pvel.y) / 256;
	if (ScrollInfo._sdir != ScrollDir::NONE) {
		mouse -= f;
	}

	// Convert to tile grid
	V2Di t = { mouse.x / TILE_WIDTH, mouse.y / TILE_HEIGHT };
	V2Di m = View;
	ShiftGrid(&m.x, &m.y, t.x, t.y);

	// Center player tile on screen
	TilesInView(&columns, &rows);
	ShiftGrid(&m.x, &m.y, -columns / 2, -(rows - RowsCoveredByPanel()) / 4);
	if ((columns % 2) != 0) {
		m.y++;
	}

	// Shift position to match diamond grid aligment
	V2Di p = { mouse.x % TILE_WIDTH, mouse.y % TILE_HEIGHT };

	// Shift position to match diamond grid aligment
	V2Di flip;
	flip.y = p.y < (p.x >> 1);
	if (flip.y) m.y--;
	flip.x = p.y >= TILE_HEIGHT - (p.x >> 1);
	if (flip.x) m.x++;

	if (m.x < 0) m.x = 0;
	if (m.x >= MAXDUNX) m.x = MAXDUNX - 1;
	if (m.y < 0) m.y = 0;
	if (m.y >= MAXDUNY) m.y = MAXDUNY - 1;

	bool flipflag = flip.y && flip.x || (flip.y || flip.x) && p.x < TILE_WIDTH / 2;

	pcurstemp = pcursmonst;
	pcursmonst = -1;
	pcursobj = -1;
	pcursitem = 0;
	if (pcursinvitem != -1) {
		drawsbarflag = true;
	}
	pcursinvitem = -1;
	pcursplr = -1;
	uitemflag = false;
	panelflag = false;
	trigflag = false;

	if (myplr().data._pInvincible) return;

	if (pcurs >= CURSOR_FIRSTITEM || spselflag) {
		cursm = m;
		return;
	}
	if (Mouse.y > PANEL_TOP && Mouse.x >= PANEL_LEFT && Mouse.x <= PANEL_LEFT + PANEL_WIDTH) {
		CheckPanelInfo();
		return;
	}
	if (doomflag) {
		return;
	}
	if (gui.invflag && Mouse.x > RIGHT_PANEL && Mouse.y <= SPANEL_HEIGHT) {
		pcursinvitem = myplr().inv.CheckInvHLight();
		return;
	}
	if (gui.sbookflag && Mouse.x > RIGHT_PANEL && Mouse.y <= SPANEL_HEIGHT) {
		return;
	}
	if ((chrflag || questlog) && Mouse.x < SPANEL_WIDTH && Mouse.y <= SPANEL_HEIGHT) {
		return;
	}

	auto _monst1 = [=](V2Di pos, bool flipflag, int flipflaglogic = 0) {
		Tile &tile = grid.at(pos);
		if (flipflaglogic == 0) {
			flipflag = true;
		} else if (flipflaglogic < 0) {
			flipflag = !flipflag;
		}
		if (flipflag && grid.isValid(pos) && tile.isActor() && tile.dFlags & BFLAG_LIT) {
			int mi = tile.getActor();
			if (mi == pcurstemp && monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 4) {
				cursm = pos;
				pcursmonst = mi;
			}
		}
	};

	if (lvl.type() != DunType::town) { // Not in town, aggressive monster selection
		if (pcurstemp != -1) { // was monster last time
			_monst1(m + V2Di(2, 1), flipflag, -1); // if flipped, false
			_monst1(m + V2Di(1, 2), flipflag, +1); // if not flipped, false
			_monst1(m + V2Di(2, 2), flipflag, 0);
			_monst1(m + V2Di(1, 0), flipflag, -1); // if flipped, false
			_monst1(m + V2Di(0, 1), flipflag, +1);
			_monst1(m + V2Di(0, 0), flipflag, 0);
			_monst1(m + V2Di(1, 1), flipflag, 0);

			if (pcursmonst != -1 && monsters[pcursmonst].data._mFlags & MonsterFlag::hidden) {
				pcursmonst = -1;
				cursm = m;
			}
			if (pcursmonst != -1 && monsters[pcursmonst].data._mFlags & MonsterFlag::golem) {
				pcursmonst = -1;
			}
			if (pcursmonst != -1) {
				return; // Do not override previous monster selection if it was a monster last time
			}
		}

		_monst1(m + V2Di(2, 1), flipflag, -1);
		_monst1(m + V2Di(1, 2), flipflag, +1);
		_monst1(m + V2Di(2, 2), flipflag, 0);
		_monst1(m + V2Di(1, 0), flipflag, -1);
		_monst1(m + V2Di(0, 1), flipflag, +1);
		_monst1(m + V2Di(0, 0), flipflag, 0);
		_monst1(m + V2Di(1, 1), flipflag, 0);

		if (pcursmonst != -1 && monsters[pcursmonst].data._mFlags & MonsterFlag::hidden) {
			pcursmonst = -1;
			cursm = m;
		}
		if (pcursmonst != -1 && monsters[pcursmonst].data._mFlags & MonsterFlag::golem) {
			pcursmonst = -1;
		}
	} else { // In town... less aggresive monster selection?

		auto _monst2 = [=](V2Di pos, bool flipflag, int flipflaglogic = 0) {
			if (flipflaglogic == 0) {
				flipflag = true;
			} else if (flipflaglogic < 0) {
				flipflag = !flipflag;
			}
			if (flipflag && grid.isValid(pos) && grid.at(pos).isActor()) {
				pcursmonst = grid.at(pos).getActor();
				cursm = { m.x + 1, m.y };
			}
		};

		_monst2(m + V2Di(1, 0), flipflag, -1);
		_monst2(m + V2Di(0, 1), flipflag, +1);
		_monst2(m + V2Di(0, 0), flipflag,  0);
		_monst2(m + V2Di(1, 1), flipflag,  0);

		if (pcursmonst != -1 && !towner[pcursmonst]._tSelFlag) {
			pcursmonst = -1;
		}
	}

	if (pcursmonst == -1) { // Not a monster, determine if a player
		if (!flipflag && m.x + 1 < MAXDUNX && grid[m.x + 1][m.y].isPlayer()) {
			bv = grid[m.x + 1][m.y].getPlayer();
			if (bv != myplr() && plr[bv].data._pHitPoints != 0) {
				cursm = { m.x + 1, m.y };
				pcursplr = bv;
			}
		}
		if (flipflag && m.y + 1 < MAXDUNY && grid[m.x][m.y + 1].isPlayer()) {
			bv = grid[m.x][m.y + 1].getPlayer();
			if (bv != myplr() && plr[bv].data._pHitPoints != 0) {
				cursm = { m.x, m.y + 1 };
				pcursplr = bv;
			}
		}
		if (grid.at(m).isPlayer()) {
			bv = grid.at(m).getPlayer(); 
			if (bv != myplr()) {
				cursm = m;
				pcursplr = bv;
			}
		}
		if (grid.at(m).dFlags & BFLAG_DEAD_PLAYER) {
			for (i = 0; i < MAX_PLRS; i++) {
				if (plr[i].pos() == m && i != myplr()) {
					cursm = m;
					pcursplr = i;
				}
			}
		}
		if (pcurs == CURSOR_RESURRECT) {
			V2Di n;
			for (n.x = -1; n.x < 2; n.x++) {
				for (n.y = -1; n.y < 2; n.y++) {
					if (m.x + n.x < MAXDUNX && m.y + n.y < MAXDUNY && grid.at(m + n).dFlags & BFLAG_DEAD_PLAYER) {
						for (i = 0; i < MAX_PLRS; i++) {
							if (plr[i].pos() == m + n && i != myplr()) {
								cursm = m + n;
								pcursplr = i;
							}
						}
					}
				}
			}
		}
		if (m.x + 1 < MAXDUNX && m.y + 1 < MAXDUNY && grid[m.x + 1][m.y + 1].isPlayer()) {
			bv = grid[m.x + 1][m.y + 1].getPlayer();
			if (bv != myplr() && plr[bv].data._pHitPoints != 0) {
				cursm = { m.x + 1, m.y + 1 };
				pcursplr = bv;
			}
		}
	}

	if (pcursmonst == -1 && pcursplr == -1) { // Not a monster or a player... determine if an object

		auto _obj1 = [=](V2Di pos, bool flipflag, int flipflaglogic = 0) {
			if (flipflaglogic == 0) {
				flipflag = true;
			} else if (flipflaglogic < 0) {
				flipflag = !flipflag;
			}
			if (flipflag && grid.isValid(pos) && grid.at(pos).isObject()) {
				pcursobj = grid.at(pos).getObject();
				cursm = pos;
			}
		};

		_obj1(m + V2Di(1, 0), flipflag, -1);
		_obj1(m + V2Di(0, 1), flipflag, +1);
		_obj1(m + V2Di(0, 0), flipflag,  0);
		_obj1(m + V2Di(1, 1), flipflag,  0);
	}

	if (pcursplr == -1 && pcursobj == -1 && pcursmonst == -1) { // Not a monster or an object or a player... determine if item

		auto _itm1 = [=](V2Di pos, bool flipflag, int flipflaglogic = 0) {
			if (flipflaglogic == 0) {
				flipflag = true;
			} else if (flipflaglogic < 0) {
				flipflag = !flipflag;
			}
			if (!flipflag && grid.isValid(pos) && grid.at(pos).isItem()) {
				pcursitem = grid.at(pos).getItem();
				cursm = pos;
			}
		};

		_itm1(m + V2Di(1, 0), flipflag, -1);
		_itm1(m + V2Di(0, 1), flipflag, +1);
		_itm1(m + V2Di(0, 0), flipflag,  0);
		_itm1(m + V2Di(1, 1), flipflag, 0);

		if (pcursitem == -1) { // Process a few special items
			cursm = m;
			CheckTrigForce();
			CheckTown();
			CheckRportal();
		}
	}

	if (pcurs == CURSOR_IDENTIFY) {
		pcursobj = -1;
		pcursmonst = -1;
		pcursitem = 0;
		cursm = m;
	}
	if (pcursmonst != -1 && monsters[pcursmonst].data._mFlags & MonsterFlag::golem) {
		pcursmonst = -1;
	}
}

DEVILUTION_END_NAMESPACE
