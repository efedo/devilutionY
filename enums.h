/**
 * @file enums.h
 *
 * Various global enumerators.
 */

#ifndef __ENUMS_H__
#define __ENUMS_H__

DEVILUTION_BEGIN_NAMESPACE

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

typedef enum dflag {
	BFLAG_MISSILE     = 0x01,
	BFLAG_VISIBLE     = 0x02,
	BFLAG_DEAD_PLAYER = 0x04,
	BFLAG_POPULATED   = 0x08,
	BFLAG_MONSTLR     = 0x10,
	BFLAG_PLAYERLR    = 0x20,
	BFLAG_LIT         = 0x40,
	BFLAG_EXPLORED    = 0x80,
} dflag;

typedef enum dungeon_message {
	DMSG_CATHEDRAL = 1 << 0,
	DMSG_CATACOMBS = 1 << 1,
	DMSG_CAVES     = 1 << 2,
	DMSG_HELL      = 1 << 3,
	DMSG_DIABLO    = 1 << 4,
} dungeon_message;

enum class DunType {
	town = 0x0,
	cathedral = 0x1,
	catacombs = 0x2,
	caves = 0x3,
	hell = 0x4,
	none = 0xFF,
};

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

typedef enum magic_type {
	STYPE_FIRE      = 0x0,
	STYPE_LIGHTNING = 0x1,
	STYPE_MAGIC     = 0x2,
} magic_type;

typedef enum theme_id {
	THEME_BARREL            = 0x0,
	THEME_SHRINE            = 0x1,
	THEME_MONSTPIT          = 0x2,
	THEME_SKELROOM          = 0x3,
	THEME_TREASURE          = 0x4,
	THEME_LIBRARY           = 0x5,
	THEME_TORTURE           = 0x6,
	THEME_BLOODFOUNTAIN     = 0x7,
	THEME_DECAPITATED       = 0x8,
	THEME_PURIFYINGFOUNTAIN = 0x9,
	THEME_ARMORSTAND        = 0xA,
	THEME_GOATSHRINE        = 0xB,
	THEME_CAULDRON          = 0xC,
	THEME_MURKYFOUNTAIN     = 0xD,
	THEME_TEARFOUNTAIN      = 0xE,
	THEME_BRNCROSS          = 0xF,
	THEME_WEAPONRACK        = 0x10,
	THEME_NONE              = -1,
} theme_id;

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

typedef enum item_color {
	ICOL_WHITE = PAL16_YELLOW + 5,
	ICOL_BLUE  = PAL16_BLUE + 5,
	ICOL_RED   = PAL16_RED + 5,
} item_color;

typedef enum _difficulty {
	DIFF_NORMAL      = 0x0,
	DIFF_NIGHTMARE   = 0x1,
	DIFF_HELL        = 0x2,
	NUM_DIFFICULTIES = 0x3,
} _difficulty;

typedef enum PLR_MODE {
	PM_STAND   = 0,
	PM_WALK    = 1,
	PM_WALK2   = 2,
	PM_WALK3   = 3,
	PM_ATTACK  = 4,
	PM_RATTACK = 5,
	PM_BLOCK   = 6,
	PM_GOTHIT  = 7,
	PM_DEATH   = 8,
	PM_SPELL   = 9,
	PM_NEWLVL  = 10,
	PM_QUIT    = 11,
} PLR_MODE;

typedef enum spell_type {
	RSPLTYPE_SKILL   = 0x0,
	RSPLTYPE_SPELL   = 0x1,
	RSPLTYPE_SCROLL  = 0x2,
	RSPLTYPE_CHARGES = 0x3,
	RSPLTYPE_INVALID = 0x4,
} spell_type;

typedef enum cursor_id {
	CURSOR_NONE        = 0x0,
	CURSOR_HAND        = 0x1,
	CURSOR_IDENTIFY    = 0x2,
	CURSOR_REPAIR      = 0x3,
	CURSOR_RECHARGE    = 0x4,
	CURSOR_DISARM      = 0x5,
	CURSOR_OIL         = 0x6,
	CURSOR_TELEKINESIS = 0x7,
	CURSOR_RESURRECT   = 0x8,
	CURSOR_TELEPORT    = 0x9,
	CURSOR_HEALOTHER   = 0xA,
	CURSOR_HOURGLASS   = 0xB,
	CURSOR_FIRSTITEM   = 0xC,
} cursor_id;

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

typedef enum spell_id {
	SPL_NULL        = 0x0,
	SPL_FIREBOLT    = 0x1,
	SPL_HEAL        = 0x2,
	SPL_LIGHTNING   = 0x3,
	SPL_FLASH       = 0x4,
	SPL_IDENTIFY    = 0x5,
	SPL_FIREWALL    = 0x6,
	SPL_TOWN        = 0x7,
	SPL_STONE       = 0x8,
	SPL_INFRA       = 0x9,
	SPL_RNDTELEPORT = 0xA,
	SPL_MANASHIELD  = 0xB,
	SPL_FIREBALL    = 0xC,
	SPL_GUARDIAN    = 0xD,
	SPL_CHAIN       = 0xE,
	SPL_WAVE        = 0xF,
	SPL_DOOMSERP    = 0x10,
	SPL_BLODRIT     = 0x11,
	SPL_NOVA        = 0x12,
	SPL_INVISIBIL   = 0x13,
	SPL_FLAME       = 0x14,
	SPL_GOLEM       = 0x15,
	SPL_BLODBOIL    = 0x16,
	SPL_TELEPORT    = 0x17,
	SPL_APOCA       = 0x18,
	SPL_ETHEREALIZE = 0x19,
	SPL_REPAIR      = 0x1A,
	SPL_RECHARGE    = 0x1B,
	SPL_DISARM      = 0x1C,
	SPL_ELEMENT     = 0x1D,
	SPL_CBOLT       = 0x1E,
	SPL_HBOLT       = 0x1F,
	SPL_RESURRECT   = 0x20,
	SPL_TELEKINESIS = 0x21,
	SPL_HEALOTHER   = 0x22,
	SPL_FLARE       = 0x23,
	SPL_BONESPIRIT  = 0x24,
	SPL_INVALID     = -1,
} spell_id;

typedef enum _cmd_id {
	CMD_STAND,
	CMD_WALKXY,
	CMD_ACK_PLRINFO,
	CMD_ADDSTR,
	CMD_ADDMAG,
	CMD_ADDDEX,
	CMD_ADDVIT,
	CMD_SBSPELL,
	CMD_GETITEM,
	CMD_AGETITEM,
	CMD_PUTITEM,
	CMD_RESPAWNITEM,
	CMD_ATTACKXY,
	CMD_RATTACKXY,
	CMD_SPELLXY,
	CMD_TSPELLXY,
	CMD_OPOBJXY,
	CMD_DISARMXY,
	CMD_ATTACKID,
	CMD_ATTACKPID,
	CMD_RATTACKID,
	CMD_RATTACKPID,
	CMD_SPELLID,
	CMD_SPELLPID,
	CMD_TSPELLID,
	CMD_TSPELLPID,
	CMD_RESURRECT,
	CMD_OPOBJT,
	CMD_KNOCKBACK,
	CMD_TALKXY,
	CMD_NEWLVL,
	CMD_WARP,
	CMD_CHEAT_EXPERIENCE,
	CMD_CHEAT_SPELL_LEVEL,
	CMD_DEBUG,
	CMD_SYNCDATA,
	CMD_MONSTDEATH,
	CMD_MONSTDAMAGE,
	CMD_PLRDEAD,
	CMD_REQUESTGITEM,
	CMD_REQUESTAGITEM,
	CMD_GOTOGETITEM,
	CMD_GOTOAGETITEM,
	CMD_OPENDOOR,
	CMD_CLOSEDOOR,
	CMD_OPERATEOBJ,
	CMD_PLROPOBJ,
	CMD_BREAKOBJ,
	CMD_CHANGEPLRITEMS,
	CMD_DELPLRITEMS,
	CMD_PLRDAMAGE,
	CMD_PLRLEVEL,
	CMD_DROPITEM,
	CMD_PLAYER_JOINLEVEL,
	CMD_SEND_PLRINFO,
	CMD_SATTACKXY,
	CMD_ACTIVATEPORTAL,
	CMD_DEACTIVATEPORTAL,
	CMD_DLEVEL_0,
	CMD_DLEVEL_1,
	CMD_DLEVEL_2,
	CMD_DLEVEL_3,
	CMD_DLEVEL_4,
	CMD_DLEVEL_5,
	CMD_DLEVEL_6,
	CMD_DLEVEL_7,
	CMD_DLEVEL_8,
	CMD_DLEVEL_9,
	CMD_DLEVEL_10,
	CMD_DLEVEL_11,
	CMD_DLEVEL_12,
	CMD_DLEVEL_13,
	CMD_DLEVEL_14,
	CMD_DLEVEL_15,
	CMD_DLEVEL_16,
	CMD_DLEVEL_JUNK,
	CMD_DLEVEL_END,
	CMD_HEALOTHER,
	CMD_STRING,
	CMD_SETSTR,
	CMD_SETMAG,
	CMD_SETDEX,
	CMD_SETVIT,
	CMD_RETOWN,
	CMD_SPELLXYD,
	CMD_ITEMEXTRA,
	CMD_SYNCPUTITEM,
	CMD_KILLGOLEM,
	CMD_SYNCQUEST,
	CMD_ENDSHIELD,
	CMD_AWAKEGOLEM,
	CMD_NOVA,
	CMD_SETSHIELD,
	CMD_REMSHIELD,
	FAKE_CMD_SETID,
	FAKE_CMD_DROPID,
	NUM_CMDS,
} _cmd_id;

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

typedef enum attribute_id {
	ATTRIB_STR = 0,
	ATTRIB_MAG = 1,
	ATTRIB_DEX = 2,
	ATTRIB_VIT = 3,
} attribute_id;

enum class SetLvl {
	None		 = 0,
	//SL_BUTCHCHAMB = 0x0, 
	SkelKing     = 0x1,
	BoneChamb    = 0x2,
	Maze         = 0x3,
	PoisonWater  = 0x4,
	VileBetrayer = 0x5,
};

typedef enum quest_id {
	Q_ROCK     = 0x0,
	Q_MUSHROOM = 0x1,
	Q_GARBUD   = 0x2,
	Q_ZHAR     = 0x3,
	Q_VEIL     = 0x4,
	Q_DIABLO   = 0x5,
	Q_BUTCHER  = 0x6,
	Q_LTBANNER = 0x7,
	Q_BLIND    = 0x8,
	Q_BLOOD    = 0x9,
	Q_ANVIL    = 0xA,
	Q_WARLORD  = 0xB,
	Q_SKELKING = 0xC,
	Q_PWATER   = 0xD,
	Q_SCHAMB   = 0xE,
	Q_BETRAYER = 0xF,
	Q_INVALID  = -1,
} quest_id;

typedef enum quest_state {
	QUEST_NOTAVAIL = 0,
	QUEST_INIT     = 1,
	QUEST_ACTIVE   = 2,
	QUEST_DONE     = 3
} quest_state;

typedef enum quest_mush_state {
	QS_INIT         = 0,
	QS_TOMESPAWNED  = 1,
	QS_TOMEGIVEN    = 2,
	QS_MUSHSPAWNED  = 3,
	QS_MUSHPICKED   = 4,
	QS_MUSHGIVEN    = 5,
	QS_BRAINSPAWNED = 6,
	QS_BRAINGIVEN   = 7,
} quest_mush_state;

typedef enum plr_class {
	PC_WARRIOR  = 0x0,
	PC_ROGUE    = 0x1,
	PC_SORCERER = 0x2,
	NUM_CLASSES
} plr_class;

typedef enum _ui_classes {
	UI_WARRIOR  = 0x0,
	UI_ROGUE    = 0x1,
	UI_SORCERER = 0x2,
	UI_NUM_CLASSES,
} _ui_classes;

//typedef enum _walk_path {
//	WALK_NE   = 0x1,
//	WALK_NW   = 0x2,
//	WALK_SE   = 0x3,
//	WALK_SW   = 0x4,
//	WALK_N    = 0x5,
//	WALK_E    = 0x6,
//	WALK_S    = 0x7,
//	WALK_W    = 0x8,
////	WALK_NONE = -1,
//} _walk_path;

typedef enum player_weapon_type {
	WT_MELEE  = 0,
	WT_RANGED = 1,
} player_weapon_type;


typedef enum player_graphic {
	PFILE_STAND     = 1 << 0,
	PFILE_WALK      = 1 << 1,
	PFILE_ATTACK    = 1 << 2,
	PFILE_HIT       = 1 << 3,
	PFILE_LIGHTNING = 1 << 4,
	PFILE_FIRE      = 1 << 5,
	PFILE_MAGIC     = 1 << 6,
	PFILE_DEATH     = 1 << 7,
	PFILE_BLOCK     = 1 << 8,
	// everything except PFILE_DEATH
	// 0b1_0111_1111
	PFILE_NONDEATH = 0x17F
} player_graphic;

typedef enum shrine_type {
	SHRINE_MYSTERIOUS   = 0,
	SHRINE_HIDDEN       = 1,
	SHRINE_GLOOMY       = 2,
	SHRINE_WEIRD        = 3,
	SHRINE_MAGICAL      = 4,
	SHRINE_STONE        = 5,
	SHRINE_RELIGIOUS    = 6,
	SHRINE_ENCHANTED    = 7,
	SHRINE_THAUMATURGIC = 8,
	SHRINE_FASCINATING  = 9,
	SHRINE_CRYPTIC      = 10,
	SHRINE_MAGICAL2     = 11,
	SHRINE_ELDRITCH     = 12,
	SHRINE_EERIE        = 13,
	SHRINE_DIVINE       = 14,
	SHRINE_HOLY         = 15,
	SHRINE_SACRED       = 16,
	SHRINE_SPIRITUAL    = 17,
	SHRINE_SPOOKY       = 18,
	SHRINE_ABANDONED    = 19,
	SHRINE_CREEPY       = 20,
	SHRINE_QUIET        = 21,
	SHRINE_SECLUDED     = 22,
	SHRINE_ORNATE       = 23,
	SHRINE_GLIMMERING   = 24,
	SHRINE_TAINTED      = 25,
	NUM_SHRINETYPE
} shrine_type;

typedef enum action_id {
	ACTION_WALK        = -2, // Automatic walk when using gamepad
	ACTION_NONE        = -1,
	ACTION_ATTACK      = 9,
	ACTION_RATTACK     = 10,
	ACTION_SPELL       = 12,
	ACTION_OPERATE     = 13,
	ACTION_DISARM      = 14,
	ACTION_PICKUPITEM  = 15, // put item in hand (inventory screen open)
	ACTION_PICKUPAITEM = 16, // put item in inventory
	ACTION_TALK        = 17,
	ACTION_OPERATETK   = 18, // operate via telekinesis
	ACTION_ATTACKMON   = 20,
	ACTION_ATTACKPLR   = 21,
	ACTION_RATTACKMON  = 22,
	ACTION_RATTACKPLR  = 23,
	ACTION_SPELLMON    = 24,
	ACTION_SPELLPLR    = 25,
	ACTION_SPELLWALL   = 26,
} action_id;

typedef enum dlrg_flag {
	DLRG_HDOOR     = 0x01,
	DLRG_VDOOR     = 0x02,
	DLRG_CHAMBER   = 0x40,
	DLRG_PROTECTED = 0x80,
} dlrg_flag;

typedef enum conn_type {
#ifndef NONET
	SELCONN_TCP,
#ifdef BUGGY
	SELCONN_UDP,
#endif
#endif
	SELCONN_LOOPBACK,
} conn_type;

DEVILUTION_END_NAMESPACE

#endif /* __ENUMS_H__ */
