/**
 * @file gamemenu.cpp
 *
 * Implementation of the in-game menu functions.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

GameDialog dialog;

GameDialog::GameDialog()
{
}

void GameDialog::DrawSTextBack()
{
	CelDraw(PANEL_X + 344, 487, pSTextBoxCels, 1, 271);
	trans_rect(PANEL_LEFT + 347, 28, 265, 297);
}

void GameDialog::PrintSString(int x, int y, bool cjustflag, char *str,
	                            char col, int val)
{
	int xx, yy;
	int len, width, sx, sy, i, k, s;
	uint8_t c;
	char valstr[32];

	s = y * 12 + stext[y]._syoff;
	if (stextsize)
		xx = PANEL_X + 32;
	else
		xx = PANEL_X + 352;
	sx = xx + x;
	sy = s + 204;
	len = strlen(str);
	if (stextsize)
		yy = 577;
	else
		yy = 257;
	k = 0;
	if (cjustflag) {
		width = 0;
		for (i = 0; i < len; i++)
			width +=
				fontkern[fontframe[gbFontTransTbl[(uint8_t)str[i]]]] + 1;
		if (width < yy) k = (yy - width) >> 1;
		sx += k;
	}
	if (stextsel == y) {
		CelDraw(cjustflag ? xx + x + k - 20 : xx + x - 20, s + 205,
			    pSPentSpn2Cels, PentSpn2Frame, 12);
	}
	for (i = 0; i < len; i++) {
		c = fontframe[gbFontTransTbl[(uint8_t)str[i]]];
		k += fontkern[c] + 1;
		if (c && k <= yy) { PrintChar({sx, sy}, c, col); }
		sx += fontkern[c] + 1;
	}
	if (!cjustflag && val >= 0) {
		sprintf(valstr, "%i", val);
		sx = PANEL_X + 592 - x;
		for (i = strlen(valstr) - 1; i >= 0; i--) {
			c = fontframe[gbFontTransTbl[(uint8_t)valstr[i]]];
			sx -= fontkern[c] + 1;
			if (c) { PrintChar({sx, sy}, c, col); }
		}
	}
	if (stextsel == y) {
		CelDraw(cjustflag ? (xx + x + k + 4) : (PANEL_X + 596 - x), s + 205,
			    pSPentSpn2Cels, PentSpn2Frame, 12);
	}
}

void GameDialog::DrawSLine(int y)
{
	int xy, yy, width, line, sy;

	sy = y * 12;
	if (stextsize) {
		xy = SCREENXY(PANEL_LEFT + 26, 25);
		yy = BUFFER_WIDTH * (sy + 198) + 26 + PANEL_X;
		width = 586 / 4;
		line = BUFFER_WIDTH - 586;
	} else {
		xy = SCREENXY(PANEL_LEFT + 346, 25);
		yy = BUFFER_WIDTH * (sy + 198) + 346 + PANEL_X;
		width = 266 / 4;
		line = BUFFER_WIDTH - 266;
	}

	/// ASSERT: assert(gpBuffer);

	int i;
	uint8_t *src, *dst;

	src = &gpBuffer[xy];
	dst = &gpBuffer[yy];

	for (i = 0; i < 3; i++, src += BUFFER_WIDTH, dst += BUFFER_WIDTH)
		memcpy(dst, src, BUFFER_WIDTH - line);
}

void GameDialog::DrawSSlider(int y1, int y2)
{
	int yd1, yd2, yd3;

	yd1 = y1 * 12 + 204;
	yd2 = y2 * 12 + 204;
	if (stextscrlubtn != -1)
		CelDraw(PANEL_X + 601, yd1, pSTextSlidCels, 12, 12);
	else
		CelDraw(PANEL_X + 601, yd1, pSTextSlidCels, 10, 12);
	if (stextscrldbtn != -1)
		CelDraw(PANEL_X + 601, yd2, pSTextSlidCels, 11, 12);
	else
		CelDraw(PANEL_X + 601, yd2, pSTextSlidCels, 9, 12);
	yd1 += 12;
	for (yd3 = yd1; yd3 < yd2; yd3 += 12) {
		CelDraw(PANEL_X + 601, yd3, pSTextSlidCels, 14, 12);
	}
	if (stextsel == 22)
		yd3 = stextlhold;
	else
		yd3 = stextsel;
	if (storenumh > 1)
		yd3 = 1000 * (stextsval + ((yd3 - stextup) >> 2)) /
			    (storenumh - 1) * (y2 * 12 - y1 * 12 - 24) / 1000;
	else
		yd3 = 0;
	CelDraw(PANEL_X + 601, (y1 + 1) * 12 + 204 + yd3, pSTextSlidCels, 13,
		    12);
}

void GameDialog::DrawSTextHelp()
{
	stextsel = -1;
	stextsize = true;
}

void GameDialog::ClearSText(int s, int e)
{
	int i;

	for (i = s; i < e; i++) {
		stext[i]._sx = 0;
		stext[i]._syoff = 0;
		stext[i]._sstr[0] = 0;
		stext[i]._sjust = false;
		stext[i]._sclr = COL_WHITE;
		stext[i]._sline = 0;
		stext[i]._ssel = false;
		stext[i]._sval = -1;
	}
}

void GameDialog::AddSLine(int y)
{
	stext[y]._sx = 0;
	stext[y]._syoff = 0;
	stext[y]._sstr[0] = 0;
	stext[y]._sline = 1;
}

void GameDialog::AddSTextVal(int y, int val)
{
	stext[y]._sval = val;
}

void GameDialog::OffsetSTextY(int y, int yo)
{
	stext[y]._syoff = yo;
}

void GameDialog::AddSText(int x, int y, bool j, char *str, char clr,
	                        bool sel)
{
	stext[y]._sx = x;
	stext[y]._syoff = 0;
	strcpy(stext[y]._sstr, str);
	stext[y]._sjust = j;
	stext[y]._sclr = clr;
	stext[y]._sline = 0;
	stext[y]._ssel = sel;
}

void GameDialog::DrawSText()
{
	int i;

	if (!stextsize)
		DrawSTextBack();
	else
		DrawQTextBack();

	if (stextscrl) {
		switch (stextflag) {
			case STORE_SBUY:
				smithStore.S_ScrollBuy(stextsval);
				break;
			case STORE_SSELL:
			case STORE_SREPAIR:
			case STORE_WSELL:
			case STORE_WRECHARGE:
			case STORE_SIDENTIFY:
				smithStore.S_ScrollSell(stextsval);
				break;
			case STORE_WBUY:
				witchStore.S_ScrollBuy(stextsval);
				break;
			case STORE_HBUY:
				healerStore.S_ScrollBuy(stextsval);
				break;
			case STORE_SPBUY:
				smithStore.S_ScrollPremiumBuy(stextsval);
				break;
		}
	}

	for (i = 0; i < 24; i++) {
		if (stext[i]._sline) DrawSLine(i);
		if (stext[i]._sstr[0])
			PrintSString(stext[i]._sx, i, stext[i]._sjust, stext[i]._sstr,
				            stext[i]._sclr, stext[i]._sval);
	}

	if (stextscrl) DrawSSlider(4, 20);

	PentSpn2Spin();
}

void GameDialog::STextESC()
{
	if (qtextflag) {
		qtextflag = false;
		if (lvl.type() == DunType::town) stream_stop();
	} else {
		switch (stextflag) {
			case STORE_SMITH:
			case STORE_WITCH:
			case STORE_BOY:
			case STORE_BBOY:
			case STORE_HEALER:
			case STORE_STORY:
			case STORE_TAVERN:
			case STORE_DRUNK:
			case STORE_BARMAID:
				stextflag = STORE_NONE;
				break;
			case STORE_GOSSIP:
				StartStore(stextshold);
				stextsel = stextlhold;
				break;
			case STORE_SBUY:
				StartStore(STORE_SMITH);
				stextsel = 12;
				break;
			case STORE_SPBUY:
				StartStore(STORE_SMITH);
				stextsel = 14;
				break;
			case STORE_SSELL:
				StartStore(STORE_SMITH);
				stextsel = 16;
				break;
			case STORE_SREPAIR:
				StartStore(STORE_SMITH);
				stextsel = 18;
				break;
			case STORE_WBUY:
				StartStore(STORE_WITCH);
				stextsel = 14;
				break;
			case STORE_WSELL:
				StartStore(STORE_WITCH);
				stextsel = 16;
				break;
			case STORE_WRECHARGE:
				StartStore(STORE_WITCH);
				stextsel = 18;
				break;
			case STORE_HBUY:
				StartStore(STORE_HEALER);
				stextsel = 16;
				break;
			case STORE_SIDENTIFY:
				StartStore(STORE_STORY);
				stextsel = 14;
				break;
			case STORE_IDSHOW:
				StartStore(STORE_SIDENTIFY);
				break;
			case STORE_NOMONEY:
			case STORE_NOROOM:
			case STORE_CONFIRM:
				StartStore(stextshold);
				stextsel = stextlhold;
				stextsval = stextvhold;
				break;
		}
	}
}

void GameDialog::STextUp()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel == -1) { return; }

	if (stextscrl) {
		if (stextsel == stextup) {
			if (stextsval) stextsval--;
			return;
		}

		stextsel--;
		while (!stext[stextsel]._ssel) {
			if (!stextsel)
				stextsel = 23;
			else
				stextsel--;
		}
		return;
	}

	if (!stextsel)
		stextsel = 23;
	else
		stextsel--;

	while (!stext[stextsel]._ssel) {
		if (!stextsel)
			stextsel = 23;
		else
			stextsel--;
	}
}

void GameDialog::STextDown()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel == -1) { return; }

	if (stextscrl) {
		if (stextsel == stextdown) {
			if (stextsval < stextsmax) stextsval++;
			return;
		}

		stextsel++;
		while (!stext[stextsel]._ssel) {
			if (stextsel == 23)
				stextsel = 0;
			else
				stextsel++;
		}
		return;
	}

	if (stextsel == 23)
		stextsel = 0;
	else
		stextsel++;

	while (!stext[stextsel]._ssel) {
		if (stextsel == 23)
			stextsel = 0;
		else
			stextsel++;
	}
}

void GameDialog::STextPrior()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1 && stextscrl) {
		if (stextsel == stextup) {
			if (stextsval) stextsval -= 4;
			stextsval = stextsval;
			if (stextsval < 0) stextsval = 0;
		} else {
			stextsel = stextup;
		}
	}
}

void GameDialog::STextNext()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1 && stextscrl) {
		if (stextsel == stextdown) {
			if (stextsval < stextsmax) stextsval += 4;
			if (stextsval > stextsmax) stextsval = stextsmax;
		} else {
			stextsel = stextdown;
		}
	}
}

void GameDialog::FreeQuestText()
{
	MemFreeDbg(pMedTextCels);
	MemFreeDbg(pTextBoxCels);
}

void GameDialog::InitQuestText()
{
	pMedTextCels = LoadFileInMem("Data\\MedTextS.CEL", NULL);
	pTextBoxCels = LoadFileInMem("Data\\TextBox.CEL", NULL);
	qtextflag = false;
}

void GameDialog::InitQTextMsg(int m)
{
	if (alltext[m].scrlltxt) {
		questlog = false;
		qtextptr = alltext[m].txtstr;
		qtextflag = true;
		qtexty = 500;
		int qtextSpd = qscroll_spd_tbl[alltext[m].txtspd - 1];
		int scrolltexty;
		if (qtextSpd <= 0)
			scrolltexty = 50 / -(qtextSpd - 1);
		else
			scrolltexty = ((qtextSpd + 1) * 50) / qtextSpd;
		qtextSpd = SDL_GetTicks();
	}
	PlaySFX(alltext[m].sfxnr);
}

void GameDialog::DrawQTextBack()
{
	CelDraw(PANEL_X + 24, 487, pTextBoxCels, 1, 591);
	trans_rect(PANEL_LEFT + 27, 28, 585, 297);
}

void GameDialog::PrintQTextChr(int sx, int sy, uint8_t *pCelBuff, int nCel)
{
	uint8_t *pStart, *pEnd;

	/// ASSERT: assert(gpBuffer);
	pStart = gpBufStart;
	gpBufStart = &gpBuffer[BUFFER_WIDTH * (49 + SCREEN_Y)];
	pEnd = gpBufEnd;
	gpBufEnd = &gpBuffer[BUFFER_WIDTH * (309 + SCREEN_Y)];
	CelDraw(sx, sy, pCelBuff, nCel, 22);

	gpBufStart = pStart;
	gpBufEnd = pEnd;
}

void GameDialog::DrawQText()
{
	int i, l, w, tx, ty;
	uint8_t c;
	char *p, *pnl, *s;
	char tempstr[128];
	bool doneflag;
	DWORD currTime;

	DrawQTextBack();

	p = qtextptr;
	pnl = NULL;
	tx = 48 + PANEL_X;
	ty = qtexty;

	doneflag = false;
	while (!doneflag) {
		w = 0;
		s = p;
		l = 0;
		while (*s != '\n' && *s != '|' && w < 543) {
			c = gbFontTransTbl[(uint8_t)*s];
			s++;
			if (c != '\0') {
				tempstr[l] = c;
				w += mfontkern[mfontframe[c]] + 2;
			} else {
				l--;
			}
			l++;
		}
		tempstr[l] = '\0';
		if (*s == '|') {
			tempstr[l] = '\0';
			doneflag = true;
		} else if (*s != '\n') {
			while (tempstr[l] != ' ' && l > 0) {
				tempstr[l] = '\0';
				l--;
			}
		}
		for (i = 0; tempstr[i]; i++) {
			p++;
			c = mfontframe[gbFontTransTbl[(uint8_t)tempstr[i]]];
			if (*p == '\n') { p++; }
			if (c != 0) { PrintQTextChr(tx, ty, pMedTextCels, c); }
			tx += mfontkern[c] + 2;
		}
		if (pnl == NULL) { pnl = p; }
		tx = 48 + PANEL_X;
		ty += 38;
		if (ty > 501) { doneflag = true; }
	}

	for (currTime = SDL_GetTicks(); qtextSpd + scrolltexty < currTime;
	     qtextSpd += scrolltexty) {
		qtexty--;
		if (qtexty <= 209) {
			qtexty += 38;
			qtextptr = pnl;
			if (*pnl == '|') { qtextflag = false; }
			break;
		}
	}
}

DEVILUTION_END_NAMESPACE
