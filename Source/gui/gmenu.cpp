/**
 * @file gmenu.cpp
 *
 * Implementation of the in-game navigation and interaction.
 */
#include "all.h"

namespace dvl {

uint8_t *optbar_cel;
bool mouseNavigation;
uint8_t *PentSpin_cel;
TMenuItem *sgpCurrItem;
uint8_t *BigTGold_cel;
int PentSpin_tick;
uint8_t PentSpin_frame;
void (*dword_63447C)(TMenuItem *);
TMenuItem *sgpCurrentMenu;
uint8_t *option_cel;
uint8_t *sgpLogo;
int sgCurrentMenuIdx;

/** Maps from font index to bigtgold.cel frame number. */
const uint8_t lfontframe[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 37, 49, 38, 0, 39, 40, 47,
	42, 43, 41, 45, 52, 44, 53, 55, 36, 27,
	28, 29, 30, 31, 32, 33, 34, 35, 51, 50,
	0, 46, 0, 54, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 42, 0, 43, 0, 0, 0, 1, 2, 3,
	4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
	14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
	24, 25, 26, 20, 0, 21, 0, 0
};

/** Maps from bigtgold.cel frame number to character width. */
const uint8_t lfontkern[] = {
	18, 33, 21, 26, 28, 19, 19, 26, 25, 11,
	12, 25, 19, 34, 28, 32, 20, 32, 28, 20,
	28, 36, 35, 46, 33, 33, 24, 11, 23, 22,
	22, 21, 22, 21, 21, 21, 32, 10, 20, 36,
	31, 17, 13, 12, 13, 18, 16, 11, 20, 21,
	11, 10, 12, 11, 21, 23
};

void gmenu_draw_pause()
{
	if (lvl.currlevel != 0)
		RedBack();
	if (!sgpCurrentMenu) {
		light_table_index = 0;
		gmenu_print_text(316 + PANEL_LEFT, 336, "Pause");
	}
}

void gmenu_print_text(int x, int y, char *pszStr)
{
	uint8_t c;

	while (*pszStr) {
		c = gbFontTransTbl[(uint8_t)*pszStr++];
		c = lfontframe[c];
		if (c != 0)
			CelDrawLight({ x, y }, BigTGold_cel, c, 46, NULL);
		x += lfontkern[c] + 2;
	}
}

void FreeGMenu()
{
	MemFreeDbg(sgpLogo);
	MemFreeDbg(BigTGold_cel);
	MemFreeDbg(PentSpin_cel);
	MemFreeDbg(option_cel);
	MemFreeDbg(optbar_cel);
}

void gmenu_init_menu()
{
	PentSpin_frame = 1;
	sgpCurrentMenu = NULL;
	sgpCurrItem = NULL;
	dword_63447C = NULL;
	sgCurrentMenuIdx = 0;
	mouseNavigation = false;
	sgpLogo = LoadFileInMem("Data\\Diabsmal.CEL", NULL);
	BigTGold_cel = LoadFileInMem("Data\\BigTGold.CEL", NULL);
	PentSpin_cel = LoadFileInMem("Data\\PentSpin.CEL", NULL);
	option_cel = LoadFileInMem("Data\\option.CEL", NULL);
	optbar_cel = LoadFileInMem("Data\\optbar.CEL", NULL);
}

bool gmenu_is_active()
{
	return sgpCurrentMenu != NULL;
}

void gmenu_set_items(TMenuItem *pItem, void (*gmFunc)(TMenuItem *))
{
	int i;

	PauseMode = 0;
	mouseNavigation = false;
	sgpCurrentMenu = pItem;
	dword_63447C = gmFunc;
	if (gmFunc) {
		dword_63447C(sgpCurrentMenu);
		pItem = sgpCurrentMenu;
	}
	sgCurrentMenuIdx = 0;
	if (sgpCurrentMenu) {
		for (i = 0; sgpCurrentMenu[i].fnMenu; i++) {
			sgCurrentMenuIdx++;
		}
	}
	// BUGFIX: OOB access when sgCurrentMenuIdx is 0; should be set to NULL instead. (fixed)
	sgpCurrItem = sgCurrentMenuIdx > 0 ? &sgpCurrentMenu[sgCurrentMenuIdx - 1] : NULL;
	gmenu_up_down(true);
}

void gmenu_up_down(bool isDown)
{
	int i;

	if (!sgpCurrItem) {
		return;
	}
	mouseNavigation = false;
	i = sgCurrentMenuIdx;
	if (sgCurrentMenuIdx) {
		while (i) {
			i--;
			if (isDown) {
				sgpCurrItem++;
				if (!sgpCurrItem->fnMenu)
					sgpCurrItem = &sgpCurrentMenu[0];
			} else {
				if (sgpCurrItem == sgpCurrentMenu)
					sgpCurrItem = &sgpCurrentMenu[sgCurrentMenuIdx];
				sgpCurrItem--;
			}
			if ((sgpCurrItem->dwFlags & GMENU_ENABLED) != 0) {
				if (i)
					PlaySFX(IS_TITLEMOV);
				return;
			}
		}
	}
}

void gmenu_draw()
{
	int y;
	TMenuItem *i;
	DWORD ticks;

	if (sgpCurrentMenu) {
		if (dword_63447C)
			dword_63447C(sgpCurrentMenu);
		CelDraw((SCREEN_WIDTH - 296) / 2 + SCREEN_X, 102 + SCREEN_Y, sgpLogo, 1, 296);
		y = 160 + SCREEN_Y;
		i = sgpCurrentMenu;
		if (sgpCurrentMenu->fnMenu) {
			while (i->fnMenu) {
				gmenu_draw_menu_item(i, y);
				i++;
				y += 45;
			}
		}

		ticks = SDL_GetTicks();
		if ((int)(ticks - PentSpin_tick) > 50) { // BUGFIX: thould be 50ms (Fixed)
			PentSpin_frame++;
			if (PentSpin_frame == 9)
				PentSpin_frame = 1;
			PentSpin_tick = ticks;
		}
	}
}

void gmenu_draw_menu_item(TMenuItem *pItem, int y)
{
	DWORD w, x, nSteps, step, pos, t;
	t = y - 2;
	w = gmenu_get_lfont(pItem);
	if (pItem->dwFlags & GMENU_SLIDER) {
		x = 16 + w / 2 + SCREEN_X;
		CelDraw(x + PANEL_LEFT, t - 8, optbar_cel, 1, 287);
		step = pItem->dwFlags & 0xFFF;
		nSteps = (pItem->dwFlags & 0xFFF000) >> 12;
		if (nSteps < 2)
			nSteps = 2;
		pos = step * 256 / nSteps;
		gmenu_clear_buffer(x + 2 + PANEL_LEFT, t - 10, pos + 13, 28);
		CelDraw(x + 2 + pos + PANEL_LEFT, y - 12, option_cel, 1, 27);
	}
	x = SCREEN_WIDTH / 2 - w / 2 + SCREEN_X;
	light_table_index = (pItem->dwFlags & GMENU_ENABLED) ? 0 : 15;
	gmenu_print_text(x, y, pItem->pszStr);
	if (pItem == sgpCurrItem) {
		CelDraw(x - 54, y + 1, PentSpin_cel, PentSpin_frame, 48);
		CelDraw(x + 4 + w, y + 1, PentSpin_cel, PentSpin_frame, 48);
	}
}

void gmenu_clear_buffer(int x, int y, int width, int height)
{
	uint8_t *i;

	i = gpBuffer + BUFFER_WIDTH * y + x;
	while (height--) {
		memset(i, 205, width);
		i -= BUFFER_WIDTH;
	}
}

int gmenu_get_lfont(TMenuItem *pItem)
{
	char *text;
	int i;
	uint8_t c;

	if (pItem->dwFlags & GMENU_SLIDER)
		return 490;
	text = pItem->pszStr;
	i = 0;
	while (*text) {
		c = gbFontTransTbl[(uint8_t)*text++];
		i += lfontkern[lfontframe[c]] + 2;
	}
	return i - 2;
}

bool gmenu_presskeys(int vkey)
{
	if (!sgpCurrentMenu)
		return false;
	switch (vkey) {
	case DVL_VK_RETURN:
		if ((sgpCurrItem->dwFlags & GMENU_ENABLED) != 0) {
			PlaySFX(IS_TITLEMOV);
			sgpCurrItem->fnMenu(true);
		}
		break;
	case DVL_VK_ESCAPE:
		PlaySFX(IS_TITLEMOV);
		gmenu_set_items(NULL, NULL);
		break;
	case DVL_VK_SPACE:
		return false;
	case DVL_VK_LEFT:
		gmenu_left_right(false);
		break;
	case DVL_VK_RIGHT:
		gmenu_left_right(true);
		break;
	case DVL_VK_UP:
		gmenu_up_down(false);
		break;
	case DVL_VK_DOWN:
		gmenu_up_down(true);
		break;
	}
	return true;
}

void gmenu_left_right(bool isRight)
{
	int step;

	if (sgpCurrItem->dwFlags & GMENU_SLIDER) {
		step = sgpCurrItem->dwFlags & 0xFFF;
		if (isRight) {
			if (step == (int)(sgpCurrItem->dwFlags & 0xFFF000) >> 12)
				return;
			step++;
		} else {
			if (!step)
				return;
			step--;
		}
		sgpCurrItem->dwFlags &= 0xFFFFF000;
		sgpCurrItem->dwFlags |= step;
		sgpCurrItem->fnMenu(false);
	}
}

bool gmenu_on_mouse_move()
{
	int step, nSteps;

	if (!mouseNavigation)
		return false;
	gmenu_get_mouse_slider(&step);
	nSteps = (int)(sgpCurrItem->dwFlags & 0xFFF000) >> 12;
	step *= nSteps;
	step /= 256;

	sgpCurrItem->dwFlags &= 0xFFFFF000;
	sgpCurrItem->dwFlags |= step;
	sgpCurrItem->fnMenu(false);
	return true;
}

bool gmenu_get_mouse_slider(int *plOffset)
{
	*plOffset = 282;
	if (Mouse.x < 282 + PANEL_LEFT) {
		*plOffset = 0;
		return false;
	}
	if (Mouse.x > 538 + PANEL_LEFT) {
		*plOffset = 256;
		return false;
	}
	*plOffset = Mouse.x - 282 - PANEL_LEFT;
	return true;
}

bool gmenu_left_mouse(bool isDown)
{
	TMenuItem *pItem;
	DWORD i, w;
	int dummy;

	if (!isDown) {
		if (mouseNavigation) {
			mouseNavigation = false;
			return true;
		} else {
			return false;
		}
	}

	if (!sgpCurrentMenu) {
		return false;
	}
	if (Mouse.y >= PANEL_TOP) {
		return false;
	}
	if (Mouse.y - 117 < 0) {
		return true;
	}
	i = (Mouse.y - 117) / 45;
	if (i >= sgCurrentMenuIdx) {
		return true;
	}
	pItem = &sgpCurrentMenu[i];
	if (!(sgpCurrentMenu[i].dwFlags & GMENU_ENABLED)) {
		return true;
	}
	w = gmenu_get_lfont(pItem);
	if (Mouse.x < SCREEN_WIDTH / 2 - w / 2) {
		return true;
	}
	if (Mouse.x > SCREEN_WIDTH / 2 + w / 2) {
		return true;
	}
	sgpCurrItem = pItem;
	PlaySFX(IS_TITLEMOV);
	if (pItem->dwFlags & GMENU_SLIDER) {
		mouseNavigation = gmenu_get_mouse_slider(&dummy);
		gmenu_on_mouse_move();
	} else {
		sgpCurrItem->fnMenu(true);
	}
	return true;
}

void gmenu_enable(TMenuItem *pMenuItem, bool enable)
{
	if (enable)
		pMenuItem->dwFlags |= GMENU_ENABLED;
	else
		pMenuItem->dwFlags &= ~GMENU_ENABLED;
}

/**
 * @brief Set the TMenuItem slider position based on the given value
 */
void gmenu_slider_set(TMenuItem *pItem, int min, int max, int value)
{
	int nSteps;

	assert(pItem);
	nSteps = (int)(pItem->dwFlags & 0xFFF000) >> 12;
	if (nSteps < 2)
		nSteps = 2;
	pItem->dwFlags &= 0xFFFFF000;
	pItem->dwFlags |= ((max - min - 1) / 2 + (value - min) * nSteps) / (max - min);
}

/**
 * @brief Get the current value for the slider
 */
int gmenu_slider_get(TMenuItem *pItem, int min, int max)
{
	int nSteps, step;

	step = pItem->dwFlags & 0xFFF;
	nSteps = (int)(pItem->dwFlags & 0xFFF000) >> 12;
	if (nSteps < 2)
		nSteps = 2;
	return min + (step * (max - min) + (nSteps - 1) / 2) / nSteps;
}

/**
 * @brief Set the number of steps for the slider
 */
void gmenu_slider_steps(TMenuItem *pItem, int steps)
{
	pItem->dwFlags &= 0xFF000FFF;
	pItem->dwFlags |= (steps << 12) & 0xFFF000;
}

}
