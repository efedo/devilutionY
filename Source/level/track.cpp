#include "all.h"

namespace dvl {

static uint8_t sgbIsScrolling;
static DWORD sgdwLastWalk;
static bool sgbIsWalking;

void track_process()
{
	if (!sgbIsWalking)
		return;

	if (cursm.x < 0 || cursm.x >= MAXDUNX - 1 || cursm.y < 0 || cursm.y >= MAXDUNY - 1)
		return;

	if (myplr().data._pVar8 <= 6 && myplr().data._pmode != PlayerMode::STAND)
		return;

	if (cursm != myplr().data._pathtarg) {
		DWORD tick = SDL_GetTicks();
		if ((int)(tick - sgdwLastWalk) >= 300) {
			sgdwLastWalk = tick;
			NetSendCmdLoc(true, Cmd::WALKXY, cursm);
			if (!sgbIsScrolling)
				sgbIsScrolling = true;
		}
	}
}

void track_repeat_walk(bool rep)
{
	if (sgbIsWalking == rep)
		return;

	sgbIsWalking = rep;
	if (rep) {
		sgbIsScrolling = false;
		sgdwLastWalk = SDL_GetTicks() - 50;
		NetSendCmdLoc(true, Cmd::WALKXY, cursm);
	} else if (sgbIsScrolling) {
		sgbIsScrolling = false;
	}
}

bool track_isscrolling()
{
	return sgbIsScrolling;
}

}
