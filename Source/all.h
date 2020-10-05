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

#include "actors/monsters/monstdat.h"
#include "actors/monsters/monster.h"
#include "actors/monsters/monster_defs.h"
#include "actors/monsters/monster_enums.h"
#include "actors/monsters/monster_type.h"
#include "actors/monsters/monster_beastiary.h"
#include "actors/monsters/monster_instance.h"
#include "actors/npcs/npc.h"
#include "actors/npcs/npc_manager.h"
#include "actors/npcs/towner_defs.h"
#include "actors/npcs/towner_enums.h"
#include "actors/npcs/towners.h"
#include "actors/player/player_attribute.h"
#include "actors/player/player_defs.h"
#include "actors/player/player_enums.h"
#include "actors/player/player_instance.h"
#include "actors/player/character.h"
#include "actors/player/inv.h"
#include "actors/player/inv_enums.h"
#include "actors/player/inv_slot.h"
#include "actors/actor.h"
#include "actors/actor_manager.h"
#include "controls/control.h"
#include "controls/cursor.h"
#include "controls/game_instance.h"
#include "graphics/capture.h"
#include "graphics/codec.h"
#include "graphics/dx.h"
#include "graphics/effects.h"
#include "graphics/lighting.h"
#include "graphics/palette.h"
#include "graphics/scrollrt.h"
#include "gui/dialog.h"
#include "gui/gamemenu.h"
#include "gui/gmenu.h"
#include "gui/gui.h"
#include "gui/help.h"
#include "gui/interfac.h"
#include "gui/mainmenu.h"
#include "gui/minitext.h"
#include "items/stores/store_base.h"
#include "items/stores/store_boy.h"
#include "items/stores/store_healer.h"
#include "items/stores/store_misc.h"
#include "items/stores/store_smith.h"
#include "items/stores/store_witch.h"
#include "items/itemdat.h"
#include "items/item_defs.h"
#include "items/item_enums.h"
#include "items/item_id.h"
#include "items/item_instance.h"
#include "items/item_manager.h"
#include "items/items.h"
#include "items/stores.h"
#include "level/generators/drlg_l1.h"
#include "level/generators/drlg_l2.h"
#include "level/generators/drlg_l3.h"
#include "level/generators/drlg_l4.h"
#include "level/generators/gendung.h"
#include "level/generators/themes.h"
#include "level/generators/town.h"
#include "level/automap.h"
#include "level/dgrid.h"
#include "level/grid.h"
#include "level/level.h"
#include "level/level_enums.h"
#include "level/pieces.h"
#include "level/portal.h"
#include "level/setmaps.h"
#include "level/tile.h"
#include "level/track.h"
#include "level/trigs.h"
#include "media/movie.h"
#include "media/mpqapi.h"
#include "media/sound.h"
#include "media/sound_enums.h"
#include "media/wave.h"
#include "missiles/misdat.h"
#include "missiles/missile_defs.h"
#include "missiles/missile_enums.h"
#include "missiles/missiles.h"
#include "multiplayer/msg.h"
#include "multiplayer/multi.h"
#include "multiplayer/sync.h"
#include "objects/object_defs.h"
#include "objects/object_enums.h"
#include "objects/objdat.h"
#include "objects/objects.h"
#include "pathfinding/directions.h"
#include "pathfinding/micropather.h"
#include "pathfinding/path.h"
#include "pathfinding/pathfinder.h"
#include "skills/skill_defs.h"
#include "skills/skill_enums.h"
#include "skills/spelldat.h"
#include "skills/spells.h"
#include "appfat.h"
#include "appfat.h"
#include "dead.h"
#include "debug.h"
#include "diablo.h"
#include "doom.h"
#include "dthread.h"
#include "encrypt.h"
#include "engine.h"
#include "error.h"
#include "init.h"
#include "loadsave.h"
#include "nthread.h"
#include "pack.h"
#include "pfile.h"
#include "plrmsg.h"
#include "quests/quests.h"
#include "restrict.h"
#include "sha.h"
#include "textdat.h" // check file name
#include "tmsg.h"
#include "vectordvl.h"

#include "graphics/render.h" // linked last, likely .s/.asm

#endif /* __ALL_H__ */
