/**
 * @file automap.cpp
 *
 * Implementation of the in-game map overlay.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

Automap automap;

/** color used to draw the player's arrow */
#define COLOR_PLAYER (PAL8_ORANGE + 1)
/** color for bright map lines (doors, stairs etc.) */
#define COLOR_BRIGHT PAL8_YELLOW
/** color for dim map lines/dots */
#define COLOR_DIM (PAL16_YELLOW + 8)

#define MAPFLAG_TYPE 0x000F
/** these are in the second byte */
#define MAPFLAG_VERTDOOR 0x01
#define MAPFLAG_HORZDOOR 0x02
#define MAPFLAG_VERTARCH 0x04
#define MAPFLAG_HORZARCH 0x08
#define MAPFLAG_VERTGRATE 0x10
#define MAPFLAG_HORZGRATE 0x20
#define MAPFLAG_DIRT 0x40
#define MAPFLAG_STAIRS 0x80

/**
 * @brief Initializes the automap.
 */
void Automap::initOnce()
{
	automapflag = FALSE;
	AutoMapScale = 50;
	AmLine64 = 32;
	AmLine32 = 16;
	AmLine16 = 8;
	AmLine8 = 4;
	AmLine4 = 2;
}

/**
 * @brief Loads the mapping between tile IDs and automap shapes.
 */
void Automap::init()
{
	BYTE b1, b2;
	DWORD dwTiles;
	int x, y;
	BYTE *pAFile, *pTmp;
	int i;

	memset(automaptype, 0, sizeof(automaptype));

	switch (level.leveltype) {
	case DTYPE_CATHEDRAL:
		pAFile = LoadFileInMem("Levels\\L1Data\\L1.AMP", &dwTiles);
		break;
	case DTYPE_CATACOMBS:
		pAFile = LoadFileInMem("Levels\\L2Data\\L2.AMP", &dwTiles);
		break;
	case DTYPE_CAVES:
		pAFile = LoadFileInMem("Levels\\L3Data\\L3.AMP", &dwTiles);
		break;
	case DTYPE_HELL:
		pAFile = LoadFileInMem("Levels\\L4Data\\L4.AMP", &dwTiles);
		break;
	default:
		return;
	}

	dwTiles /= 2;
	pTmp = pAFile;

	for (i = 1; i <= dwTiles; i++) {
		b1 = *pTmp++;
		b2 = *pTmp++;
		automaptype[i] = b1 + (b2 << 8);
	}

	mem_free_dbg(pAFile);
	memset(automapview, 0, sizeof(automapview));

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++)
			grid[x][y].dFlags &= ~BFLAG_EXPLORED;
	}
}

/**
 * @brief Displays the automap.
 */
void Automap::start()
{
	AutoMapOfs = { 0, 0 };
	automapflag = TRUE;
}

/**
 * @brief Scrolls the automap upwards.
 */
void Automap::up()
{
	AutoMapOfs.x--;
	AutoMapOfs.y--;
}

/**
 * @brief Scrolls the automap downwards.
 */
void Automap::down()
{
	AutoMapOfs.x++;
	AutoMapOfs.y++;
}

/**
 * @brief Scrolls the automap leftwards.
 */
void Automap::left()
{
	AutoMapOfs.x--;
	AutoMapOfs.y++;
}

/**
 * @brief Scrolls the automap rightwards.
 */
void Automap::right()
{
	AutoMapOfs.x++;
	AutoMapOfs.y--;
}

/**
 * @brief Increases the zoom level of the automap.
 */
void Automap::zoomIn()
{
	if (AutoMapScale < 200) {
		AutoMapScale += 5;
		AmLine64 = (AutoMapScale << 6) / 100;
		AmLine32 = AmLine64 >> 1;
		AmLine16 = AmLine32 >> 1;
		AmLine8 = AmLine16 >> 1;
		AmLine4 = AmLine8 >> 1;
	}
}

/**
 * @brief Decreases the zoom level of the automap.
 */
void Automap::zoomOut()
{
	if (AutoMapScale > 50) {
		AutoMapScale -= 5;
		AmLine64 = (AutoMapScale << 6) / 100;
		AmLine32 = AmLine64 >> 1;
		AmLine16 = AmLine32 >> 1;
		AmLine8 = AmLine16 >> 1;
		AmLine4 = AmLine8 >> 1;
	}
}

/**
 * @brief Renders the automap on screen.
 */
void Automap::draw()
{
	int cells;
	V2Di s, map;
	int i, j, d;

	if (level.leveltype == DTYPE_TOWN) {
		drawText();
		return;
	}

	gpBufEnd = &gpBuffer[BUFFER_WIDTH * (SCREEN_Y + VIEWPORT_HEIGHT)];

	AutoMap.x = (View.x - 16) >> 1;
	while (AutoMap.x + AutoMapOfs.x < 0)
		AutoMapOfs.x++;
	while (AutoMap.x + AutoMapOfs.x >= DMAXX)
		AutoMapOfs.x--;
	AutoMap.x += AutoMapOfs.x;

	AutoMap.y = (View.y - 16) >> 1;
	while (AutoMap.y + AutoMapOfs.y < 0)
		AutoMapOfs.y++;
	while (AutoMap.y + AutoMapOfs.y >= DMAXY)
		AutoMapOfs.y--;
	AutoMap.y += AutoMapOfs.y;

	d = (AutoMapScale << 6) / 100;
	cells = 2 * (SCREEN_WIDTH / 2 / d) + 1;
	if ((SCREEN_WIDTH / 2) % d)
		cells++;
	if ((SCREEN_WIDTH / 2) % d >= (AutoMapScale << 5) / 100)
		cells++;

	if (ScrollInfo._soff.x + ScrollInfo._soff.y)
		cells++;
	map.x = AutoMap.x - cells;
	map.y = AutoMap.y - 1;

	if (cells & 1) {
		s.x = SCREEN_WIDTH / 2 + SCREEN_X - AmLine64 * ((cells - 1) >> 1);
		s.y = (SCREEN_HEIGHT - PANEL_HEIGHT) / 2 + SCREEN_Y - AmLine32 * ((cells + 1) >> 1);
	} else {
		s.x = SCREEN_WIDTH / 2 + SCREEN_X - AmLine64 * (cells >> 1) + AmLine32;
		s.y = (SCREEN_HEIGHT - PANEL_HEIGHT) / 2 + SCREEN_Y - AmLine32 * (cells >> 1) - AmLine16;
	}
	if (View.x & 1) {
		s.x -= AmLine16;
		s.y -= AmLine8;
	}
	if (View.y & 1) {
		s.x += AmLine16;
		s.y -= AmLine8;
	}

	s.x += AutoMapScale * ScrollInfo._soff.x / 100 >> 1;
	s.y += AutoMapScale * ScrollInfo._soff.y / 100 >> 1;
	if (PANELS_COVER) {
		if (invflag || sbookflag) {
			s.x -= SCREEN_WIDTH / 4;
		}
		if (chrflag || questlog) {
			s.x += SCREEN_WIDTH / 4;
		}
	}

	for (i = 0; i <= cells + 1; i++) {
		int x = s.x;
		int y;

		for (j = 0; j < cells; j++) {
			WORD maptype = GetAutomapType({ map.x + j, map.y - j }, TRUE);
			if (maptype != 0)
				drawTile({x, s.y}, maptype);
			x += AmLine64;
		}
		map.y++;
		x = s.x - AmLine32;
		y = s.y + AmLine16;
		for (j = 0; j <= cells; j++) {
			WORD maptype = GetAutomapType({map.x + j, map.y - j }, TRUE);
			if (maptype != 0)
				drawTile({ x, y }, maptype);
			x += AmLine64;
		}
		map.x++;
		s.y += AmLine32;
	}
	drawPlayer();
	drawText();
	gpBufEnd = &gpBuffer[BUFFER_WIDTH * (SCREEN_Y + SCREEN_HEIGHT)];
}

/**
 * @brief Renders the given automap shape at the specified screen coordinates.
 */
void Automap::drawTile(V2Di s, WORD automap_type)
{
	BOOL do_vert;
	BOOL do_horz;
	BOOL do_cave_horz;
	BOOL do_cave_vert;
	//int t1.x, t1.y, t2.x, t2.y;
	V2Di t1, t2;

	BYTE flags = automap_type >> 8;

	if (flags & MAPFLAG_DIRT) {
		ENG_set_pixel(s.x, s.y, COLOR_DIM);
		ENG_set_pixel(s.x - AmLine8, s.y - AmLine4, COLOR_DIM);
		ENG_set_pixel(s.x - AmLine8, s.y + AmLine4, COLOR_DIM);
		ENG_set_pixel(s.x + AmLine8, s.y - AmLine4, COLOR_DIM);
		ENG_set_pixel(s.x + AmLine8, s.y + AmLine4, COLOR_DIM);
		ENG_set_pixel(s.x - AmLine16, s.y, COLOR_DIM);
		ENG_set_pixel(s.x + AmLine16, s.y, COLOR_DIM);
		ENG_set_pixel(s.x, s.y - AmLine8, COLOR_DIM);
		ENG_set_pixel(s.x, s.y + AmLine8, COLOR_DIM);
		ENG_set_pixel(s.x + AmLine8 - AmLine32, s.y + AmLine4, COLOR_DIM);
		ENG_set_pixel(s.x - AmLine8 + AmLine32, s.y + AmLine4, COLOR_DIM);
		ENG_set_pixel(s.x - AmLine16, s.y + AmLine8, COLOR_DIM);
		ENG_set_pixel(s.x + AmLine16, s.y + AmLine8, COLOR_DIM);
		ENG_set_pixel(s.x - AmLine8, s.y + AmLine16 - AmLine4, COLOR_DIM);
		ENG_set_pixel(s.x + AmLine8, s.y + AmLine16 - AmLine4, COLOR_DIM);
		ENG_set_pixel(s.x, s.y + AmLine16, COLOR_DIM);
	}

	if (flags & MAPFLAG_STAIRS) {
		DrawLine(s.x - AmLine8, s.y - AmLine8 - AmLine4, s.x + AmLine8 + AmLine16, s.y + AmLine4, COLOR_BRIGHT);
		DrawLine(s.x - AmLine16, s.y - AmLine8, s.x + AmLine16, s.y + AmLine8, COLOR_BRIGHT);
		DrawLine(s.x - AmLine16 - AmLine8, s.y - AmLine4, s.x + AmLine8, s.y + AmLine8 + AmLine4, COLOR_BRIGHT);
		DrawLine(s.x - AmLine32, s.y, s.x, s.y + AmLine16, COLOR_BRIGHT);
	}

	do_vert = FALSE;
	do_horz = FALSE;
	do_cave_horz = FALSE;
	do_cave_vert = FALSE;
	switch (automap_type & MAPFLAG_TYPE) {
	case 1: // stand-alone column or other unpassable object
		t1.x = s.x - AmLine16;
		t1.y = s.y - AmLine16;
		t2.x = t1.x + AmLine32;
		t2.y = s.y - AmLine8;
		DrawLine(s.x, t1.y, t1.x, t2.y, COLOR_DIM);
		DrawLine(s.x, t1.y, t2.x, t2.y, COLOR_DIM);
		DrawLine(s.x, s.y, t1.x, t2.y, COLOR_DIM);
		DrawLine(s.x, s.y, t2.x, t2.y, COLOR_DIM);
		break;
	case 2:
	case 5:
		do_vert = TRUE;
		break;
	case 3:
	case 6:
		do_horz = TRUE;
		break;
	case 4:
		do_vert = TRUE;
		do_horz = TRUE;
		break;
	case 8:
		do_vert = TRUE;
		do_cave_horz = TRUE;
		break;
	case 9:
		do_horz = TRUE;
		do_cave_vert = TRUE;
		break;
	case 10:
		do_cave_horz = TRUE;
		break;
	case 11:
		do_cave_vert = TRUE;
		break;
	case 12:
		do_cave_horz = TRUE;
		do_cave_vert = TRUE;
		break;
	}

	if (do_vert) {                      // right-facing obstacle
		if (flags & MAPFLAG_VERTDOOR) { // two wall segments with a door in the middle
			t1.x = s.x - AmLine32;
			t2.x = s.x - AmLine16;
			t1.y = s.y - AmLine16;
			t2.y = s.y - AmLine8;

			DrawLine(s.x, t1.y, s.x - AmLine8, t1.y + AmLine4, COLOR_DIM);
			DrawLine(t1.x, s.y, t1.x + AmLine8, s.y - AmLine4, COLOR_DIM);
			DrawLine(t2.x, t1.y, t1.x, t2.y, COLOR_BRIGHT);
			DrawLine(t2.x, t1.y, s.x, t2.y, COLOR_BRIGHT);
			DrawLine(t2.x, s.y, t1.x, t2.y, COLOR_BRIGHT);
			DrawLine(t2.x, s.y, s.x, t2.y, COLOR_BRIGHT);
		}
		if (flags & MAPFLAG_VERTGRATE) { // right-facing half-wall
			DrawLine(s.x - AmLine16, s.y - AmLine8, s.x - AmLine32, s.y, COLOR_DIM);
			flags |= MAPFLAG_VERTARCH;
		}
		if (flags & MAPFLAG_VERTARCH) { // window or passable column
			t1.x = s.x - AmLine16;
			t1.y = s.y - AmLine16;
			t2.x = t1.x + AmLine32;
			t2.y = s.y - AmLine8;

			DrawLine(s.x, t1.y, t1.x, t2.y, COLOR_DIM);
			DrawLine(s.x, t1.y, t2.x, t2.y, COLOR_DIM);
			DrawLine(s.x, s.y, t1.x, t2.y, COLOR_DIM);
			DrawLine(s.x, s.y, t2.x, t2.y, COLOR_DIM);
		}
		if ((flags & (MAPFLAG_VERTDOOR | MAPFLAG_VERTGRATE | MAPFLAG_VERTARCH)) == 0)
			DrawLine(s.x, s.y - AmLine16, s.x - AmLine32, s.y, COLOR_DIM);
	}

	if (do_horz) { // left-facing obstacle
		if (flags & MAPFLAG_HORZDOOR) {
			t1.x = s.x + AmLine16;
			t2.x = s.x + AmLine32;
			t1.y = s.y - AmLine16;
			t2.y = s.y - AmLine8;

			DrawLine(s.x, t1.y, s.x + AmLine8, t1.y + AmLine4, COLOR_DIM);
			DrawLine(t2.x, s.y, t2.x - AmLine8, s.y - AmLine4, COLOR_DIM);
			DrawLine(t1.x, t1.y, s.x, t2.y, COLOR_BRIGHT);
			DrawLine(t1.x, t1.y, t2.x, t2.y, COLOR_BRIGHT);
			DrawLine(t1.x, s.y, s.x, t2.y, COLOR_BRIGHT);
			DrawLine(t1.x, s.y, t2.x, t2.y, COLOR_BRIGHT);
		}
		if (flags & MAPFLAG_HORZGRATE) {
			DrawLine(s.x + AmLine16, s.y - AmLine8, s.x + AmLine32, s.y, COLOR_DIM);
			flags |= MAPFLAG_HORZARCH;
		}
		if (flags & MAPFLAG_HORZARCH) {
			t1.x = s.x - AmLine16;
			t1.y = s.y - AmLine16;
			t2.x = t1.x + AmLine32;
			t2.y = s.y - AmLine8;

			DrawLine(s.x, t1.y, t1.x, t2.y, COLOR_DIM);
			DrawLine(s.x, t1.y, t2.x, t2.y, COLOR_DIM);
			DrawLine(s.x, s.y, t1.x, t2.y, COLOR_DIM);
			DrawLine(s.x, s.y, t2.x, t2.y, COLOR_DIM);
		}
		if ((flags & (MAPFLAG_HORZDOOR | MAPFLAG_HORZGRATE | MAPFLAG_HORZARCH)) == 0)
			DrawLine(s.x, s.y - AmLine16, s.x + AmLine32, s.y, COLOR_DIM);
	}

	// for caves the horz/vert flags are switched
	if (do_cave_horz) {
		if (flags & MAPFLAG_VERTDOOR) {
			t1.x = s.x - AmLine32;
			t2.x = s.x - AmLine16;
			t1.y = s.y + AmLine16;
			t2.y = s.y + AmLine8;

			DrawLine(s.x, t1.y, s.x - AmLine8, t1.y - AmLine4, COLOR_DIM);
			DrawLine(t1.x, s.y, t1.x + AmLine8, s.y + AmLine4, COLOR_DIM);
			DrawLine(t2.x, t1.y, t1.x, t2.y, COLOR_BRIGHT);
			DrawLine(t2.x, t1.y, s.x, t2.y, COLOR_BRIGHT);
			DrawLine(t2.x, s.y, t1.x, t2.y, COLOR_BRIGHT);
			DrawLine(t2.x, s.y, s.x, t2.y, COLOR_BRIGHT);
		} else
			DrawLine(s.x, s.y + AmLine16, s.x - AmLine32, s.y, COLOR_DIM);
	}

	if (do_cave_vert) {
		if (flags & MAPFLAG_HORZDOOR) {
			t1.x = s.x + AmLine16;
			t2.x = s.x + AmLine32;
			t1.y = s.y + AmLine16;
			t2.y = s.y + AmLine8;

			DrawLine(s.x, t1.y, s.x + AmLine8, t1.y - AmLine4, COLOR_DIM);
			DrawLine(t2.x, s.y, t2.x - AmLine8, s.y + AmLine4, COLOR_DIM);
			DrawLine(t1.x, t1.y, s.x, t2.y, COLOR_BRIGHT);
			DrawLine(t1.x, t1.y, t2.x, t2.y, COLOR_BRIGHT);
			DrawLine(t1.x, s.y, s.x, t2.y, COLOR_BRIGHT);
			DrawLine(t1.x, s.y, t2.x, t2.y, COLOR_BRIGHT);
		} else
			DrawLine(s.x, s.y + AmLine16, s.x + AmLine32, s.y, COLOR_DIM);
	}
}

/**
 * @brief Renders an arrow on the automap, centered on and facing the direction of the player.
 */
void Automap::drawPlayer()
{
	int px, py;
	int x, y;

	if (myplr().data._pmode == PM_WALK3) {
		x = myplr().data._pfut.x;
		y = myplr().data._pfut.y;
		if (myplr().data._pdir == DIR_W)
			x++;
		else
			y++;
	} else {
		x = myplr().data._p.x;
		y = myplr().data._p.y;
	}
	px = x - 2 * AutoMapOfs.x - View.x;
	py = y - 2 * AutoMapOfs.y - View.y;

	x = (myplr().data._poff.x * AutoMapScale / 100 >> 1) + (ScrollInfo._soff.x * AutoMapScale / 100 >> 1) + (px - py) * AmLine16 + SCREEN_WIDTH / 2 + SCREEN_X;
	y = (myplr().data._poff.y * AutoMapScale / 100 >> 1) + (ScrollInfo._soff.y * AutoMapScale / 100 >> 1) + (px + py) * AmLine8 + (SCREEN_HEIGHT - PANEL_HEIGHT) / 2 + SCREEN_Y;

	if (PANELS_COVER) {
		if (invflag || sbookflag)
			x -= SCREEN_WIDTH / 4;
		if (chrflag || questlog)
			x += SCREEN_WIDTH / 4;
	}
	y -= AmLine8;

	switch (myplr().data._pdir) {
	case DIR_N:
		DrawLine(x, y, x, y - AmLine16, COLOR_PLAYER);
		DrawLine(x, y - AmLine16, x - AmLine4, y - AmLine8, COLOR_PLAYER);
		DrawLine(x, y - AmLine16, x + AmLine4, y - AmLine8, COLOR_PLAYER);
		break;
	case DIR_NE:
		DrawLine(x, y, x + AmLine16, y - AmLine8, COLOR_PLAYER);
		DrawLine(x + AmLine16, y - AmLine8, x + AmLine8, y - AmLine8, COLOR_PLAYER);
		DrawLine(x + AmLine16, y - AmLine8, x + AmLine8 + AmLine4, y, COLOR_PLAYER);
		break;
	case DIR_E:
		DrawLine(x, y, x + AmLine16, y, COLOR_PLAYER);
		DrawLine(x + AmLine16, y, x + AmLine8, y - AmLine4, COLOR_PLAYER);
		DrawLine(x + AmLine16, y, x + AmLine8, y + AmLine4, COLOR_PLAYER);
		break;
	case DIR_SE:
		DrawLine(x, y, x + AmLine16, y + AmLine8, COLOR_PLAYER);
		DrawLine(x + AmLine16, y + AmLine8, x + AmLine8 + AmLine4, y, COLOR_PLAYER);
		DrawLine(x + AmLine16, y + AmLine8, x + AmLine8, y + AmLine8, COLOR_PLAYER);
		break;
	case DIR_S:
		DrawLine(x, y, x, y + AmLine16, COLOR_PLAYER);
		DrawLine(x, y + AmLine16, x + AmLine4, y + AmLine8, COLOR_PLAYER);
		DrawLine(x, y + AmLine16, x - AmLine4, y + AmLine8, COLOR_PLAYER);
		break;
	case DIR_SW:
		DrawLine(x, y, x - AmLine16, y + AmLine8, COLOR_PLAYER);
		DrawLine(x - AmLine16, y + AmLine8, x - AmLine4 - AmLine8, y, COLOR_PLAYER);
		DrawLine(x - AmLine16, y + AmLine8, x - AmLine8, y + AmLine8, COLOR_PLAYER);
		break;
	case DIR_W:
		DrawLine(x, y, x - AmLine16, y, COLOR_PLAYER);
		DrawLine(x - AmLine16, y, x - AmLine8, y - AmLine4, COLOR_PLAYER);
		DrawLine(x - AmLine16, y, x - AmLine8, y + AmLine4, COLOR_PLAYER);
		break;
	case DIR_NW:
		DrawLine(x, y, x - AmLine16, y - AmLine8, COLOR_PLAYER);
		DrawLine(x - AmLine16, y - AmLine8, x - AmLine8, y - AmLine8, COLOR_PLAYER);
		DrawLine(x - AmLine16, y - AmLine8, x - AmLine4 - AmLine8, y, COLOR_PLAYER);
		break;
	}
}

/**
 * @brief Returns the automap shape at the given coordinate.
 */
WORD Automap::GetAutomapType(V2Di pos, BOOL view)
{
	WORD rv;
	int x = pos.x;
	int y = pos.y;

	if (view && x == -1 && y >= 0 && y < DMAXY && automapview[0][y]) {
		if (GetAutomapType({ 0, y }, FALSE) & (MAPFLAG_DIRT << 8)) {
			return 0;
		} else {
			return MAPFLAG_DIRT << 8;
		}
	}

	if (view && y == -1 && x >= 0 && x < DMAXY && automapview[x][0]) {
		if (GetAutomapType({ x, 0 }, FALSE) & (MAPFLAG_DIRT << 8)) {
			return 0;
		} else {
			return MAPFLAG_DIRT << 8;
		}
	}

	if (x < 0 || x >= DMAXX) {
		return 0;
	}
	if (y < 0 || y >= DMAXX) {
		return 0;
	}
	if (!automapview[x][y] && view) {
		return 0;
	}

	rv = automaptype[(BYTE)dgrid[x][y].dungeon];
	if (rv == 7) {
		if ((GetAutomapType({ x - 1, y }, FALSE) >> 8) & MAPFLAG_HORZARCH) {
			if ((GetAutomapType({ x, y - 1 }, FALSE) >> 8) & MAPFLAG_VERTARCH) {
				rv = 1;
			}
		}
	}
	return rv;
}

/**
 * @brief Renders game info, such as the name of the current level, and in multi player the name of the game and the game password.
 */
void Automap::drawText()
{
	char desc[256];
	int nextline = 20;

	if (gbMaxPlayers > 1) {
		strcat(strcpy(desc, "game: "), szPlayerName);
		PrintGameStr({ 8, 20 }, desc, COL_GOLD);
		nextline = 35;
		if (szPlayerDescript[0]) {
			strcat(strcpy(desc, "password: "), szPlayerDescript);
			PrintGameStr({ 8, 35 }, desc, COL_GOLD);
			nextline = 50;
		}
	}
	if (level.setlevel) {
		PrintGameStr({ 8, nextline }, quest_level_names[(BYTE)level.setlvlnum], COL_GOLD);
	} else if (level.currlevel) {
		sprintf(desc, "Level: %i", level.currlevel);
		PrintGameStr({ 8, nextline }, desc, COL_GOLD);
	}
}

/**
 * @brief Marks the given coordinate as within view on the automap.
 */
void Automap::SetAutomapView(V2Di pos)
{
	WORD maptype, solid;
	int xx, yy;

	xx = (pos.x - 16) >> 1;
	yy = (pos.y - 16) >> 1;

	if (xx < 0 || xx >= DMAXX || yy < 0 || yy >= DMAXY) {
		return;
	}

	automapview[xx][yy] = TRUE;

	maptype = GetAutomapType({ xx, yy }, FALSE);
	solid = maptype & 0x4000;

	switch (maptype & 0xF) {
	case 2:
		if (solid) {
			if (GetAutomapType({ xx, yy + 1 }, FALSE) == 0x4007)
				automapview[xx][yy + 1] = TRUE;
		} else if (GetAutomapType({ xx - 1, yy }, FALSE) & 0x4000) {
			automapview[xx - 1][yy] = TRUE;
		}
		break;
	case 3:
		if (solid) {
			if (GetAutomapType({ xx + 1, yy }, FALSE) == 0x4007)
				automapview[xx + 1][yy] = TRUE;
		} else if (GetAutomapType({ xx, yy - 1 }, FALSE) & 0x4000) {
			automapview[xx][yy - 1] = TRUE;
		}
		break;
	case 4:
		if (solid) {
			if (GetAutomapType({ xx, yy + 1 }, FALSE) == 0x4007)
				automapview[xx][yy + 1] = TRUE;
			if (GetAutomapType({ xx + 1, yy }, FALSE) == 0x4007)
				automapview[xx + 1][yy] = TRUE;
		} else {
			if (GetAutomapType({ xx - 1, yy }, FALSE) & 0x4000)
				automapview[xx - 1][yy] = TRUE;
			if (GetAutomapType({xx, yy - 1 }, FALSE) & 0x4000)
				automapview[xx][yy - 1] = TRUE;
			if (GetAutomapType({ xx - 1, yy - 1 }, FALSE) & 0x4000)
				automapview[xx - 1][yy - 1] = TRUE;
		}
		break;
	case 5:
		if (solid) {
			if (GetAutomapType({ xx, yy - 1 }, FALSE) & 0x4000)
				automapview[xx][yy - 1] = TRUE;
			if (GetAutomapType({ xx, yy + 1 }, FALSE) == 0x4007)
				automapview[xx][yy + 1] = TRUE;
		} else if (GetAutomapType({ xx - 1, yy }, FALSE) & 0x4000) {
			automapview[xx - 1][yy] = TRUE;
		}
		break;
	case 6:
		if (solid) {
			if (GetAutomapType({ xx - 1, yy }, FALSE) & 0x4000)
				automapview[xx - 1][yy] = TRUE;
			if (GetAutomapType({ xx + 1, yy }, FALSE) == 0x4007)
				automapview[xx + 1][yy] = TRUE;
		} else if (GetAutomapType({ xx, yy - 1 }, FALSE) & 0x4000) {
			automapview[xx][yy - 1] = TRUE;
		}
		break;
	}
}

/**
 * @brief Resets the zoom level of the automap.
 */
void Automap::zoomReset()
{
	AutoMapOfs = { 0, 0 };
	AmLine64 = (AutoMapScale << 6) / 100;
	AmLine32 = AmLine64 >> 1;
	AmLine16 = AmLine32 >> 1;
	AmLine8 = AmLine16 >> 1;
	AmLine4 = AmLine8 >> 1;
}

DEVILUTION_END_NAMESPACE
