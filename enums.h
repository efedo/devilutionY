/**
 * @file enums.h
 *
 * Various global enumerators.
 */

#ifndef __ENUMS_H__
#define __ENUMS_H__

namespace dvl {

typedef enum FILE_SYSTEM {
	FS_PC = 0,
	FS_CD = 1,
} FILE_SYSTEM;

typedef enum _artfonts {
	AF_SMALL     = 0,
	AF_SMALLGRAY = 1,
	AF_MED       = 2,
	AF_MEDGRAY   = 3,
	AF_BIG       = 4,
	AF_BIGGRAY   = 5,
	AF_HUGE      = 6,
	AF_HUGEGRAY  = 7,
} _artfonts;

typedef enum dungeon_message {
	DMSG_CATHEDRAL = 1 << 0,
	DMSG_CATACOMBS = 1 << 1,
	DMSG_CAVES     = 1 << 2,
	DMSG_HELL      = 1 << 3,
	DMSG_DIABLO    = 1 << 4,
} dungeon_message;

typedef enum diablo_message {
	EMSG_NONE                   = 0,
	EMSG_NO_AUTOMAP_IN_TOWN     = 1,
	EMSG_NO_MULTIPLAYER_IN_DEMO = 2,
	EMSG_DIRECT_SOUND_FAILED    = 3,
	EMSG_NOT_IN_SHAREWARE       = 4,
	EMSG_NO_SPACE_TO_SAVE       = 5,
	EMSG_NO_PAUSE_IN_TOWN       = 6,
	EMSG_COPY_TO_HDD            = 7,
	EMSG_DESYNC                 = 8,
	EMSG_NO_PAUSE_IN_MP         = 9,
	EMSG_LOADING                = 10,
	EMSG_SAVING                 = 11,
	EMSG_SHRINE_MYSTERIOUS      = 12,
	EMSG_SHRINE_HIDDEN          = 13,
	EMSG_SHRINE_GLOOMY          = 14,
	EMSG_SHRINE_WEIRD           = 15,
	EMSG_SHRINE_MAGICAL         = 16,
	EMSG_SHRINE_STONE           = 17,
	EMSG_SHRINE_RELIGIOUS       = 18,
	EMSG_SHRINE_ENCHANTED       = 19,
	EMSG_SHRINE_THAUMATURGIC    = 20,
	EMSG_SHRINE_FASCINATING     = 21,
	EMSG_SHRINE_CRYPTIC         = 22,
	EMSG_SHRINE_UNUSED          = 23,
	EMSG_SHRINE_ELDRITCH        = 24,
	EMSG_SHRINE_EERIE           = 25,
	EMSG_SHRINE_DIVINE          = 26,
	EMSG_SHRINE_HOLY            = 27,
	EMSG_SHRINE_SACRED          = 28,
	EMSG_SHRINE_SPIRITUAL       = 29,
	EMSG_SHRINE_SPOOKY1         = 30,
	EMSG_SHRINE_SPOOKY2         = 31,
	EMSG_SHRINE_ABANDONED       = 32,
	EMSG_SHRINE_CREEPY          = 33,
	EMSG_SHRINE_QUIET           = 34,
	EMSG_SHRINE_SECLUDED        = 35,
	EMSG_SHRINE_ORNATE          = 36,
	EMSG_SHRINE_GLIMMERING      = 37,
	EMSG_SHRINE_TAINTED1        = 38,
	EMSG_SHRINE_TAINTED2        = 39,
	EMSG_REQUIRES_LVL_8         = 40,
	EMSG_REQUIRES_LVL_13        = 41,
	EMSG_REQUIRES_LVL_17        = 42,
	EMSG_BONECHAMB              = 43,
} diablo_message;

typedef enum event_type {
	EVENT_TYPE_PLAYER_CREATE_GAME = 1,
	EVENT_TYPE_2                  = 2,
	EVENT_TYPE_PLAYER_LEAVE_GAME  = 3,
	EVENT_TYPE_PLAYER_MESSAGE     = 4,
	EVENT_TYPE_5                  = 5,
	EVENT_TYPE_6                  = 6,
	EVENT_TYPE_7                  = 7,
	EVENT_TYPE_8                  = 8,
	EVENT_TYPE_9                  = 9,
	EVENT_TYPE_10                 = 10,
	EVENT_TYPE_11                 = 11,
	EVENT_TYPE_12                 = 12,
	EVENT_TYPE_13                 = 13,
	EVENT_TYPE_14                 = 14,
	EVENT_TYPE_15                 = 15,
} event_type;

typedef enum _copyprot_results {
	COPYPROT_OK     = 1,
	COPYPROT_CANCEL = 2,
} _copyprot_results;

typedef enum text_color {
	COL_WHITE = 0x0,
	COL_BLUE  = 0x1,
	COL_RED   = 0x2,
	COL_GOLD  = 0x3,
} text_color;

enum class Difficulty {
	NORMAL				= 0x0,
	NIGHTMARE			= 0x1,
	HELL				= 0x2,
	NUM_DIFFICULTIES	= 0x3,
};

enum class Cursor {
	NONE        = 0x0,
	HAND        = 0x1,
	IDENTIFY    = 0x2,
	REPAIR      = 0x3,
	RECHARGE    = 0x4,
	DISARM      = 0x5,
	OIL         = 0x6,
	TELEKINESIS = 0x7,
	RESURRECT   = 0x8,
	TELEPORT    = 0x9,
	HEALOTHER   = 0xA,
	HOURGLASS   = 0xB,
	FIRSTITEM   = 0xC,
};

typedef enum interface_mode {
	WM_DIABNEXTLVL  = 0x402, // WM_USER+2
	WM_DIABPREVLVL  = 0x403,
	WM_DIABRTNLVL   = 0x404,
	WM_DIABSETLVL   = 0x405, // load a 'set' level
	WM_DIABWARPLVL  = 0x406,
	WM_DIABTOWNWARP = 0x407,
	WM_DIABTWARPUP  = 0x408,
	WM_DIABRETOWN   = 0x409,
	WM_DIABNEWGAME  = 0x40A,
	WM_DIABLOADGAME = 0x40B
	// WM_LEIGHSKIP = 0x40C, // psx only
	// WM_DIAVNEWLVL = 0x40D, // psx only
} interface_mode;

typedef enum game_info {
	GAMEINFO_NAME         = 1,
	GAMEINFO_PASSWORD     = 2,
	GAMEINFO_STATS        = 3,
	GAMEINFO_MODEFLAG     = 4,
	GAMEINFO_GAMETEMPLATE = 5,
	GAMEINFO_PLAYERS      = 6,
} game_info;

enum class Cmd : uint8_t {
	STAND,
	WALKXY,
	ACK_PLRINFO,
	ADDSTR,
	ADDMAG,
	ADDDEX,
	ADDVIT,
	SBSPELL,
	GETITEM,
	AGETITEM,
	PUTITEM,
	RESPAWNITEM,
	ATTACKXY,
	RATTACKXY,
	SPELLXY,
	TSPELLXY,
	OPOBJXY,
	DISARMXY,
	ATTACKID,
	ATTACKPID,
	RATTACKID,
	RATTACKPID,
	SPELLID,
	SPELLPID,
	TSPELLID,
	TSPELLPID,
	RESURRECT,
	OPOBJT,
	KNOCKBACK,
	TALKXY,
	NEWLVL,
	WARP,
	CHEAT_EXPERIENCE,
	CHEAT_SPELL_LEVEL,
	DEBUG,
	SYNCDATA,
	MONSTDEATH,
	MONSTDAMAGE,
	PLRDEAD,
	REQUESTGITEM,
	REQUESTAGITEM,
	GOTOGETITEM,
	GOTOAGETITEM,
	OPENDOOR,
	CLOSEDOOR,
	OPERATEOBJ,
	PLROPOBJ,
	BREAKOBJ,
	CHANGEPLRITEMS,
	DELPLRITEMS,
	PLRDAMAGE,
	PLRLEVEL,
	DROPITEM,
	PLAYER_JOINLEVEL,
	SEND_PLRINFO,
	SATTACKXY,
	ACTIVATEPORTAL,
	DEACTIVATEPORTAL,
	DLEVEL_0,
	DLEVEL_1,
	DLEVEL_2,
	DLEVEL_3,
	DLEVEL_4,
	DLEVEL_5,
	DLEVEL_6,
	DLEVEL_7,
	DLEVEL_8,
	DLEVEL_9,
	DLEVEL_10,
	DLEVEL_11,
	DLEVEL_12,
	DLEVEL_13,
	DLEVEL_14,
	DLEVEL_15,
	DLEVEL_16,
	DLEVEL_JUNK,
	DLEVEL_END,
	HEALOTHER,
	STRING,
	SETSTR,
	SETMAG,
	SETDEX,
	SETVIT,
	RETOWN,
	SPELLXYD,
	ITEMEXTRA,
	SYNCPUTITEM,
	KILLGOLEM,
	SYNCQUEST,
	ENDSHIELD,
	AWAKEGOLEM,
	NOVA,
	SETSHIELD,
	REMSHIELD,
	FAKE_SETID,
	FAKE_DROPID,
	NUM_CMDS,
	INVALID = 255
};

typedef enum _music_id {
	TMUSIC_TOWN,
	TMUSIC_L1,
	TMUSIC_L2,
	TMUSIC_L3,
	TMUSIC_L4,
	TMUSIC_INTRO,
	NUM_MUSIC,
} _music_id;

typedef enum _mainmenu_selections {
	MAINMENU_SINGLE_PLAYER = 1,
	MAINMENU_MULTIPLAYER   = 2,
	MAINMENU_REPLAY_INTRO  = 3,
	MAINMENU_SHOW_CREDITS  = 4,
	MAINMENU_EXIT_DIABLO   = 5,
	MAINMENU_ATTRACT_MODE  = 6,
} _mainmenu_selections;

typedef enum _selhero_selections {
	SELHERO_NEW_DUNGEON = 1,
	SELHERO_CONTINUE    = 2,
	SELHERO_CONNECT     = 3,
	SELHERO_PREVIOUS    = 4
} _selhero_selections;

typedef enum panel_button_id {
	PANBTN_CHARINFO  = 0,
	PANBTN_QLOG      = 1,
	PANBTN_AUTOMAP   = 2,
	PANBTN_MAINMENU  = 3,
	PANBTN_INVENTORY = 4,
	PANBTN_SPELLBOOK = 5,
	PANBTN_SENDMSG   = 6,
	PANBTN_FRIENDLY  = 7,
} panel_button_id;


typedef enum conn_type {
	#ifndef NONET
	SELCONN_TCP,
	#ifdef BUGGY
	SELCONN_UDP,
	#endif
	#endif
	SELCONN_LOOPBACK,
} conn_type;

}

#endif /* __ENUMS_H__ */
