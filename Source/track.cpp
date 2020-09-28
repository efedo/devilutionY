#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

static uint8_t sgbIsScrolling;
static DWORD sgdwLastWalk;
static bool sgbIsWalking;

void track_process()
{
	if (!sgbIsWalking)
		return;

	if (cursm.x < 0 || cursm.x >= MAXDUNX - 1 || cursm.y < 0 || cursm.y >= MAXDUNY - 1)
		return;

	if (myplr().data._pVar8 <= 6 && myplr().data._pmode != PM_STAND)
		return;

	if (cursm != myplr().data._pathtarg) {
		DWORD tick = SDL_GetTicks();
		if ((int)(tick - sgdwLastWalk) >= 300) {
			sgdwLastWalk = tick;
			NetSendCmdLoc(TRUE, CMD_WALKXY, cursm);
			if (!sgbIsScrolling)
				sgbIsScrolling = TRUE;
		}
	}
}

void track_repeat_walk(bool rep)
{
	if (sgbIsWalking == rep)
		return;

	sgbIsWalking = rep;
	if (rep) {
		sgbIsScrolling = FALSE;
		sgdwLastWalk = SDL_GetTicks() - 50;
		NetSendCmdLoc(TRUE, CMD_WALKXY, cursm);
	} else if (sgbIsScrolling) {
		sgbIsScrolling = FALSE;
	}
}

bool track_isscrolling()
{
	return sgbIsScrolling;
}

DEVILUTION_END_NAMESPACE
