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
BYTE *pCursCels; // Cursor cel image

/** inv_item value */
char pcursinvitem;
int icursW;
int icursH;
char pcursitem;
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
	pcursitem = -1;
	pcursplr = -1;
	ClearCursor();
}

void CheckTown()
{
	int i, mx;

	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_TOWN) {
			if (cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y
			    || cursm.x == missile[mx]._mi.x && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 2 && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 2 && cursm.y == missile[mx]._mi.y - 2
			    || cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y - 2
			    || cursm.x == missile[mx]._mi.x && cursm.y == missile[mx]._mi.y) {
				trigflag = TRUE;
				ClearPanel();
				strcpy(infostr, "Town Portal");
				sprintf(tempstr, "from %s", plr[missile[mx]._misource].data._pName);
				AddPanelString(tempstr, TRUE);
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
		if (missile[mx]._mitype == MIS_RPORTAL) {
			if (cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y
			    || cursm.x == missile[mx]._mi.x && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 2 && cursm.y == missile[mx]._mi.y - 1
			    || cursm.x == missile[mx]._mi.x - 2 && cursm.y == missile[mx]._mi.y - 2
			    || cursm.x == missile[mx]._mi.x - 1 && cursm.y == missile[mx]._mi.y - 2
			    || cursm.x == missile[mx]._mi.x && cursm.y == missile[mx]._mi.y) {
				trigflag = TRUE;
				ClearPanel();
				strcpy(infostr, "Portal to");
				if (!level.setlevel)
					strcpy(tempstr, "The Unholy Altar");
				else
					strcpy(tempstr, "level 15");
				AddPanelString(tempstr, TRUE);
				cursm = missile[mx]._mi;
			}
		}
	}
}

void CheckCursMove()
{
	V2Di s, f, m, t, p, n, o, flip; // sx, sy, fx, fy, mx, my, tx, ty, px, py, xx, yy, xo, yo, flipx, flipy
	int i, mi, columns, rows;
	char bv;
	BOOL flipflag;

	s = Mouse;

	if (PANELS_COVER) {
		if (chrflag || questlog) {
			if (s.x >= SCREEN_WIDTH / 2) { /// BUGFIX: (sx >= SCREEN_WIDTH / 2) (fixed)
				s.x -= SCREEN_WIDTH / 4;
			} else {
				s.x = 0;
			}
		} else if (invflag || sbookflag) {
			if (s.x <= SCREEN_WIDTH / 2) {
				s.x += SCREEN_WIDTH / 4;
			} else {
				s.x = 0;
			}
		}
	}
	if (s.y > PANEL_TOP - 1 && Mouse.x >= PANEL_LEFT && Mouse.x < PANEL_LEFT + PANEL_WIDTH && track_isscrolling()) {
		s.y = PANEL_TOP - 1;
	}

	if (!zoomflag) {
		s.x >>= 1;
		s.y >>= 1;
	}

	// Adjust by player offset and tile grid alignment
	CalcTileOffset(&o.x, &o.y);
	s -= ScrollInfo._soff - o;

	// Predict the next frame when walking to avoid input jitter
	f.x = myplr().data._pVar6 / 256;
	f.y = myplr().data._pVar7 / 256;
	f.x -= (myplr().data._pVar6 + myplr().data._pvel.x) / 256;
	f.y -= (myplr().data._pVar7 + myplr().data._pvel.y) / 256;
	if (ScrollInfo._sdir != SDIR_NONE) {
		s -= f;
	}

	// Convert to tile grid
	m = View;
	t.x = s.x / TILE_WIDTH;
	t.y = s.y / TILE_HEIGHT;
	ShiftGrid(&m.x, &m.y, t.x, t.y);

	// Center player tile on screen
	TilesInView(&columns, &rows);
	ShiftGrid(&m.x, &m.y, -columns / 2, -(rows - RowsCoveredByPanel()) / 4);
	if ((columns % 2) != 0) {
		m.y++;
	}

	// Shift position to match diamond grid aligment
	p.x = s.x % TILE_WIDTH;
	p.y = s.y % TILE_HEIGHT;

	// Shift position to match diamond grid aligment
	flip.y = p.y < (p.x >> 1);
	if (flip.y) m.y--;
	flip.x = p.y >= TILE_HEIGHT - (p.x >> 1);
	if (flip.x) m.x++;

	if (m.x < 0) m.x = 0;
	if (m.x >= MAXDUNX) m.x = MAXDUNX - 1;
	if (m.y < 0) m.y = 0;
	if (m.y >= MAXDUNY) m.y = MAXDUNY - 1;

	flipflag = flip.y && flip.x || (flip.y || flip.x) && p.x < TILE_WIDTH / 2;

	pcurstemp = pcursmonst;
	pcursmonst = -1;
	pcursobj = -1;
	pcursitem = -1;
	if (pcursinvitem != -1) {
		drawsbarflag = TRUE;
	}
	pcursinvitem = -1;
	pcursplr = -1;
	uitemflag = FALSE;
	panelflag = FALSE;
	trigflag = FALSE;

	if (myplr().data._pInvincible) {
		return;
	}
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
	if (invflag && Mouse.x > RIGHT_PANEL && Mouse.y <= SPANEL_HEIGHT) {
		pcursinvitem = CheckInvHLight();
		return;
	}
	if (sbookflag && Mouse.x > RIGHT_PANEL && Mouse.y <= SPANEL_HEIGHT) {
		return;
	}
	if ((chrflag || questlog) && Mouse.x < SPANEL_WIDTH && Mouse.y <= SPANEL_HEIGHT) {
		return;
	}

	if (level.leveltype != DTYPE_TOWN) {
		if (pcurstemp != -1) {
			if (!flipflag && m.x + 2 < MAXDUNX && m.y + 1 < MAXDUNY && grid[m.x + 2][m.y + 1].dMonster != 0 && grid[m.x + 2][m.y + 1].dFlags & BFLAG_LIT) {
				mi = grid[m.x + 2][m.y + 1].dMonster > 0 ? grid[m.x + 2][m.y + 1].dMonster - 1 : -(grid[m.x + 2][m.y + 1].dMonster + 1);
				if (mi == pcurstemp && monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 4) {
					cursm = { m.x + 2, m.y + 1 };
					pcursmonst = mi;
				}
			}
			if (flipflag && m.x + 1 < MAXDUNX && m.y + 2 < MAXDUNY && grid[m.x + 1][m.y + 2].dMonster != 0 && grid[m.x + 1][m.y + 2].dFlags & BFLAG_LIT) {
				mi = grid[m.x + 1][m.y + 2].dMonster > 0 ? grid[m.x + 1][m.y + 2].dMonster - 1 : -(grid[m.x + 1][m.y + 2].dMonster + 1);
				if (mi == pcurstemp && monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 4) {
					cursm = { m.x + 1, m.y + 2 };
					pcursmonst = mi;
				}
			}
			if (m.x + 2 < MAXDUNX && m.y + 2 < MAXDUNY && grid[m.x + 2][m.y + 2].dMonster != 0 && grid[m.x + 2][m.y + 2].dFlags & BFLAG_LIT) {
				mi = grid[m.x + 2][m.y + 2].dMonster > 0 ? grid[m.x + 2][m.y + 2].dMonster - 1 : -(grid[m.x + 2][m.y + 2].dMonster + 1);
				if (mi == pcurstemp && monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 4) {
					cursm = { m.x + 2, m.y + 2 };
					pcursmonst = mi;
				}
			}
			if (m.x + 1 < MAXDUNX && !flipflag && grid[m.x + 1][m.y].dMonster != 0 && grid[m.x + 1][m.y].dFlags & BFLAG_LIT) {
				mi = grid[m.x + 1][m.y].dMonster > 0 ? grid[m.x + 1][m.y].dMonster - 1 : -(grid[m.x + 1][m.y].dMonster + 1);
				if (mi == pcurstemp && monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 2) {
					cursm = { m.x + 1, m.y };
					pcursmonst = mi;
				}
			}
			if (m.y + 1 < MAXDUNY && flipflag && grid[m.x][m.y + 1].dMonster != 0 && grid[m.x][m.y + 1].dFlags & BFLAG_LIT) {
				mi = grid[m.x][m.y + 1].dMonster > 0 ? grid[m.x][m.y + 1].dMonster - 1 : -(grid[m.x][m.y + 1].dMonster + 1);
				if (mi == pcurstemp && monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 2) {
					cursm = { m.x, m.y + 1 };
					pcursmonst = mi;
				}
			}
			if (grid[m.x][m.y].dMonster != 0 && grid[m.x][m.y].dFlags & BFLAG_LIT) {
				mi = grid[m.x][m.y].dMonster > 0 ? grid[m.x][m.y].dMonster - 1 : -(grid[m.x][m.y].dMonster + 1);
				if (mi == pcurstemp && monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 1) {
					cursm = m;
					pcursmonst = mi;
				}
			}
			if (m.x + 1 < MAXDUNX && m.y + 1 < MAXDUNY && grid[m.x + 1][m.y + 1].dMonster != 0 && grid[m.x + 1][m.y + 1].dFlags & BFLAG_LIT) {
				mi = grid[m.x + 1][m.y + 1].dMonster > 0 ? grid[m.x + 1][m.y + 1].dMonster - 1 : -(grid[m.x + 1][m.y + 1].dMonster + 1);
				if (mi == pcurstemp && monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 2) {
					cursm = { m.x + 1, m.y + 1 };
					pcursmonst = mi;
				}
			}
			if (pcursmonst != -1 && monsters[pcursmonst].data._mFlags & MFLAG_HIDDEN) {
				pcursmonst = -1;
				cursm = m;
			}
			if (pcursmonst != -1 && monsters[pcursmonst].data._mFlags & MFLAG_GOLEM) {
				pcursmonst = -1;
			}
			if (pcursmonst != -1) {
				return;
			}
		}
		if (!flipflag && m.x + 2 < MAXDUNX && m.y + 1 < MAXDUNY && grid[m.x + 2][m.y + 1].dMonster != 0 && grid[m.x + 2][m.y + 1].dFlags & BFLAG_LIT) {
			mi = grid[m.x + 2][m.y + 1].dMonster > 0 ? grid[m.x + 2][m.y + 1].dMonster - 1 : -(grid[m.x + 2][m.y + 1].dMonster + 1);
			if (monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 4) {
				cursm = { m.x + 2, m.y + 1 };
				pcursmonst = mi;
			}
		}
		if (flipflag && m.x + 1 < MAXDUNX && m.y + 2 < MAXDUNY && grid[m.x + 1][m.y + 2].dMonster != 0 && grid[m.x + 1][m.y + 2].dFlags & BFLAG_LIT) {
			mi = grid[m.x + 1][m.y + 2].dMonster > 0 ? grid[m.x + 1][m.y + 2].dMonster - 1 : -(grid[m.x + 1][m.y + 2].dMonster + 1);
			if (monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 4) {
				cursm = { m.x + 1, m.y + 2 };
				pcursmonst = mi;
			}
		}
		if (m.x + 2 < MAXDUNX && m.y + 2 < MAXDUNY && grid[m.x + 2][m.y + 2].dMonster != 0 && grid[m.x + 2][m.y + 2].dFlags & BFLAG_LIT) {
			mi = grid[m.x + 2][m.y + 2].dMonster > 0 ? grid[m.x + 2][m.y + 2].dMonster - 1 : -(grid[m.x + 2][m.y + 2].dMonster + 1);
			if (monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 4) {
				cursm = { m.x + 2, m.y + 2 };
				pcursmonst = mi;
			}
		}
		if (!flipflag && m.x + 1 < MAXDUNX && grid[m.x + 1][m.y].dMonster != 0 && grid[m.x + 1][m.y].dFlags & BFLAG_LIT) {
			mi = grid[m.x + 1][m.y].dMonster > 0 ? grid[m.x + 1][m.y].dMonster - 1 : -(grid[m.x + 1][m.y].dMonster + 1);
			if (monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 2) {
				cursm = { m.x + 1, m.y };
				pcursmonst = mi;
			}
		}
		if (flipflag && m.y + 1 < MAXDUNY && grid[m.x][m.y + 1].dMonster != 0 && grid[m.x][m.y + 1].dFlags & BFLAG_LIT) {
			mi = grid[m.x][m.y + 1].dMonster > 0 ? grid[m.x][m.y + 1].dMonster - 1 : -(grid[m.x][m.y + 1].dMonster + 1);
			if (monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 2) {
				cursm = { m.x, m.y + 1 };
				pcursmonst = mi;
			}
		}
		if (grid[m.x][m.y].dMonster != 0 && grid[m.x][m.y].dFlags & BFLAG_LIT) {
			mi = grid[m.x][m.y].dMonster > 0 ? grid[m.x][m.y].dMonster - 1 : -(grid[m.x][m.y].dMonster + 1);
			if (monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 1) {
				cursm = { m.x, m.y };
				pcursmonst = mi;
			}
		}
		if (m.x + 1 < MAXDUNX && m.y + 1 < MAXDUNY && grid[m.x + 1][m.y + 1].dMonster != 0 && grid[m.x + 1][m.y + 1].dFlags & BFLAG_LIT) {
			mi = grid[m.x + 1][m.y + 1].dMonster > 0 ? grid[m.x + 1][m.y + 1].dMonster - 1 : -(grid[m.x + 1][m.y + 1].dMonster + 1);
			if (monsters[mi].data._mhitpoints >> 6 > 0 && monsters[mi].data.MData->mSelFlag & 2) {
				cursm = { m.x + 1, m.y + 1 };
				pcursmonst = mi;
			}
		}
		if (pcursmonst != -1 && monsters[pcursmonst].data._mFlags & MFLAG_HIDDEN) {
			pcursmonst = -1;
			cursm = m;
		}
		if (pcursmonst != -1 && monsters[pcursmonst].data._mFlags & MFLAG_GOLEM) {
			pcursmonst = -1;
		}
	} else {
		if (!flipflag && m.x + 1 < MAXDUNX && grid[m.x + 1][m.y].dMonster > 0) {
			pcursmonst = grid[m.x + 1][m.y].dMonster - 1;
			cursm = { m.x + 1, m.y };
		}
		if (flipflag && m.y + 1 < MAXDUNY && grid[m.x][m.y + 1].dMonster > 0) {
			pcursmonst = grid[m.x][m.y + 1].dMonster - 1;
			cursm = { m.x, m.y + 1 };
		}
		if (grid[m.x][m.y].dMonster > 0) {
			pcursmonst = grid[m.x][m.y].dMonster - 1;
			cursm = m;
		}
		if (m.x + 1 < MAXDUNX && m.y + 1 < MAXDUNY && grid[m.x + 1][m.y + 1].dMonster > 0) {
			pcursmonst = grid[m.x + 1][m.y + 1].dMonster - 1;
			cursm = { m.x + 1, m.y + 1 };
		}
		if (pcursmonst != -1 && !towner[pcursmonst]._tSelFlag) {
			pcursmonst = -1;
		}
	}

	if (pcursmonst == -1) {
		if (!flipflag && m.x + 1 < MAXDUNX && grid[m.x + 1][m.y].dPlayer != 0) {
			bv = grid[m.x + 1][m.y].dPlayer > 0 ? grid[m.x + 1][m.y].dPlayer - 1 : -(grid[m.x + 1][m.y].dPlayer + 1);
			if (bv != myplr() && plr[bv].data._pHitPoints != 0) {
				cursm = { m.x + 1, m.y };
				pcursplr = bv;
			}
		}
		if (flipflag && m.y + 1 < MAXDUNY && grid[m.x][m.y + 1].dPlayer != 0) {
			bv = grid[m.x][m.y + 1].dPlayer > 0 ? grid[m.x][m.y + 1].dPlayer - 1 : -(grid[m.x][m.y + 1].dPlayer + 1);
			if (bv != myplr() && plr[bv].data._pHitPoints != 0) {
				cursm = { m.x, m.y + 1 };
				pcursplr = bv;
			}
		}
		if (grid[m.x][m.y].dPlayer != 0) {
			bv = grid[m.x][m.y].dPlayer > 0 ? grid[m.x][m.y].dPlayer - 1 : -(grid[m.x][m.y].dPlayer + 1);
			if (bv != myplr()) {
				cursm = m;
				pcursplr = bv;
			}
		}
		if (grid[m.x][m.y].dFlags & BFLAG_DEAD_PLAYER) {
			for (i = 0; i < MAX_PLRS; i++) {
				if (plr[i].data._p.x == m.x && plr[i].data._p.y == m.y && i != myplr()) {
					cursm = m;
					pcursplr = i;
				}
			}
		}
		if (pcurs == CURSOR_RESURRECT) {
			for (n.x = -1; n.x < 2; n.x++) {
				for (n.y = -1; n.y < 2; n.y++) {
					if (m.x + n.x < MAXDUNX && m.y + n.y < MAXDUNY && grid[m.x + n.x][m.y + n.y].dFlags & BFLAG_DEAD_PLAYER) {
						for (i = 0; i < MAX_PLRS; i++) {
							if (plr[i].data._p.x == m.x + n.x && plr[i].data._p.y == m.y + n.y && i != myplr()) {
								cursm = { m.x + n.x, m.y + n.y };
								pcursplr = i;
							}
						}
					}
				}
			}
		}
		if (m.x + 1 < MAXDUNX && m.y + 1 < MAXDUNY && grid[m.x + 1][m.y + 1].dPlayer != 0) {
			bv = grid[m.x + 1][m.y + 1].dPlayer > 0 ? grid[m.x + 1][m.y + 1].dPlayer - 1 : -(grid[m.x + 1][m.y + 1].dPlayer + 1);
			if (bv != myplr() && plr[bv].data._pHitPoints != 0) {
				cursm = { m.x + 1, m.y + 1 };
				pcursplr = bv;
			}
		}
	}
	if (pcursmonst == -1 && pcursplr == -1) {
		if (!flipflag && m.x + 1 < MAXDUNX && grid[m.x + 1][m.y].dObject != 0) {
			bv = grid[m.x + 1][m.y].dObject > 0 ? grid[m.x + 1][m.y].dObject - 1 : -(grid[m.x + 1][m.y].dObject + 1);
			if (object[bv]._oSelFlag >= 2) {
				cursm = { m.x + 1, m.y };
				pcursobj = bv;
			}
		}
		if (flipflag && m.y + 1 < MAXDUNY && grid[m.x][m.y + 1].dObject != 0) {
			bv = grid[m.x][m.y + 1].dObject > 0 ? grid[m.x][m.y + 1].dObject - 1 : -(grid[m.x][m.y + 1].dObject + 1);
			if (object[bv]._oSelFlag >= 2) {
				cursm = { m.x, m.y + 1 };
				pcursobj = bv;
			}
		}
		if (grid[m.x][m.y].dObject != 0) {
			bv = grid[m.x][m.y].dObject > 0 ? grid[m.x][m.y].dObject - 1 : -(grid[m.x][m.y].dObject + 1);
			if (object[bv]._oSelFlag == 1 || object[bv]._oSelFlag == 3) {
				cursm = m;
				pcursobj = bv;
			}
		}
		if (m.x + 1 < MAXDUNX && m.y + 1 < MAXDUNY && grid[m.x + 1][m.y + 1].dObject != 0) {
			bv = grid[m.x + 1][m.y + 1].dObject > 0 ? grid[m.x + 1][m.y + 1].dObject - 1 : -(grid[m.x + 1][m.y + 1].dObject + 1);
			if (object[bv]._oSelFlag >= 2) {
				cursm = { m.x + 1, m.y + 1 };
				pcursobj = bv;
			}
		}
	}
	if (pcursplr == -1 && pcursobj == -1 && pcursmonst == -1) {
		if (!flipflag && m.x + 1 < MAXDUNX && grid[m.x + 1][m.y].dItem > 0) {
			bv = grid[m.x + 1][m.y].dItem - 1;
			if (item[bv]._iSelFlag >= 2) {
				cursm = { m.x + 1, m.y };
				pcursitem = bv;
			}
		}
		if (flipflag && m.y + 1 < MAXDUNY && grid[m.x][m.y + 1].dItem > 0) {
			bv = grid[m.x][m.y + 1].dItem - 1;
			if (item[bv]._iSelFlag >= 2) {
				cursm = { m.x, m.y + 1 };
				pcursitem = bv;
			}
		}
		if (grid[m.x][m.y].dItem > 0) {
			bv = grid[m.x][m.y].dItem - 1;
			if (item[bv]._iSelFlag == 1 || item[bv]._iSelFlag == 3) {
				cursm = m;
				pcursitem = bv;
			}
		}
		if (m.x + 1 < MAXDUNX && m.y + 1 < MAXDUNY && grid[m.x + 1][m.y + 1].dItem > 0) {
			bv = grid[m.x + 1][m.y + 1].dItem - 1;
			if (item[bv]._iSelFlag >= 2) {
				cursm = { m.x + 1, m.y + 1 };
				pcursitem = bv;
			}
		}
		if (pcursitem == -1) {
			cursm = m;
			CheckTrigForce();
			CheckTown();
			CheckRportal();
		}
	}

	if (pcurs == CURSOR_IDENTIFY) {
		pcursobj = -1;
		pcursmonst = -1;
		pcursitem = -1;
		cursm = m;
	}
	if (pcursmonst != -1 && monsters[pcursmonst].data._mFlags & MFLAG_GOLEM) {
		pcursmonst = -1;
	}
}

DEVILUTION_END_NAMESPACE
