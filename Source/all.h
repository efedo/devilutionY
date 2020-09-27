/**
 * @file all.h
 *
 * Include all aplication headers.
 */
#ifndef __ALL_H__
#define __ALL_H__

#include <SDL.h>

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#else
#include "sdl2_backports.h"
#endif

#include "sdl_compat.h"

#include "../types.h"
#include "actors/actor.h"
#include "appfat.h"
#include "appfat.h"
#include "automap.h"
#include "capture.h"
#include "actors/character.h"
#include "codec.h"
#include "control.h"
#include "cursor.h"
#include "dead.h"
#include "debug.h"
#include "diablo.h"
#include "doom.h"
#include "generators/drlg_l1.h"
#include "generators/drlg_l2.h"
#include "generators/drlg_l3.h"
#include "generators/drlg_l4.h"
#include "directions.h"
#include "dthread.h"
#include "dx.h"
#include "effects.h"
#include "encrypt.h"
#include "engine.h"
#include "error.h"
#include "gamemenu.h"
#include "generators/gendung.h"
#include "gmenu.h"
#include "grid/grid.h"
#include "help.h"
#include "init.h"
#include "interfac.h"
#include "actors/inv.h"
#include "itemdat.h"
#include "items.h"
#include "lighting.h"
#include "loadsave.h"
#include "mainmenu.h"
#include "minitext.h"
#include "misdat.h"
#include "missiles.h"
#include "actors/monstdat.h"
#include "actors/monster.h"
#include "actors/monster_type.h"
#include "actors/monster_manager.h"
#include "actors/monster_beastiary.h"
#include "actors/monster_instance.h"
#include "media/movie.h"
#include "media/mpqapi.h"
#include "multiplayer/msg.h"
#include "multiplayer/multi.h"
#include "nthread.h"
#include "objdat.h"
#include "objects.h"
#include "pack.h"
#include "palette.h"
#include "path.h"
#include "micropather/pathfinder.h"
#include "micropather/micropather.h"
#include "pfile.h"
#include "grid/pieces.h"
#include "actors/player_instance.h"
#include "actors/player_manager.h"
#include "plrmsg.h"
#include "portal.h"
#include "quests.h"
#include "restrict.h"
#include "scrollrt.h"
#include "setmaps.h"
#include "sha.h"
#include "sound.h"
#include "spelldat.h"
#include "spells.h"
#include "stores.h"
#include "multiplayer/sync.h"
#include "textdat.h" // check file name
#include "themes.h"
#include "grid/tile.h"
#include "tmsg.h"
#include "town.h"
#include "actors/towners.h"
#include "track.h"
#include "trigs.h"
#include "vectordvl.h"

//DEVILUTION_BEGIN_NAMESPACE
//
//// Lean 2D int vector
//class V2Di {
//public:
//	V2Di(int nx = 0, int ny = 0);
//	int x;
//	int y;
//};
//
//DEVILUTION_END_NAMESPACE

#include "media/wave.h"
#include "render.h" // linked last, likely .s/.asm

#endif /* __ALL_H__ */
