/**
 * @file monstdat.cpp
 *
 * Implementation of all monster data.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

MonsterData monsterdata[] = {
	// clang-format off
	// width, mImage, GraphicType,                       has_special, sndfile,                             snd_special, has_trans, TransFile,                         Frames[6],                  Rate[6],              mName,               mMinDLvl, mMaxDLvl, mLevel, mMinHP, mMaxHP, mAi,         mFlags,                                                              mInt, mHit, mAFNum, mMinDamage, mMaxDamage, mHit2, mAFNum2, mMinDamage2, mMaxDamage2, mArmorClass, mMonstClass, mMagicRes                                                    , mMagicRes2                                                   , mTreasure, mSelFlag,  mExp
	{    128,    799, "Monsters\\Zombie\\Zombie%c.CL2",  false,       "Monsters\\Zombie\\Zombie%c%i.WAV",  false,       false,     NULL,                              { 11, 24, 12,  6, 16,  0 }, { 4, 0, 0, 0, 0, 0 }, "Zombie",                   1,        3,      1,      4,      7, AI_ZOMBIE,   0                                                                  ,    0,   10,      8,          2,          5,     0,       0,           0,           0,           5, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,    54 },
	{    128,    799, "Monsters\\Zombie\\Zombie%c.CL2",  false,       "Monsters\\Zombie\\Zombie%c%i.WAV",  false,       true,      "Monsters\\Zombie\\Bluered.TRN",   { 11, 24, 12,  6, 16,  0 }, { 4, 0, 0, 0, 0, 0 }, "Ghoul",                    2,        4,      2,      7,     11, AI_ZOMBIE,   0                                                                  ,    1,   10,      8,          3,         10,     0,       0,           0,           0,          10, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,    58 },
	{    128,    799, "Monsters\\Zombie\\Zombie%c.CL2",  false,       "Monsters\\Zombie\\Zombie%c%i.WAV",  false,       true,      "Monsters\\Zombie\\Grey.TRN",      { 11, 24, 12,  6, 16,  0 }, { 4, 0, 0, 0, 0, 0 }, "Rotting Carcass",          2,        6,      4,     15,     25, AI_ZOMBIE,   0                                                                  ,    2,   25,      8,          5,         15,     0,       0,           0,           0,          15, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,         0,        3,   136 },
	{    128,    799, "Monsters\\Zombie\\Zombie%c.CL2",  false,       "Monsters\\Zombie\\Zombie%c%i.WAV",  false,       true,      "Monsters\\Zombie\\Yellow.TRN",    { 11, 24, 12,  6, 16,  0 }, { 4, 0, 0, 0, 0, 0 }, "Black Death",              4,        8,      6,     25,     40, AI_ZOMBIE,   0                                                                  ,    3,   30,      8,          6,         22,     0,       0,           0,           0,          20, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   240 },
	{    128,    543, "Monsters\\FalSpear\\Phall%c.CL2", true,        "Monsters\\FalSpear\\Phall%c%i.WAV", true,        true,      "Monsters\\FalSpear\\FallenT.TRN", { 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Fallen One",               1,        3,      1,      1,      4, AI_FALLEN,   0                                                                  ,    0,   15,      7,          1,          3,     0,       5,           0,           0,           0, MC_ANIMAL,   0                                                            , 0                                                            ,         0,        3,    46 },
	{    128,    543, "Monsters\\FalSpear\\Phall%c.CL2", true,        "Monsters\\FalSpear\\Phall%c%i.WAV", true,        true,      "Monsters\\FalSpear\\Dark.TRN",    { 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Carver",                   2,        5,      3,      4,      8, AI_FALLEN,   0                                                                  ,    2,   20,      7,          2,          5,     0,       5,           0,           0,           5, MC_ANIMAL,   0                                                            , 0                                                            ,         0,        3,    80 },
	{    128,    543, "Monsters\\FalSpear\\Phall%c.CL2", true,        "Monsters\\FalSpear\\Phall%c%i.WAV", true,        false,     NULL,                              { 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Devil Kin",                3,        7,      5,     12,     24, AI_FALLEN,   0                                                                  ,    2,   25,      7,          3,          7,     0,       5,           0,           0,          10, MC_ANIMAL,   0                                                            ,                RESIST_FIRE                                   ,         0,        3,   155 },
	{    128,    543, "Monsters\\FalSpear\\Phall%c.CL2", true,        "Monsters\\FalSpear\\Phall%c%i.WAV", true,        true,      "Monsters\\FalSpear\\Blue.TRN",    { 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Dark One",                 5,        9,      7,     20,     36, AI_FALLEN,   0                                                                  ,    3,   30,      7,          4,          8,     0,       5,           0,           0,          15, MC_ANIMAL,                                                   IMUNE_NULL_40,                              RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   255 },
	{    128,    553, "Monsters\\SkelAxe\\SklAx%c.CL2",  true,        "Monsters\\SkelAxe\\SklAx%c%i.WAV",  false,       true,      "Monsters\\SkelAxe\\White.TRN",    { 12,  8, 13,  6, 17, 16 }, { 5, 0, 0, 0, 0, 0 }, "Skeleton",                 1,        3,      1,      2,      4, AI_SKELSD,   0                                                                  ,    0,   20,      8,          1,          4,     0,       0,           0,           0,           0, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,    64 },
	{    128,    553, "Monsters\\SkelAxe\\SklAx%c.CL2",  true,        "Monsters\\SkelAxe\\SklAx%c%i.WAV",  false,       true,      "Monsters\\SkelAxe\\Skelt.TRN",    { 12,  8, 13,  6, 17, 16 }, { 4, 0, 0, 0, 0, 0 }, "Corpse Axe",               2,        5,      2,      4,      7, AI_SKELSD,   0                                                                  ,    1,   25,      8,          3,          5,     0,       0,           0,           0,           0, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,    68 },
	{    128,    553, "Monsters\\SkelAxe\\SklAx%c.CL2",  true,        "Monsters\\SkelAxe\\SklAx%c%i.WAV",  false,       false,     NULL,                              { 12,  8, 13,  6, 17, 16 }, { 2, 0, 0, 0, 0, 0 }, "Burning Dead",             2,        6,      4,      8,     12, AI_SKELSD,   0                                                                  ,    2,   30,      8,          3,          7,     0,       0,           0,           0,           5, MC_UNDEAD,   IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,         0,        3,   154 },
	{    128,    553, "Monsters\\SkelAxe\\SklAx%c.CL2",  true,        "Monsters\\SkelAxe\\SklAx%c%i.WAV",  false,       true,      "Monsters\\SkelAxe\\Black.TRN",    { 12,  8, 13,  6, 17, 16 }, { 3, 0, 0, 0, 0, 0 }, "Horror",                   4,        8,      6,     12,     20, AI_SKELSD,   0                                                                  ,    3,   35,      8,          4,          9,     0,       0,           0,           0,          15, MC_UNDEAD,   IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   264 },
	{    128,    623, "Monsters\\FalSword\\Fall%c.CL2",  true,        "Monsters\\FalSword\\Fall%c%i.WAV",  true,        true,      "Monsters\\FalSword\\FallenT.TRN", { 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Fallen One",               1,        3,      1,      2,      5, AI_FALLEN,   0                                                                  ,    0,   15,      8,          1,          4,     0,       5,           0,           0,          10, MC_ANIMAL,   0                                                            , 0                                                            ,         0,        3,    52 },
	{    128,    623, "Monsters\\FalSword\\Fall%c.CL2",  true,        "Monsters\\FalSword\\Fall%c%i.WAV",  true,        true,      "Monsters\\FalSword\\Dark.TRN",    { 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Carver",                   2,        5,      3,      5,      9, AI_FALLEN,   0                                                                  ,    1,   20,      8,          2,          7,     0,       5,           0,           0,          15, MC_ANIMAL,   0                                                            , 0                                                            ,         0,        3,    90 },
	{    128,    623, "Monsters\\FalSword\\Fall%c.CL2",  true,        "Monsters\\FalSword\\Fall%c%i.WAV",  true,        false,     NULL,                              { 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Devil Kin",                3,        7,      5,     16,     24, AI_FALLEN,   0                                                                  ,    2,   25,      8,          4,         10,     0,       5,           0,           0,          20, MC_ANIMAL,   0                                                            ,                RESIST_FIRE                                   ,         0,        3,   180 },
	{    128,    623, "Monsters\\FalSword\\Fall%c.CL2",  true,        "Monsters\\FalSword\\Fall%c%i.WAV",  true,        true,      "Monsters\\FalSword\\Blue.TRN",    { 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Dark One",                 5,        9,      7,     24,     36, AI_FALLEN,   0                                                                  ,    3,   30,      8,          4,         12,     0,       5,           0,           0,          25, MC_ANIMAL,                                                   IMUNE_NULL_40,                              RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   280 },
	{    128,    410, "Monsters\\Scav\\Scav%c.CL2",      true,        "Monsters\\Scav\\Scav%c%i.WAV",      false,       false,     NULL,                              { 12,  8, 12,  6, 20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Scavenger",                1,        4,      2,      3,      6, AI_SCAV,     0                                                                  ,    0,   20,      7,          1,          5,     0,       0,           0,           0,          10, MC_ANIMAL,   0                                                            ,                RESIST_FIRE                                   ,         0,        3,    80 },
	{    128,    410, "Monsters\\Scav\\Scav%c.CL2",      true,        "Monsters\\Scav\\Scav%c%i.WAV",      false,       true,      "Monsters\\Scav\\ScavBr.TRN",      { 12,  8, 12,  6, 20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Plague Eater",             3,        6,      4,     12,     24, AI_SCAV,     0                                                                  ,    1,   30,      7,          1,          8,     0,       0,           0,           0,          20, MC_ANIMAL,   0                                                            ,                              RESIST_LIGHTNING                ,         0,        3,   188 },
	{    128,    410, "Monsters\\Scav\\Scav%c.CL2",      true,        "Monsters\\Scav\\Scav%c%i.WAV",      false,       true,      "Monsters\\Scav\\ScavBe.TRN",      { 12,  8, 12,  6, 20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Shadow Beast",             4,        8,      6,     24,     36, AI_SCAV,     0                                                                  ,    2,   35,      7,          3,         12,     0,       0,           0,           0,          25, MC_ANIMAL,                                                   IMUNE_NULL_40,                RESIST_FIRE |                    IMUNE_NULL_40,         0,        3,   375 },
	{    128,    410, "Monsters\\Scav\\Scav%c.CL2",      true,        "Monsters\\Scav\\Scav%c%i.WAV",      false,       true,      "Monsters\\Scav\\ScavW.TRN",       { 12,  8, 12,  6, 20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Bone Gasher",              6,       10,      8,     28,     40, AI_SCAV,     0                                                                  ,    3,   35,      7,          5,         15,     0,       0,           0,           0,          30, MC_ANIMAL,   RESIST_MAGIC |                                  IMUNE_NULL_40,                              RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   552 },
	{    128,    567, "Monsters\\SkelBow\\SklBw%c.CL2",  true,        "Monsters\\SkelBow\\SklBw%c%i.WAV",  false,       true,      "Monsters\\SkelBow\\White.TRN",    {  9,  8, 16,  5, 16, 16 }, { 4, 0, 0, 0, 0, 0 }, "Skeleton",                 2,        5,      3,      2,      4, AI_SKELBOW,  0                                                                  ,    0,   15,     12,          1,          2,     0,       0,           0,           0,           0, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   110 },
	{    128,    567, "Monsters\\SkelBow\\SklBw%c.CL2",  true,        "Monsters\\SkelBow\\SklBw%c%i.WAV",  false,       true,      "Monsters\\SkelBow\\Skelt.TRN",    {  9,  8, 16,  5, 16, 16 }, { 4, 0, 0, 0, 0, 0 }, "Corpse Bow",               3,        7,      5,      8,     16, AI_SKELBOW,  0                                                                  ,    1,   25,     12,          1,          4,     0,       0,           0,           0,           0, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   210 },
	{    128,    567, "Monsters\\SkelBow\\SklBw%c.CL2",  true,        "Monsters\\SkelBow\\SklBw%c%i.WAV",  false,       false,     NULL,                              {  9,  8, 16,  5, 16, 16 }, { 2, 0, 0, 0, 0, 0 }, "Burning Dead",             5,        9,      7,     10,     24, AI_SKELBOW,  0                                                                  ,    2,   30,     12,          1,          6,     0,       0,           0,           0,           5, MC_UNDEAD,   IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,         0,        3,   364 },
	{    128,    567, "Monsters\\SkelBow\\SklBw%c.CL2",  true,        "Monsters\\SkelBow\\SklBw%c%i.WAV",  false,       true,      "Monsters\\SkelBow\\Black.TRN",    {  9,  8, 16,  5, 16, 16 }, { 3, 0, 0, 0, 0, 0 }, "Horror",                   7,       11,      9,     15,     45, AI_SKELBOW,  0                                                                  ,    3,   35,     12,          2,          9,     0,       0,           0,           0,          15, MC_UNDEAD,   IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   594 },
	{    128,    575, "Monsters\\SkelSd\\SklSr%c.CL2",   true,        "Monsters\\SkelSd\\SklSr%c%i.WAV",   true,        true,      "Monsters\\SkelSd\\White.TRN",     { 13,  8, 12,  7, 15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Skeleton Captain",         1,        4,      2,      3,      6, AI_SKELSD,   0                                                                  ,    0,   20,      8,          2,          7,     0,       0,           0,           0,          10, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,    90 },
	{    128,    575, "Monsters\\SkelSd\\SklSr%c.CL2",   true,        "Monsters\\SkelSd\\SklSr%c%i.WAV",   false,       true,      "Monsters\\SkelSd\\Skelt.TRN",     { 13,  8, 12,  7, 15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Corpse Captain",           2,        6,      4,     12,     20, AI_SKELSD,   0                                                                  ,    1,   30,      8,          3,          9,     0,       0,           0,           0,           5, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   200 },
	{    128,    575, "Monsters\\SkelSd\\SklSr%c.CL2",   true,        "Monsters\\SkelSd\\SklSr%c%i.WAV",   false,       false,     NULL,                              { 13,  8, 12,  7, 15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Burning Dead Captain",     4,        8,      6,     16,     30, AI_SKELSD,   0                                                                  ,    2,   35,      8,          4,         10,     0,       0,           0,           0,          15, MC_UNDEAD,   IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,         0,        3,   393 },
	{    128,    575, "Monsters\\SkelSd\\SklSr%c.CL2",   true,        "Monsters\\SkelSd\\SklSr%c%i.WAV",   false,       true,      "Monsters\\SkelSd\\Black.TRN",     { 13,  8, 12,  7, 15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Horror Captain",           6,       10,      8,     35,     50, AI_SKELSD,                                    MonsterFlag::search                      ,    3,   40,      8,          5,         14,     0,       0,           0,           0,          30, MC_UNDEAD,   IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   604 },
	{    128,   2000, "Monsters\\TSneak\\TSneak%c.CL2",  false,       "Monsters\\TSneak\\Sneakl%c%i.WAV",  false,       false,     NULL,                              { 13, 13, 15, 11, 16,  0 }, { 2, 0, 0, 0, 0, 0 }, "Invisible Lord",          14,       14,     14,    278,    278, AI_SKELSD,                                    MonsterFlag::search                      ,    3,   65,      8,         16,         30,     0,       0,           0,           0,          60, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40, RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,  2000 },
	{    128,    992, "Monsters\\Sneak\\Sneak%c.CL2",    true,        "Monsters\\Sneak\\Sneak%c%i.WAV",    false,       false,     NULL,                              { 16,  8, 12,  8, 24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Hidden",                   3,        8,      5,      8,     24, AI_SNEAK,    MonsterFlag::hidden                                                       ,    0,   35,      8,          3,          6,     0,       0,           0,           0,          25, MC_DEMON,    0                                                            ,                                                 IMUNE_NULL_40,         0,        3,   278 },
	{    128,    992, "Monsters\\Sneak\\Sneak%c.CL2",    true,        "Monsters\\Sneak\\Sneak%c%i.WAV",    false,       true,      "Monsters\\Sneak\\Sneakv2.TRN",    { 16,  8, 12,  8, 24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Stalker",                  8,       12,      9,     30,     45, AI_SNEAK,    MonsterFlag::hidden |                   MonsterFlag::search                      ,    1,   40,      8,          8,         16,     0,       0,           0,           0,          30, MC_DEMON,    0                                                            ,                                                 IMUNE_NULL_40,         0,        3,   630 },
	{    128,    992, "Monsters\\Sneak\\Sneak%c.CL2",    true,        "Monsters\\Sneak\\Sneak%c%i.WAV",    false,       true,      "Monsters\\Sneak\\Sneakv3.TRN",    { 16,  8, 12,  8, 24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Unseen",                  10,       14,     11,     35,     50, AI_SNEAK,    MonsterFlag::hidden |                   MonsterFlag::search                      ,    2,   45,      8,         12,         20,     0,       0,           0,           0,          30, MC_DEMON,    RESIST_MAGIC |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   935 },
	{    128,    992, "Monsters\\Sneak\\Sneak%c.CL2",    true,        "Monsters\\Sneak\\Sneak%c%i.WAV",    false,       true,      "Monsters\\Sneak\\Sneakv1.TRN",    { 16,  8, 12,  8, 24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Illusion Weaver",         14,       18,     13,     40,     60, AI_SNEAK,    MonsterFlag::hidden |                   MonsterFlag::search                      ,    3,   60,      8,         16,         24,     0,       0,           0,           0,          30, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE                                   , IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,         0,        3,  1500 },
	{    160,   2000, "Monsters\\GoatLord\\GoatL%c.CL2", false,       "Monsters\\GoatLord\\Goatl%c%i.WAV", false,       false,     NULL,                              { 13, 13, 14,  9, 16,  0 }, { 2, 0, 0, 0, 0, 0 }, "Lord Sayter",             13,       13,     12,    351,    351, AI_SKELSD,                                    MonsterFlag::search                      ,    3,   80,      8,         14,         24,     0,       0,           0,           0,          60, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE |                    IMUNE_NULL_40, RESIST_MAGIC | RESIST_FIRE |                    IMUNE_NULL_40,         0,        3,  1500 },
	{    128,   1030, "Monsters\\GoatMace\\Goat%c.CL2",  true,        "Monsters\\GoatMace\\Goat%c%i.WAV",  false,       false,     NULL,                              { 12,  8, 12,  6, 20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Flesh Clan",               6,       10,      8,     30,     45, AI_GOATMC,                                    MonsterFlag::search | MonsterFlag::can_open_door,    0,   50,      8,          4,         10,     0,       0,           0,           0,          40, MC_DEMON,    0                                                            , 0                                                            ,         0,        3,   460 },
	{    128,   1030, "Monsters\\GoatMace\\Goat%c.CL2",  true,        "Monsters\\GoatMace\\Goat%c%i.WAV",  false,       true,      "Monsters\\GoatMace\\Beige.TRN",   { 12,  8, 12,  6, 20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Stone Clan",               8,       12,     10,     40,     55, AI_GOATMC,                                    MonsterFlag::search | MonsterFlag::can_open_door,    1,   60,      8,          6,         12,     0,       0,           0,           0,          40, MC_DEMON,    RESIST_MAGIC |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   685 },
	{    128,   1030, "Monsters\\GoatMace\\Goat%c.CL2",  true,        "Monsters\\GoatMace\\Goat%c%i.WAV",  false,       true,      "Monsters\\GoatMace\\Red.TRN",     { 12,  8, 12,  6, 20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Fire Clan",               10,       14,     12,     50,     65, AI_GOATMC,                                    MonsterFlag::search | MonsterFlag::can_open_door,    2,   70,      8,          8,         16,     0,       0,           0,           0,          45, MC_DEMON,                   RESIST_FIRE                                   ,                IMUNE_FIRE                                    ,         0,        3,   906 },
	{    128,   1030, "Monsters\\GoatMace\\Goat%c.CL2",  true,        "Monsters\\GoatMace\\Goat%c%i.WAV",  false,       true,      "Monsters\\GoatMace\\Gray.TRN",    { 12,  8, 12,  6, 20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Night Clan",              12,       16,     14,     55,     70, AI_GOATMC,                                    MonsterFlag::search | MonsterFlag::can_open_door,    3,   80,      8,         10,         20,    15,       0,          30,          30,          50, MC_DEMON,    RESIST_MAGIC |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,  1190 },
	{     96,    364, "Monsters\\Bat\\Bat%c.CL2",        false,       "Monsters\\Bat\\Bat%c%i.WAV",        false,       true,      "Monsters\\Bat\\red.trn",          {  9, 13, 10,  9, 13,  0 }, { 0, 0, 0, 0, 0, 0 }, "Fiend",                    2,        5,      3,      3,      6, AI_BAT,      0                                                                  ,    0,   35,      5,          1,          6,     0,       0,           0,           0,           0, MC_ANIMAL,   0                                                            , 0                                                            ,    0x4000,        6,   102 },
	{     96,    364, "Monsters\\Bat\\Bat%c.CL2",        false,       "Monsters\\Bat\\Bat%c%i.WAV",        false,       false,     NULL,                              {  9, 13, 10,  9, 13,  0 }, { 0, 0, 0, 0, 0, 0 }, "Blink",                    5,        9,      7,     12,     28, AI_BAT,      0                                                                  ,    1,   45,      5,          1,          8,     0,       0,           0,           0,          15, MC_ANIMAL,   0                                                            , 0                                                            ,    0x4000,        6,   340 },
	{     96,    364, "Monsters\\Bat\\Bat%c.CL2",        false,       "Monsters\\Bat\\Bat%c%i.WAV",        false,       true,      "Monsters\\Bat\\grey.trn",         {  9, 13, 10,  9, 13,  0 }, { 0, 0, 0, 0, 0, 0 }, "Gloom",                    7,       11,      9,     28,     36, AI_BAT,                                       MonsterFlag::search                      ,    2,   70,      5,          4,         12,     0,       0,           0,           0,          35, MC_ANIMAL,   RESIST_MAGIC                                                 , RESIST_MAGIC |                                  IMUNE_NULL_40,    0x4000,        6,   509 },
	{     96,    364, "Monsters\\Bat\\Bat%c.CL2",        false,       "Monsters\\Bat\\Bat%c%i.WAV",        false,       true,      "Monsters\\Bat\\orange.trn",       {  9, 13, 10,  9, 13,  0 }, { 0, 0, 0, 0, 0, 0 }, "Familiar",                11,       15,     13,     20,     35, AI_BAT,                                       MonsterFlag::search                      ,    3,   50,      5,          4,         16,     0,       0,           0,           0,          35, MC_DEMON,    RESIST_MAGIC |               IMUNE_LIGHTNING                 , RESIST_MAGIC |               IMUNE_LIGHTNING  | IMUNE_NULL_40,    0x4000,        6,   448 },
	{    128,   1040, "Monsters\\GoatBow\\GoatB%c.CL2",  false,       "Monsters\\GoatBow\\GoatB%c%i.WAV",  false,       false,     NULL,                              { 12,  8, 16,  6, 20,  0 }, { 3, 0, 0, 0, 0, 0 }, "Flesh Clan",               6,       10,      8,     20,     35, AI_GOATBOW,                                                  MonsterFlag::can_open_door,    0,   35,     13,          1,          7,     0,       0,           0,           0,          35, MC_DEMON,    0                                                            , 0                                                            ,         0,        3,   448 },
	{    128,   1040, "Monsters\\GoatBow\\GoatB%c.CL2",  false,       "Monsters\\GoatBow\\GoatB%c%i.WAV",  false,       true,      "Monsters\\GoatBow\\Beige.TRN",    { 12,  8, 16,  6, 20,  0 }, { 3, 0, 0, 0, 0, 0 }, "Stone Clan",               8,       12,     10,     30,     40, AI_GOATBOW,                                                  MonsterFlag::can_open_door,    1,   40,     13,          2,          9,     0,       0,           0,           0,          35, MC_DEMON,    RESIST_MAGIC |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   645 },
	{    128,   1040, "Monsters\\GoatBow\\GoatB%c.CL2",  false,       "Monsters\\GoatBow\\GoatB%c%i.WAV",  false,       true,      "Monsters\\GoatBow\\Red.TRN",      { 12,  8, 16,  6, 20,  0 }, { 3, 0, 0, 0, 0, 0 }, "Fire Clan",               10,       14,     12,     40,     50, AI_GOATBOW,                                   MonsterFlag::search | MonsterFlag::can_open_door,    2,   45,     13,          3,         11,     0,       0,           0,           0,          35, MC_DEMON,                   RESIST_FIRE                                   ,                IMUNE_FIRE                                    ,         0,        3,   822 },
	{    128,   1040, "Monsters\\GoatBow\\GoatB%c.CL2",  false,       "Monsters\\GoatBow\\GoatB%c%i.WAV",  false,       true,      "Monsters\\GoatBow\\Gray.TRN",     { 12,  8, 16,  6, 20,  0 }, { 3, 0, 0, 0, 0, 0 }, "Night Clan",              12,       16,     14,     50,     65, AI_GOATBOW,                                   MonsterFlag::search | MonsterFlag::can_open_door,    3,   50,     13,          4,         13,    15,       0,           0,           0,          40, MC_DEMON,    RESIST_MAGIC |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,  1092 },
	{    128,    716, "Monsters\\Acid\\Acid%c.CL2",      true,        "Monsters\\Acid\\Acid%c%i.WAV",      true,        false,     NULL,                              { 13,  8, 12,  8, 16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Acid Beast",              10,       14,     11,     40,     66, AI_ACID,     0                                                                  ,    0,   40,      8,          4,         12,    25,       8,           0,           0,          30, MC_ANIMAL,                                                   IMUNE_ACID   , IMUNE_MAGIC  |                                  IMUNE_ACID   ,         0,        3,   846 },
	{    128,    716, "Monsters\\Acid\\Acid%c.CL2",      true,        "Monsters\\Acid\\Acid%c%i.WAV",      true,        true,      "Monsters\\Acid\\AcidBlk.TRN",     { 13,  8, 12,  8, 16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Poison Spitter",          14,       18,     15,     60,     85, AI_ACID,     0                                                                  ,    1,   45,      8,          4,         16,    25,       8,           0,           0,          30, MC_ANIMAL,                                                   IMUNE_ACID   , IMUNE_MAGIC  |                                  IMUNE_ACID   ,         0,        3,  1248 },
	{    128,    716, "Monsters\\Acid\\Acid%c.CL2",      true,        "Monsters\\Acid\\Acid%c%i.WAV",      true,        true,      "Monsters\\Acid\\AcidB.TRN",       { 13,  8, 12,  8, 16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Pit Beast",               18,       22,     21,     80,    110, AI_ACID,     0                                                                  ,    2,   55,      8,          8,         18,    35,       8,           0,           0,          35, MC_ANIMAL,   RESIST_MAGIC |                                  IMUNE_ACID   , IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_ACID   ,         0,        3,  2060 },
	{    128,    716, "Monsters\\Acid\\Acid%c.CL2",      true,        "Monsters\\Acid\\Acid%c%i.WAV",      true,        true,      "Monsters\\Acid\\AcidR.TRN",       { 13,  8, 12,  8, 16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Lava Maw",                22,       27,     25,    100,    150, AI_ACID,     0                                                                  ,    3,   65,      8,         10,         20,    40,       8,           0,           0,          35, MC_ANIMAL,   RESIST_MAGIC | IMUNE_FIRE  |                    IMUNE_ACID   , IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_ACID   ,         0,        3,  2940 },
	{    160,   1010, "Monsters\\SKing\\SKing%c.CL2",    true,        "Monsters\\SKing\\SKing%c%i.WAV",    true,        true,      "Monsters\\SkelAxe\\White.TRN",    {  8,  6, 16,  6, 16,  6 }, { 2, 0, 0, 0, 0, 2 }, "Skeleton King",            6,        6,      9,    140,    140, AI_SKELKING,                                  MonsterFlag::search | MonsterFlag::can_open_door,    3,   60,      8,          6,         16,     0,       0,           0,           0,          70, MC_UNDEAD,   IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,    0x8001,        7,   570 },
	{    128,    980, "Monsters\\FatC\\FatC%c.CL2",      false,       "Monsters\\FatC\\FatC%c%i.WAV",      false,       false,     NULL,                              { 10,  8, 12,  6, 16,  0 }, { 1, 0, 0, 0, 0, 0 }, "The Butcher",              0,        0,      1,    320,    320, AI_CLEAVER,  0                                                                  ,    3,   50,      8,          6,         12,     0,       0,           0,           0,          50, MC_DEMON,                   RESIST_FIRE | RESIST_LIGHTNING                , RESIST_MAGIC | IMUNE_FIRE  | IMUNE_LIGHTNING                 ,    0x8000,        3,   710 },
	{    128,   1130, "Monsters\\Fat\\Fat%c.CL2",        true,        "Monsters\\Fat\\Fat%c%i.WAV",        false,       false,     NULL,                              {  8, 10, 15,  6, 16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Overlord",                 8,       12,     10,     60,     80, AI_FAT,      0                                                                  ,    0,   55,      8,          6,         12,     0,       0,           0,           0,          55, MC_DEMON,    0                                                            ,                RESIST_FIRE                                   ,         0,        3,   635 },
	{    128,   1130, "Monsters\\Fat\\Fat%c.CL2",        true,        "Monsters\\Fat\\Fat%c%i.WAV",        false,       true,      "Monsters\\Fat\\Blue.TRN",         {  8, 10, 15,  6, 16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Mud Man",                 13,       17,     14,    100,    125, AI_FAT,                                       MonsterFlag::search                      ,    1,   60,      8,          8,         16,     0,       0,           0,           0,          60, MC_DEMON,    0                                                            ,                              IMUNE_LIGHTNING                 ,         0,        3,  1165 },
	{    128,   1130, "Monsters\\Fat\\Fat%c.CL2",        true,        "Monsters\\Fat\\Fat%c%i.WAV",        false,       true,      "Monsters\\Fat\\FatB.TRN",         {  8, 10, 15,  6, 16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Toad Demon",              15,       19,     16,    135,    160, AI_FAT,                                       MonsterFlag::search                      ,    2,   70,      8,          8,         16,    40,       0,           8,          20,          65, MC_DEMON,    IMUNE_MAGIC                                                  , IMUNE_MAGIC  |               RESIST_LIGHTNING                ,         0,        3,  1380 },
	{    128,   1130, "Monsters\\Fat\\Fat%c.CL2",        true,        "Monsters\\Fat\\Fat%c%i.WAV",        false,       true,      "Monsters\\Fat\\FatF.TRN",         {  8, 10, 15,  6, 16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Flayed One",              19,       23,     20,    160,    200, AI_FAT,                                       MonsterFlag::search                      ,    3,   85,      8,         10,         20,     0,       0,           0,           0,          70, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        3,  2058 },
	{    160,   2420, "Monsters\\Worm\\Worm%c.CL2",      false,       "Monsters\\Fat\\Fat%c%i.WAV",        false,       false,     NULL,                              { 13, 13, 13, 11, 19,  0 }, { 0, 0, 0, 0, 0, 0 }, "Wyrm",                     9,       13,     11,     60,     90, AI_SKELSD,   0                                                                  ,    0,   40,      8,          4,         10,     0,       0,           0,           0,          25, MC_ANIMAL,   RESIST_MAGIC                                                 , RESIST_MAGIC                                                 ,         0,        3,   660 },
	{    160,   2420, "Monsters\\Worm\\Worm%c.CL2",      false,       "Monsters\\Fat\\Fat%c%i.WAV",        false,       false,     NULL,                              { 13, 13, 13, 11, 19,  0 }, { 0, 0, 0, 0, 0, 0 }, "Cave Slug",               11,       15,     13,     75,    110, AI_SKELSD,   0                                                                  ,    1,   50,      8,          6,         13,     0,       0,           0,           0,          30, MC_ANIMAL,   RESIST_MAGIC                                                 , RESIST_MAGIC                                                 ,         0,        3,   994 },
	{    160,   2420, "Monsters\\Worm\\Worm%c.CL2",      false,       "Monsters\\Fat\\Fat%c%i.WAV",        false,       false,     NULL,                              { 13, 13, 13, 11, 19,  0 }, { 0, 0, 0, 0, 0, 0 }, "Devil Wyrm",              13,       17,     15,    100,    140, AI_SKELSD,   0                                                                  ,    2,   55,      8,          8,         16,     0,       0,           0,           0,          30, MC_ANIMAL,   RESIST_MAGIC | RESIST_FIRE                                   , RESIST_MAGIC | RESIST_FIRE                                   ,         0,        3,  1320 },
	{    160,   2420, "Monsters\\Worm\\Worm%c.CL2",      false,       "Monsters\\Fat\\Fat%c%i.WAV",        false,       false,     NULL,                              { 13, 13, 13, 11, 19,  0 }, { 0, 0, 0, 0, 0, 0 }, "Devourer",                15,       19,     17,    125,    200, AI_SKELSD,   0                                                                  ,    3,   60,      8,         10,         20,     0,       0,           0,           0,          35, MC_ANIMAL,   RESIST_MAGIC | RESIST_FIRE |                    IMUNE_NULL_40, RESIST_MAGIC | RESIST_FIRE |                    IMUNE_NULL_40,         0,        3,  1827 },
	{    128,   1680, "Monsters\\Magma\\Magma%c.CL2",    true,        "Monsters\\Magma\\Magma%c%i.WAV",    true,        false,     NULL,                              {  8, 10, 14,  7, 18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Magma Demon",             14,       17,     13,     50,     70, AI_MAGMA,                                     MonsterFlag::search | MonsterFlag::can_open_door,    0,   45,      4,          2,         10,    50,      13,           0,           0,          45, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE                                   , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  1076 },
	{    128,   1680, "Monsters\\Magma\\Magma%c.CL2",    true,        "Monsters\\Magma\\Magma%c%i.WAV",    true,        true,      "Monsters\\Magma\\Yellow.TRN",     {  8, 10, 14,  7, 18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Blood Stone",             15,       19,     14,     55,     75, AI_MAGMA,                                     MonsterFlag::search | MonsterFlag::can_open_door,    1,   50,      4,          2,         12,    50,      14,           0,           0,          45, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  1309 },
	{    128,   1680, "Monsters\\Magma\\Magma%c.CL2",    true,        "Monsters\\Magma\\Magma%c%i.WAV",    true,        true,      "Monsters\\Magma\\Blue.TRN",       {  8, 10, 14,  7, 18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Hell Stone",              16,       20,     16,     60,     80, AI_MAGMA,                                     MonsterFlag::search | MonsterFlag::can_open_door,    2,   60,      4,          2,         20,    60,      14,           0,           0,          50, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  1680 },
	{    128,   1680, "Monsters\\Magma\\Magma%c.CL2",    true,        "Monsters\\Magma\\Magma%c%i.WAV",    true,        true,      "Monsters\\Magma\\Wierd.TRN",      {  8, 10, 14,  7, 18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Lava Lord",               17,       21,     18,     70,     85, AI_MAGMA,                                     MonsterFlag::search | MonsterFlag::can_open_door,    3,   75,      4,          4,         24,    60,      14,           0,           0,          60, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  2124 },
	{    160,   1630, "Monsters\\Rhino\\Rhino%c.CL2",    true,        "Monsters\\Rhino\\Rhino%c%i.WAV",    true,        false,     NULL,                              {  8,  8, 14,  6, 16,  6 }, { 2, 0, 0, 0, 0, 0 }, "Horned Demon",            12,       16,     13,     40,     80, AI_RHINO,                                     MonsterFlag::search | MonsterFlag::can_open_door,    0,   60,      7,          2,         16,   100,       0,           5,          32,          40, MC_ANIMAL,   0                                                            ,                RESIST_FIRE                                   ,         0,        7,  1172 },
	{    160,   1630, "Monsters\\Rhino\\Rhino%c.CL2",    true,        "Monsters\\Rhino\\Rhino%c%i.WAV",    true,        true,      "Monsters\\Rhino\\Orange.TRN",     {  8,  8, 14,  6, 16,  6 }, { 2, 0, 0, 0, 0, 0 }, "Mud Runner",              14,       18,     15,     50,     90, AI_RHINO,                                     MonsterFlag::search | MonsterFlag::can_open_door,    1,   70,      7,          6,         18,   100,       0,          12,          36,          45, MC_ANIMAL,   0                                                            ,                RESIST_FIRE                                   ,         0,        7,  1404 },
	{    160,   1630, "Monsters\\Rhino\\Rhino%c.CL2",    true,        "Monsters\\Rhino\\Rhino%c%i.WAV",    true,        true,      "Monsters\\Rhino\\Blue.TRN",       {  8,  8, 14,  6, 16,  6 }, { 2, 0, 0, 0, 0, 0 }, "Frost Charger",           16,       20,     17,     60,    100, AI_RHINO,                                     MonsterFlag::search | MonsterFlag::can_open_door,    2,   80,      7,          8,         20,   100,       0,          20,          40,          50, MC_ANIMAL,   IMUNE_MAGIC  |               RESIST_LIGHTNING                , IMUNE_MAGIC  |               RESIST_LIGHTNING                ,         0,        7,  1720 },
	{    160,   1630, "Monsters\\Rhino\\Rhino%c.CL2",    true,        "Monsters\\Rhino\\Rhino%c%i.WAV",    true,        true,      "Monsters\\Rhino\\RhinoB.TRN",     {  8,  8, 14,  6, 16,  6 }, { 2, 0, 0, 0, 0, 0 }, "Obsidian Lord",           18,       22,     19,     70,    110, AI_RHINO,                                     MonsterFlag::search | MonsterFlag::can_open_door,    3,   90,      7,         10,         22,   100,       0,          20,          50,          55, MC_ANIMAL,   IMUNE_MAGIC  |               RESIST_LIGHTNING                , IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING                 ,         0,        7,  1809 },
	{    128,   1740, "Monsters\\Demskel\\Demskl%c.CL2", true,        "Monsters\\Thin\\Thin%c%i.WAV",      true,        false,     "Monsters\\Thin\\Thinv3.TRN",      { 10,  8, 20,  6, 24, 16 }, { 3, 0, 0, 0, 0, 0 }, "Bone Demon",              10,       14,     12,     70,     70, AI_STORM,    0                                                                  ,    0,   60,      8,          6,         14,    12,       0,           0,           0,          50, MC_DEMON,    IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        7,  1344 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      true,        "Monsters\\Thin\\Thin%c%i.WAV",      true,        true,      "Monsters\\Thin\\Thinv3.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Red Death",               14,       18,     16,     96,     96, AI_STORM,    0                                                                  ,    1,   75,      5,         10,         20,     0,       0,           0,           0,          60, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  2168 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      true,        "Monsters\\Thin\\Thin%c%i.WAV",      true,        true,      "Monsters\\Thin\\Thinv3.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Litch Demon",             16,       20,     18,    110,    110, AI_STORM,    0                                                                  ,    2,   80,      5,         10,         24,     0,       0,           0,           0,          45, MC_DEMON,    IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  2736 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      true,        "Monsters\\Thin\\Thin%c%i.WAV",      true,        true,      "Monsters\\Thin\\Thinv3.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Undead Balrog",           20,       24,     22,    130,    130, AI_STORM,    0                                                                  ,    3,   85,      5,         12,         30,     0,       0,           0,           0,          65, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7,  3575 },
	{    128,   1460, "Monsters\\Fireman\\FireM%c.CL2",  true,        "Monsters\\Acid\\Acid%c%i.WAV",      false,       false,     NULL,                              { 14, 19, 20,  8, 14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Incinerator",             14,       18,     16,     30,     45, AI_FIREMAN,  0                                                                  ,    0,   75,      8,          8,         16,     0,       0,           0,           0,          25, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        3,  1888 },
	{    128,   1460, "Monsters\\Fireman\\FireM%c.CL2",  true,        "Monsters\\Acid\\Acid%c%i.WAV",      false,       false,     NULL,                              { 14, 19, 20,  8, 14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Flame Lord",              16,       20,     18,     40,     55, AI_FIREMAN,  0                                                                  ,    1,   75,      8,         10,         20,     0,       0,           0,           0,          25, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        3,  2250 },
	{    128,   1460, "Monsters\\Fireman\\FireM%c.CL2",  true,        "Monsters\\Acid\\Acid%c%i.WAV",      false,       false,     NULL,                              { 14, 19, 20,  8, 14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Doom Fire",               18,       22,     20,     50,     65, AI_FIREMAN,  0                                                                  ,    2,   80,      8,         12,         24,     0,       0,           0,           0,          30, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                , IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                ,         0,        3,  2740 },
	{    128,   1460, "Monsters\\Fireman\\FireM%c.CL2",  true,        "Monsters\\Acid\\Acid%c%i.WAV",      false,       false,     NULL,                              { 14, 19, 20,  8, 14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Hell Burner",             20,       24,     22,     60,     80, AI_FIREMAN,  0                                                                  ,    3,   85,      8,         15,         30,     0,       0,           0,           0,          30, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                , IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                ,         0,        3,  3355 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      true,        "Monsters\\Thin\\Thin%c%i.WAV",      true,        true,      "Monsters\\Thin\\Thinv3.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Red Storm",               17,       21,     18,     55,    110, AI_STORM,                                     MonsterFlag::search | MonsterFlag::can_open_door,    0,   80,      5,          8,         18,    75,       8,           4,          16,          30, MC_DEMON,    IMUNE_MAGIC  |               RESIST_LIGHTNING                , IMUNE_MAGIC  |               IMUNE_LIGHTNING                 ,         0,        7,  2160 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      true,        "Monsters\\Thin\\Thin%c%i.WAV",      true,        false,     NULL,                              {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Storm Rider",             19,       23,     20,     60,    120, AI_STORM,                                     MonsterFlag::search | MonsterFlag::can_open_door,    1,   80,      5,          8,         18,    80,       8,           4,          16,          30, MC_DEMON,    RESIST_MAGIC |               IMUNE_LIGHTNING                 , IMUNE_MAGIC  |               IMUNE_LIGHTNING                 ,         0,        7,  2391 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      true,        "Monsters\\Thin\\Thin%c%i.WAV",      true,        true,      "Monsters\\Thin\\Thinv2.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Storm Lord",              21,       25,     22,     75,    135, AI_STORM,                                     MonsterFlag::search | MonsterFlag::can_open_door,    2,   85,      5,         12,         24,    75,       8,           4,          16,          35, MC_DEMON,    RESIST_MAGIC |               IMUNE_LIGHTNING                 , IMUNE_MAGIC  |               IMUNE_LIGHTNING                 ,         0,        7,  2775 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      true,        "Monsters\\Thin\\Thin%c%i.WAV",      true ,       true,      "Monsters\\Thin\\Thinv1.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Maelstorm",               23,       27,     24,     90,    150, AI_STORM,                                     MonsterFlag::search | MonsterFlag::can_open_door,    3,   90,      5,         12,         28,    75,       8,           4,          16,          40, MC_DEMON,    RESIST_MAGIC |               IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  3177 },
	{    128,   1650, "Monsters\\BigFall\\Fallg%c.CL2",  true,        "Monsters\\BigFall\\Bfal%c%i.WAV",   false,       false,     NULL,                              { 10,  8, 11,  8, 17,  0 }, { 0, 0, 0, 0, 2, 2 }, "Devil Kin Brute",         20,       20,     24,    160,    220, AI_SKELSD,                                    MonsterFlag::search | MonsterFlag::can_open_door,    3,  100,      6,         18,         24,     0,       0,           0,           0,          75, MC_ANIMAL,   0                                                            , 0                                                            ,         0,        6,  2000 },
	{    160,   1650, "Monsters\\Gargoyle\\Gargo%c.CL2", true,        "Monsters\\Gargoyle\\Gargo%c%i.WAV", false,       false,     NULL,                              { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 2 }, "Winged-Demon",             8,       12,      9,     45,     60, AI_GARG,                                                     MonsterFlag::can_open_door,    0,   50,       7,        10,         16,     0,       0,           0,           0,          45, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,         0,        6,   662 },
	{    160,   1650, "Monsters\\Gargoyle\\Gargo%c.CL2", true,        "Monsters\\Gargoyle\\Gargo%c%i.WAV", false,       true,      "Monsters\\Gargoyle\\GarE.TRN",    { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 2 }, "Gargoyle",                12,       16,     13,     60,     90, AI_GARG,                                                     MonsterFlag::can_open_door,    1,   65,       7,        10,         16,     0,       0,           0,           0,          45, MC_DEMON,    IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        6,  1205 },
	{    160,   1650, "Monsters\\Gargoyle\\Gargo%c.CL2", true,        "Monsters\\Gargoyle\\Gargo%c%i.WAV", false,       true,      "Monsters\\Gargoyle\\GargBr.TRN",  { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 0 }, "Blood Claw",              16,       20,     19,     75,    125, AI_GARG,                                                     MonsterFlag::can_open_door,    2,   80,       7,        14,         22,     0,       0,           0,           0,          50, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        6,  1873 },
	{    160,   1650, "Monsters\\Gargoyle\\Gargo%c.CL2", true,        "Monsters\\Gargoyle\\Gargo%c%i.WAV", false,       true,      "Monsters\\Gargoyle\\GargB.TRN",   { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 0 }, "Death Wing",              18,       22,     23,     90,    150, AI_GARG,                                                     MonsterFlag::can_open_door,    3,   95,       7,        16,         28,     0,       0,           0,           0,          60, MC_DEMON,    IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        6,  2278 },
	{    160,   2220, "Monsters\\Mega\\Mega%c.CL2",      true,        "Monsters\\Mega\\Mega%c%i.WAV",      true,        false,     NULL,                              {  6,  7, 14,  1, 24,  5 }, { 3, 0, 0, 0, 2, 0 }, "Slayer",                  19,       23,     20,    120,    140, AI_MEGA,                                      MonsterFlag::search | MonsterFlag::can_open_door,    0,  100,      8,         12,         20,     0,       3,           0,           0,          60, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE                                    , RESIST_MAGIC | IMUNE_FIRE                                    ,         0,        7,  2300 },
	{    160,   2220, "Monsters\\Mega\\Mega%c.CL2",      true,        "Monsters\\Mega\\Mega%c%i.WAV",      true,        true,      "Monsters\\Mega\\Guard.TRN",       {  6,  7, 14,  1, 24,  5 }, { 3, 0, 0, 0, 2, 0 }, "Guardian",                21,       25,     22,    140,    160, AI_MEGA,                                      MonsterFlag::search | MonsterFlag::can_open_door,    1,  110,      8,         14,         22,     0,       3,           0,           0,          65, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE                                    , RESIST_MAGIC | IMUNE_FIRE                                    ,         0,        7,  2714 },
	{    160,   2220, "Monsters\\Mega\\Mega%c.CL2",      true,        "Monsters\\Mega\\Mega%c%i.WAV",      true,        true,      "Monsters\\Mega\\Vtexl.TRN",       {  6,  7, 14,  1, 24,  5 }, { 3, 0, 0, 0, 2, 0 }, "Vortex Lord",             23,       26,     24,    160,    180, AI_MEGA,                                      MonsterFlag::search | MonsterFlag::can_open_door,    2,  120,      8,         18,         24,     0,       3,           0,           0,          70, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE  |                    IMUNE_NULL_40, RESIST_MAGIC | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7,  3252 },
	{    160,   2220, "Monsters\\Mega\\Mega%c.CL2",      true,        "Monsters\\Mega\\Mega%c%i.WAV",      true,        true,      "Monsters\\Mega\\Balr.TRN",        {  6,  7, 14,  1, 24,  5 }, { 3, 0, 0, 0, 2, 0 }, "Balrog",                  25,       29,     26,    180,    200, AI_MEGA,                                      MonsterFlag::search | MonsterFlag::can_open_door,    3,  130,      8,         22,         30,     0,       3,           0,           0,          75, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE  |                    IMUNE_NULL_40, RESIST_MAGIC | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7,  3643 },
	{    160,   1270, "Monsters\\Snake\\Snake%c.CL2",    false,       "Monsters\\Snake\\Snake%c%i.WAV",    false,       false,     NULL,                              { 12, 11, 13,  5, 18,  0 }, { 2, 0, 0, 0, 1, 0 }, "Cave Viper",              20,       24,     21,    100,    150, AI_SNAKE,                                     MonsterFlag::search                      ,    0,   90,      8,          8,         20,     0,       0,           0,           0,          60, MC_DEMON,    IMUNE_MAGIC                                                  , IMUNE_MAGIC                                                  ,         0,        7,  2725 },
	{    160,   1270, "Monsters\\Snake\\Snake%c.CL2",    false,       "Monsters\\Snake\\Snake%c%i.WAV",    false,       true,      "Monsters\\Snake\\SnakR.TRN",      { 12, 11, 13,  5, 18,  0 }, { 2, 0, 0, 0, 1, 0 }, "Fire Drake",              22,       26,     23,    120,    170, AI_SNAKE,                                     MonsterFlag::search                      ,    1,  105,      8,         12,         24,     0,       0,           0,           0,          65, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE                                   , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  3139 },
	{    160,   1270, "Monsters\\Snake\\Snake%c.CL2",    false,       "Monsters\\Snake\\Snake%c%i.WAV",    false,       true,      "Monsters\\Snake\\Snakg.TRN",      { 12, 11, 13,  5, 18,  0 }, { 2, 0, 0, 0, 1, 0 }, "Gold Viper",              24,       27,     25,    140,    180, AI_SNAKE,                                     MonsterFlag::search                      ,    2,  120,      8,         15,         26,     0,       0,           0,           0,          70, MC_DEMON,    IMUNE_MAGIC  |               RESIST_LIGHTNING                , IMUNE_MAGIC  |               RESIST_LIGHTNING                ,         0,        7,  3540 },
	{    160,   1270, "Monsters\\Snake\\Snake%c.CL2",    false,       "Monsters\\Snake\\Snake%c%i.WAV",    false,       true,      "Monsters\\Snake\\Snakb.TRN",      { 12, 11, 13,  5, 18,  0 }, { 2, 0, 0, 0, 1, 0 }, "Azure Drake",             28,       30,     27,    160,    200, AI_SNAKE,                                     MonsterFlag::search                      ,    3,  130,      8,         18,         30,     0,       0,           0,           0,          75, MC_DEMON,                   RESIST_FIRE | RESIST_LIGHTNING                , IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING                 ,         0,        7,  3791 },
	{    160,   2120, "Monsters\\Black\\Black%c.CL2",    false,       "Monsters\\Black\\Black%c%i.WAV",    false,       false,     NULL,                              {  8,  8, 16,  4, 24,  0 }, { 2, 0, 0, 0, 0, 0 }, "Black Knight",            23,       27,     24,    150,    150, AI_SKELSD,                                    MonsterFlag::search                      ,    0,  110,      8,         15,         20,     0,       0,           0,           0,          75, MC_DEMON,    RESIST_MAGIC |               RESIST_LIGHTNING | IMUNE_NULL_40, RESIST_MAGIC |               IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  3360 },
	{    160,   2120, "Monsters\\Black\\Black%c.CL2",    false,       "Monsters\\Black\\Black%c%i.WAV",    false,       true,      "Monsters\\Black\\BlkKntRT.TRN",   {  8,  8, 16,  4, 24,  0 }, { 2, 0, 0, 0, 0, 0 }, "Doom Guard",              25,       29,     26,    165,    165, AI_SKELSD,                                    MonsterFlag::search                      ,    0,  130,      8,         18,         25,     0,       0,           0,           0,          75, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE |                    IMUNE_NULL_40, RESIST_MAGIC | IMUNE_FIRE  |                    IMUNE_NULL_40,         0,        7,  3650 },
	{    160,   2120, "Monsters\\Black\\Black%c.CL2",    false,       "Monsters\\Black\\Black%c%i.WAV",    false,       true,      "Monsters\\Black\\BlkKntBT.TRN",   {  8,  8, 16,  4, 24,  0 }, { 2, 0, 0, 0, 0, 0 }, "Steel Lord",              27,       30,     28,    180,    180, AI_SKELSD,                                    MonsterFlag::search                      ,    1,  120,      8,         20,         30,     0,       0,           0,           0,          80, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7,  4252 },
	{    160,   2120, "Monsters\\Black\\Black%c.CL2",    false,       "Monsters\\Black\\Black%c%i.WAV",    false,       true,      "Monsters\\Black\\BlkKntBe.TRN",   {  8,  8, 16,  4, 24,  0 }, { 2, 0, 0, 0, 0, 0 }, "Blood Knight",            24,       26,     30,    200,    200, AI_SKELSD,                                    MonsterFlag::search                      ,    1,  130,      8,         25,         35,     0,       0,           0,           0,          85, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  5130 },
	{     96,    484, "Monsters\\Unrav\\Unrav%c.CL2",    false,       "Monsters\\Acid\\Acid%c%i.WAV",      false,       false,     NULL,                              { 10, 10, 12,  5, 16,  0 }, { 0, 0, 0, 0, 0, 0 }, "Unraveler",               26,       28,     25,     70,    150, AI_SKELSD,   0                                                                  ,    0,   75,      7,         10,         20,     0,       0,           0,           0,          70, MC_UNDEAD,   IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        3,  3812 },
	{     96,    484, "Monsters\\Unrav\\Unrav%c.CL2",    false,       "Monsters\\Acid\\Acid%c%i.WAV",      false,       false,     NULL,                              { 10, 10, 12,  5, 16,  0 }, { 0, 0, 0, 0, 0, 0 }, "Hollow One",              28,       30,     27,    135,    240, AI_SKELSD,   0                                                                  ,    1,   75,      7,         12,         24,     0,       0,           0,           0,          75, MC_UNDEAD,   IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,  4374 },
	{     96,    484, "Monsters\\Unrav\\Unrav%c.CL2",    false,       "Monsters\\Acid\\Acid%c%i.WAV",      false,       false,     NULL,                              { 10, 10, 12,  5, 16,  0 }, { 0, 0, 0, 0, 0, 0 }, "Pain Master",             27,       30,     29,    110,    200, AI_SKELSD,   0                                                                  ,    2,   80,      7,         16,         30,     0,       0,           0,           0,          80, MC_UNDEAD,   IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,  5147 },
	{     96,    484, "Monsters\\Unrav\\Unrav%c.CL2",    false,       "Monsters\\Acid\\Acid%c%i.WAV",      false,       false,     NULL,                              { 10, 10, 12,  5, 16,  0 }, { 0, 0, 0, 0, 0, 0 }, "Reality Weaver",          28,       30,     30,    135,    240, AI_SKELSD,   0                                                                  ,    3,   85,      7,         20,         35,     0,       0,           0,           0,          85, MC_UNDEAD,   RESIST_MAGIC | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40, RESIST_MAGIC | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        3,  5925 },
	{    128,    980, "Monsters\\Succ\\Scbs%c.CL2",      false,       "Monsters\\Succ\\Scbs%c%i.WAV",      false,       false,     NULL,                              { 14,  8, 16,  7, 24,  0 }, { 0, 0, 0, 0, 0, 0 }, "Succubus",                22,       26,     24,    120,    150, AI_SUCC,                                                     MonsterFlag::can_open_door,    0,  100,     10,          1,         20,     0,       0,           0,           0,          60, MC_DEMON,    RESIST_MAGIC                                                 , IMUNE_MAGIC  | RESIST_FIRE                                   ,         0,        3,  3696 },
	{    128,    980, "Monsters\\Succ\\Scbs%c.CL2",      false,       "Monsters\\Succ\\Scbs%c%i.WAV",      false,       true,      "Monsters\\Succ\\Succb.TRN",       { 14,  8, 16,  7, 24,  0 }, { 0, 0, 0, 0, 0, 0 }, "Snow Witch",              25,       28,     26,    135,    175, AI_SUCC,                                                     MonsterFlag::can_open_door,    1,  110,     10,          1,         24,     0,       0,           0,           0,          65, MC_DEMON,                                 RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,  4084 },
	{    128,    980, "Monsters\\Succ\\Scbs%c.CL2",      false,       "Monsters\\Succ\\Scbs%c%i.WAV",      false,       true,      "Monsters\\Succ\\Succrw.TRN",      { 14,  8, 16,  7, 24,  0 }, { 0, 0, 0, 0, 0, 0 }, "Hell Spawn",              27,       30,     28,    150,    200, AI_SUCC,                                      MonsterFlag::search | MonsterFlag::can_open_door,    2,  115,     10,          1,         30,     0,       0,           0,           0,          75, MC_DEMON,    RESIST_MAGIC |               IMUNE_LIGHTNING                 , IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                ,         0,        3,  4480 },
	{    128,    980, "Monsters\\Succ\\Scbs%c.CL2",      false,       "Monsters\\Succ\\Scbs%c%i.WAV",      false,       true,      "Monsters\\Succ\\Succbw.TRN",      { 14,  8, 16,  7, 24,  0 }, { 0, 0, 0, 0, 0, 0 }, "Soul Burner",             28,       30,     30,    140,    225, AI_SUCC,                                      MonsterFlag::search | MonsterFlag::can_open_door,    3,  120,     10,          1,         35,     0,       0,           0,           0,          85, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE  | RESIST_LIGHTNING                , IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING                 ,         0,        3,  4644 },
	{    128,   2000, "Monsters\\Mage\\Mage%c.CL2",      true,        "Monsters\\Mage\\Mage%c%i.WAV",      false,       false,     NULL,                              { 12,  1, 20,  8, 28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Counselor",               24,       26,     25,     70,     70, AI_COUNSLR,                                                  MonsterFlag::can_open_door,    0,   90,      8,          8,         20,     0,       0,           0,           0,           0, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING                , RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING                ,         0,        7,  4070 },
	{    128,   2000, "Monsters\\Mage\\Mage%c.CL2",      true,        "Monsters\\Mage\\Mage%c%i.WAV",      false,       true,      "Monsters\\Mage\\Cnselg.TRN",      { 12,  1, 20,  8, 28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Magistrate",              26,       28,     27,     85,     85, AI_COUNSLR,                                                  MonsterFlag::can_open_door,    1,  100,      8,         10,         24,     0,       0,           0,           0,           0, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7,  4478 },
	{    128,   2000, "Monsters\\Mage\\Mage%c.CL2",      true,        "Monsters\\Mage\\Mage%c%i.WAV",      false,       true,      "Monsters\\Mage\\Cnselgd.TRN",     { 12,  1, 20,  8, 28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Cabalist",                28,       30,     29,    120,    120, AI_COUNSLR,                                                  MonsterFlag::can_open_door,    2,  110,      8,         14,         30,     0,       0,           0,           0,           0, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  4929 },
	{    128,   2000, "Monsters\\Mage\\Mage%c.CL2",      true,        "Monsters\\Mage\\Mage%c%i.WAV",      false,       true,      "Monsters\\Mage\\Cnselbk.TRN",     { 12,  1, 20,  8, 28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Advocate",                30,       30,     30,    145,    145, AI_COUNSLR,                                                  MonsterFlag::can_open_door,    3,  120,      8,         15,         25,     0,       0,           0,           0,           0, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  4968 },
	{     96,    386, "Monsters\\Golem\\Golem%c.CL2",    true,        "Monsters\\Golem\\Golm%c%i.WAV",     false,       false,     NULL,                              {  0, 16, 12,  0, 12, 20 }, { 0, 0, 0, 0, 0, 0 }, "Golem",                    0,        0,     12,      1,      1, AI_GOLUM,                                                    MonsterFlag::can_open_door,    0,    0,      7,          1,          1,     0,       0,           0,           0,           1, MC_DEMON,    0                                                            , 0                                                            ,         0,        0,     0 },
	{    160,   2000, "Monsters\\Diablo\\Diablo%c.CL2",  true,        "Monsters\\Diablo\\Diablo%c%i.WAV",  true,        false,     NULL,                              { 16,  6, 16,  6, 16, 16 }, { 0, 0, 0, 0, 0, 0 }, "The Dark Lord",           50,       50,     30,   1666,   1666, AI_DIABLO,                  MonsterFlag::knockback | MonsterFlag::search | MonsterFlag::can_open_door,    3,  220,      4,         30,         60,     0,      11,           0,           0,          70, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7, 31666 },
	{    128,   1060, "Monsters\\DarkMage\\Dmage%c.CL2", true,        "Monsters\\DarkMage\\Dmag%c%i.WAV",  false,       false,     NULL,                              {  6,  1, 21,  6, 23, 18 }, { 0, 0, 0, 0, 0, 0 }, "The Arch-Litch Malignus", 30,       30,     30,    160,    160, AI_COUNSLR,                                                  MonsterFlag::can_open_door,    3,  120,      8,         20,         40,     0,       0,           0,           0,          70, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  4968 },
	// clang-format on
};

/**
 * Map between .DUN file value and monster type enum
 */
uint8_t MonstConvTbl[] = {
	MonsterType::NZOMBIE,
	MonsterType::BZOMBIE,
	MonsterType::GZOMBIE,
	MonsterType::YZOMBIE,
	MonsterType::RFALLSP,
	MonsterType::DFALLSP,
	MonsterType::YFALLSP,
	MonsterType::BFALLSP,
	MonsterType::WSKELAX,
	MonsterType::TSKELAX,
	MonsterType::RSKELAX,
	MonsterType::XSKELAX,
	MonsterType::RFALLSD,
	MonsterType::DFALLSD,
	MonsterType::YFALLSD,
	MonsterType::BFALLSD,
	MonsterType::NSCAV,
	MonsterType::BSCAV,
	MonsterType::WSCAV,
	MonsterType::YSCAV,
	MonsterType::WSKELBW,
	MonsterType::TSKELBW,
	MonsterType::RSKELBW,
	MonsterType::XSKELBW,
	MonsterType::WSKELSD,
	MonsterType::TSKELSD,
	MonsterType::RSKELSD,
	MonsterType::XSKELSD,
	MonsterType::SNEAK,
	MonsterType::STALKER,
	MonsterType::UNSEEN,
	MonsterType::ILLWEAV,
	MonsterType::NGOATMC,
	MonsterType::BGOATMC,
	MonsterType::RGOATMC,
	MonsterType::GGOATMC,
	MonsterType::FIEND,
	MonsterType::GLOOM,
	MonsterType::BLINK,
	MonsterType::FAMILIAR,
	MonsterType::NGOATBW,
	MonsterType::BGOATBW,
	MonsterType::RGOATBW,
	MonsterType::GGOATBW,
	MonsterType::NACID,
	MonsterType::RACID,
	MonsterType::BACID,
	MonsterType::XACID,
	MonsterType::SKING,
	MonsterType::FAT,
	MonsterType::MUDMAN,
	MonsterType::TOAD,
	MonsterType::FLAYED,
	MonsterType::WYRM,
	MonsterType::CAVSLUG,
	MonsterType::DEVOUR,
	MonsterType::DVLWYRM,
	MonsterType::NMAGMA,
	MonsterType::YMAGMA,
	MonsterType::BMAGMA,
	MonsterType::WMAGMA,
	MonsterType::HORNED,
	MonsterType::MUDRUN,
	MonsterType::FROSTC,
	MonsterType::OBLORD,
	MonsterType::BONEDMN,
	MonsterType::REDDTH,
	MonsterType::LTCHDMN,
	MonsterType::UDEDBLRG,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::INCIN,
	MonsterType::FLAMLRD,
	MonsterType::DOOMFIRE,
	MonsterType::HELLBURN,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::RSTORM,
	MonsterType::STORM,
	MonsterType::STORML,
	MonsterType::MAEL,
	MonsterType::WINGED,
	MonsterType::GARGOYLE,
	MonsterType::BLOODCLW,
	MonsterType::DEATHW,
	MonsterType::MEGA,
	MonsterType::GUARD,
	MonsterType::VTEXLRD,
	MonsterType::BALROG,
	MonsterType::NSNAKE,
	MonsterType::RSNAKE,
	MonsterType::GSNAKE,
	MonsterType::BSNAKE,
	MonsterType::NBLACK,
	MonsterType::RTBLACK,
	MonsterType::BTBLACK,
	MonsterType::RBLACK,
	MonsterType::UNRAV,
	MonsterType::HOLOWONE,
	MonsterType::PAINMSTR,
	MonsterType::REALWEAV,
	MonsterType::SUCCUBUS,
	MonsterType::SNOWWICH,
	MonsterType::HLSPWN,
	MonsterType::SOLBRNR,
	MonsterType::COUNSLR,
	MonsterType::MAGISTR,
	MonsterType::CABALIST,
	MonsterType::ADVOCATE,
	MonsterType::NZOMBIE,
	MonsterType::DIABLO,
	MonsterType::NZOMBIE,
	MonsterFlag::golem,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::NZOMBIE,
	MonsterType::BIGFALL,
	MonsterType::DARKMAGE,
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	0, // Reserved for Hellfire
	MonsterType::CLEAVER,
	MonsterType::INVILORD,
	MonsterType::LRDSAYTR,
};

#define MAT_NEVER 0
#define MAT_ALWAYS 1
#define MAT_RETAIL 2
/**
 * Define what version a monster type is available in
 */
uint8_t MonstAvailTbl[] = {
	MAT_ALWAYS, // Zombie
	MAT_ALWAYS, // Ghoul
	MAT_ALWAYS, // Rotting Carcass
	MAT_ALWAYS, // Black Death
	MAT_ALWAYS, // Fallen One
	MAT_ALWAYS, // Carver
	MAT_ALWAYS, // Devil Kin
	MAT_ALWAYS, // Dark One
	MAT_ALWAYS, // Skeleton
	MAT_ALWAYS, // Corpse Axe
	MAT_ALWAYS, // Burning Dead
	MAT_ALWAYS, // Horror
	MAT_ALWAYS, // Fallen One
	MAT_ALWAYS, // Carver
	MAT_ALWAYS, // Devil Kin
	MAT_ALWAYS, // Dark One
	MAT_ALWAYS, // Scavenger
	MAT_ALWAYS, // Plague Eater
	MAT_ALWAYS, // Shadow Beast
	MAT_ALWAYS, // Bone Gasher
	MAT_ALWAYS, // Skeleton
	MAT_ALWAYS, // Corpse Bow
	MAT_ALWAYS, // Burning Dead
	MAT_ALWAYS, // Horror
	MAT_ALWAYS, // Skeleton Captain
	MAT_ALWAYS, // Corpse Captain
	MAT_ALWAYS, // Burning Dead Captain
	MAT_ALWAYS, // Horror Captain
	MAT_NEVER,  // Invisible Lord
	MAT_RETAIL, // Hidden
	MAT_RETAIL, // Stalker
	MAT_RETAIL, // Unseen
	MAT_RETAIL, // Illusion Weaver
	MAT_NEVER,  // Lord Sayter
	MAT_RETAIL, // Flesh Clan
	MAT_RETAIL, // Stone Clan
	MAT_RETAIL, // Fire Clan
	MAT_RETAIL, // Night Clan
	MAT_ALWAYS, // Fiend
	MAT_ALWAYS, // Blink
	MAT_ALWAYS, // Gloom
	MAT_ALWAYS, // Familiar
	MAT_RETAIL, // Flesh Clan
	MAT_RETAIL, // Stone Clan
	MAT_RETAIL, // Fire Clan
	MAT_RETAIL, // Night Clan
	MAT_RETAIL, // Acid Beast
	MAT_RETAIL, // Poison Spitter
	MAT_RETAIL, // Pit Beast
	MAT_RETAIL, // Lava Maw
	MAT_NEVER,  // Skeleton King
	MAT_NEVER,  // The Butcher
	MAT_RETAIL, // Overlord
	MAT_RETAIL, // Mud Man
	MAT_RETAIL, // Toad Demon
	MAT_RETAIL, // Flayed One
	MAT_NEVER,  // Wyrm
	MAT_NEVER,  // Cave Slug
	MAT_NEVER,  // Devil Wyrm
	MAT_NEVER,  // Devourer
	MAT_RETAIL, // Magma Demon
	MAT_RETAIL, // Blood Stone
	MAT_RETAIL, // Hell Stone
	MAT_RETAIL, // Lava Lord
	MAT_RETAIL, // Horned Demon
	MAT_RETAIL, // Mud Runner
	MAT_RETAIL, // Frost Charger
	MAT_RETAIL, // Obsidian Lord
	MAT_NEVER,  // Bone Demon (oldboned in Hellfire)
	MAT_NEVER,  // Red Death
	MAT_NEVER,  // Litch Demon
	MAT_NEVER,  // Undead Balrog
	MAT_NEVER,  // Incinerator
	MAT_NEVER,  // Flame Lord
	MAT_NEVER,  // Doom Fire
	MAT_NEVER,  // Hell Burner
	MAT_RETAIL, // Red Storm
	MAT_RETAIL, // Storm Rider
	MAT_RETAIL, // Storm Lord
	MAT_RETAIL, // Maelstorm
	MAT_NEVER,  // Devil Kin Brute
	MAT_RETAIL, // Winged-Demon
	MAT_RETAIL, // Gargoyle
	MAT_RETAIL, // Blood Claw
	MAT_RETAIL, // Death Wing
	MAT_RETAIL, // Slayer
	MAT_RETAIL, // Guardian
	MAT_RETAIL, // Vortex Lord
	MAT_RETAIL, // Balrog
	MAT_RETAIL, // Cave Viper
	MAT_RETAIL, // Fire Drake
	MAT_RETAIL, // Gold Viper
	MAT_RETAIL, // Azure Drake
	MAT_RETAIL, // Black Knight
	MAT_RETAIL, // Doom Guard
	MAT_RETAIL, // Steel Lord
	MAT_RETAIL, // Blood Knight
	MAT_NEVER,  // Unraveler
	MAT_NEVER,  // Hollow One
	MAT_NEVER,  // Pain Master
	MAT_NEVER,  // Reality Weaver
	MAT_RETAIL, // Succubus
	MAT_RETAIL, // Snow Witch
	MAT_RETAIL, // Hell Spawn
	MAT_RETAIL, // Soul Burner
	MAT_RETAIL, // Counselor
	MAT_RETAIL, // Magistrate
	MAT_RETAIL, // Cabalist
	MAT_RETAIL, // Advocate
	MAT_NEVER,  // Golem
	MAT_NEVER,  // The Dark Lord
	MAT_NEVER,  // The Arch-Litch Malignus
};

UniqMonstStruct UniqMonst[] = {
	// clang-format off
	// mtype,       mName,                   mTrnName, mlevel, mmaxhp, mAi,      mint, mMinDamage, mMaxDamage, mMagicRes,                                                     mUnqAttr, mUnqVar1, mUnqVar2, mtalkmsg
	{  MonsterType::NGOATMC,  "Gharbad the Weak",         "BSDB",     4,    120, AI_GARBUD,   3,          8,         16,                              IMUNE_LIGHTNING  | IMUNE_NULL_40,        0,        0,        0, TEXT_GARBUD1  },
	{  MonsterType::SKING,    "Skeleton King",            "GENRL",    0,    240, AI_SKELKING, 3,          6,         16, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        1,        0,        0, 0              },
	{  MonsterType::COUNSLR,  "Zhar the Mad",             "GENERAL",  8,    360, AI_ZHAR,     3,         16,         40, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING                ,        0,        0,        0, TEXT_ZHAR1    },
	{  MonsterType::BFALLSP,  "Snotspill",                "BNG",      4,    220, AI_SNOTSPIL, 3,         10,         18,                              RESIST_LIGHTNING                ,        0,        0,        0, TEXT_BANNER10 },
	{  MonsterType::ADVOCATE, "Arch-Bishop Lazarus",      "GENERAL",  0,    600, AI_LAZURUS,  3,         30,         50, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        0,        0,        0, TEXT_VILE13   },
	{  MonsterType::HLSPWN,   "Red Vex",                  "REDV",     0,    400, AI_LAZHELP,  3,         30,         50, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,        0,        0,        0, TEXT_VILE13   },
	{  MonsterType::HLSPWN,   "BlackJade",                "BLKJD",    0,    400, AI_LAZHELP,  3,         30,         50, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,        0,        0,        0, TEXT_VILE13   },
	{  MonsterType::RBLACK,   "Lachdanan",                "BHKA",    14,    500, AI_LACHDAN,  3,          0,          0, 0                                                            ,        0,        0,        0, TEXT_VEIL9    },
	{  MonsterType::BTBLACK,  "Warlord of Blood",         "GENERAL", 13,    850, AI_WARLORD,  3,         35,         50, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,        0,        0,        0, TEXT_WARLRD9  },
	{  MonsterType::CLEAVER,  "The Butcher",              "GENRL",    0,    220, AI_CLEAVER,  3,          6,         12,                RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::TSKELAX,  "Bonehead Keenaxe",         "BHKA",     2,     91, AI_SKELSD,   2,          4,         10, IMUNE_MAGIC  |                                  IMUNE_NULL_40,        7,      100,        0, 0              },
	{  MonsterType::RFALLSD,  "Bladeskin the Slasher",    "BSTS",     2,     51, AI_FALLEN,   0,          6,         18,                RESIST_FIRE                                   ,       11,       45,        0, 0              },
	{  MonsterType::NZOMBIE,  "Soulpus",                  "GENERAL",  2,    133, AI_ZOMBIE,   0,          4,          8,                RESIST_FIRE | RESIST_LIGHTNING                ,        0,        0,        0, 0              },
	{  MonsterType::RFALLSP,  "Pukerat the Unclean",      "PTU",      2,     77, AI_FALLEN,   3,          1,          5,                RESIST_FIRE                                   ,        0,        0,        0, 0              },
	{  MonsterType::WSKELAX,  "Boneripper",               "BR",       2,     54, AI_BAT,      0,          6,         15, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::NZOMBIE,  "Rotfeast the Hungry",      "ETH",      2,     85, AI_SKELSD,   3,          4,         12, IMUNE_MAGIC  |                                  IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::DFALLSD,  "Gutshank the Quick",       "GTQ",      3,     66, AI_BAT,      2,          6,         16,                RESIST_FIRE                                   ,        3,        0,        0, 0              },
	{  MonsterType::TSKELSD,  "Brokenhead Bangshield",    "BHBS",     3,    108, AI_SKELSD,   3,         12,         20, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::YFALLSP,  "Bongo",                    "BNG",      3,    178, AI_FALLEN,   3,          9,         21, 0                                                            ,        3,        0,        0, 0              },
	{  MonsterType::BZOMBIE,  "Rotcarnage",               "RCRN",     3,    102, AI_ZOMBIE,   3,          9,         24, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,       11,       45,        0, 0              },
	{  MonsterType::NSCAV,    "Shadowbite",               "SHBT",     2,     60, AI_SKELSD,   3,          3,         20,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MonsterType::WSKELBW,  "Deadeye",                  "DE",       2,     49, AI_GOATBOW,  0,          6,          9, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::RSKELAX,  "Madeye the Dead",          "MTD",      4,     75, AI_BAT,      0,          9,         21, IMUNE_MAGIC  | IMUNE_FIRE                                    ,       11,       30,        0, 0              },
	{  MonsterType::BSCAV,    "El Chupacabras",           "GENERAL",  3,    120, AI_GOATMC,   0,         10,         18,                RESIST_FIRE                                   ,        3,       30,        0, 0              },
	{  MonsterType::TSKELBW,  "Skullfire",                "SKFR",     3,    125, AI_GOATBOW,  1,          6,         10,                IMUNE_FIRE                                    ,        0,      100,        0, 0              },
	{  MonsterType::SNEAK,    "Warpskull",                "TSPO",     3,    117, AI_SNEAK,    2,          6,         18,                RESIST_FIRE | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::GZOMBIE,  "Goretongue",               "PMR",      3,    156, AI_SKELSD,   1,         15,         30, IMUNE_MAGIC  |                                  IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::WSCAV,    "Pulsecrawler",             "BHKA",     4,    150, AI_SCAV,     0,         16,         20,                IMUNE_FIRE  | RESIST_LIGHTNING                ,       11,       45,        0, 0              },
	{  MonsterType::BLINK,    "Moonbender",               "GENERAL",  4,    135, AI_BAT,      0,          9,         27,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MonsterType::BLINK,    "Wrathraven",               "GENERAL",  5,    135, AI_BAT,      2,          9,         22,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MonsterType::YSCAV,    "Spineeater",               "GENERAL",  4,    180, AI_SCAV,     1,         18,         25,                              IMUNE_LIGHTNING  | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::RSKELBW,  "Blackash the Burning",     "BASHTB",   4,    120, AI_GOATBOW,  0,          6,         16, IMUNE_MAGIC  | IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MonsterType::BFALLSD,  "Shadowcrow",               "GENERAL",  5,    270, AI_SNEAK,    2,         12,         25, 0                                                            ,        3,        0,        0, 0              },
	{  MonsterType::LRDSAYTR, "Blightstone the Weak",     "BHKA",     4,    360, AI_SKELSD,   0,          4,         12, IMUNE_MAGIC  |               RESIST_LIGHTNING                ,        7,       70,        0, 0              },
	{  MonsterType::FAT,      "Bilefroth the Pit Master", "BFTP",     6,    210, AI_BAT,      1,         16,         23, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::NGOATBW,  "Bloodskin Darkbow",        "BSDB",     5,    207, AI_GOATBOW,  0,          3,         16,                RESIST_FIRE | RESIST_LIGHTNING                ,       11,       55,        0, 0              },
	{  MonsterType::GLOOM,    "Foulwing",                 "DB",       5,    246, AI_RHINO,    3,         12,         28,                RESIST_FIRE                                   ,        3,        0,        0, 0              },
	{  MonsterType::XSKELSD,  "Shadowdrinker",            "SHDR",     5,    300, AI_SNEAK,    1,         18,         26, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        8,       45,        0, 0              },
	{  MonsterType::UNSEEN,   "Hazeshifter",              "BHKA",     5,    285, AI_SNEAK,    3,         18,         30,                              IMUNE_LIGHTNING  | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::NACID,    "Deathspit",                "BFDS",     6,    303, AI_ACIDUNIQ, 0,         12,         32,                RESIST_FIRE | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::RGOATMC,  "Bloodgutter",              "BGBL",     6,    315, AI_BAT,      1,         24,         34,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MonsterType::BGOATMC,  "Deathshade Fleshmaul",     "DSFM",     6,    276, AI_RHINO,    0,         12,         24, IMUNE_MAGIC  | RESIST_FIRE                                   ,        8,       65,        0, 0              },
	{  MonsterType::WYRM,     "Warmaggot the Mad",        "GENERAL",  6,    246, AI_BAT,      3,         15,         30,                              RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::STORM,    "Glasskull the Jagged",     "BHKA",     7,    354, AI_STORM,    0,         18,         30, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::RGOATBW,  "Blightfire",               "BLF",      7,    321, AI_SUCC,     2,         13,         21,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MonsterType::GARGOYLE, "Nightwing the Cold",       "GENERAL",  7,    342, AI_BAT,      1,         18,         26, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::GGOATBW,  "Gorestone",                "GENERAL",  7,    303, AI_GOATBOW,  1,         15,         28,                              RESIST_LIGHTNING | IMUNE_NULL_40,        7,       70,        0, 0              },
	{  MonsterType::BMAGMA,   "Bronzefist Firestone",     "GENERAL",  8,    360, AI_MAGMA,    0,         30,         36, IMUNE_MAGIC  | RESIST_FIRE                                   ,        3,        0,        0, 0              },
	{  MonsterType::INCIN,    "Wrathfire the Doomed",     "WFTD",     8,    270, AI_SKELSD,   2,         20,         30, IMUNE_MAGIC  | RESIST_FIRE |  RESIST_LIGHTNING               ,        3,        0,        0, 0              },
	{  MonsterType::NMAGMA,   "Firewound the Grim",       "BHKA",     8,    303, AI_MAGMA,    0,         18,         22, IMUNE_MAGIC  | RESIST_FIRE                                   ,        3,        0,        0, 0              },
	{  MonsterType::MUDMAN,   "Baron Sludge",             "BSM",      8,    315, AI_SNEAK,    3,         25,         34, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,       11,       75,        0, 0              },
	{  MonsterType::GGOATMC,  "Blighthorn Steelmace",     "BHSM",     7,    250, AI_RHINO,    0,         20,         28,                              RESIST_LIGHTNING                ,       11,       45,        0, 0              },
	{  MonsterType::RACID,    "Chaoshowler",              "GENERAL",  8,    240, AI_ACIDUNIQ, 0,         12,         20, 0                                                            ,        3,        0,        0, 0              },
	{  MonsterType::REDDTH,   "Doomgrin the Rotting",     "GENERAL",  8,    405, AI_STORM,    3,         25,         50, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::FLAMLRD,  "Madburner",                "GENERAL",  9,    270, AI_STORM,    0,         20,         40, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING                 ,        3,        0,        0, 0              },
	{  MonsterType::LTCHDMN,  "Bonesaw the Litch",        "GENERAL",  9,    495, AI_STORM,    2,         30,         55, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::MUDRUN,   "Breakspine",               "GENERAL",  9,    351, AI_RHINO,    0,         25,         34,                RESIST_FIRE                                   ,        3,        0,        0, 0              },
	{  MonsterType::REDDTH,   "Devilskull Sharpbone",     "GENERAL",  9,    444, AI_STORM,    1,         25,         40,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MonsterType::STORM,    "Brokenstorm",              "GENERAL",  9,    411, AI_STORM,    2,         25,         36,                              IMUNE_LIGHTNING                 ,        3,        0,        0, 0              },
	{  MonsterType::RSTORM,   "Stormbane",                "GENERAL",  9,    555, AI_STORM,    3,         30,         30,                              IMUNE_LIGHTNING                 ,        3,        0,        0, 0              },
	{  MonsterType::TOAD,     "Oozedrool",                "GENERAL",  9,    483, AI_FAT,      3,         25,         30,                              RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::BLOODCLW, "Goldblight of the Flame",  "GENERAL", 10,    405, AI_GARG,     0,         15,         35, IMUNE_MAGIC  | IMUNE_FIRE                                    ,       11,       80,        0, 0              },
	{  MonsterType::OBLORD,   "Blackstorm",               "GENERAL", 10,    525, AI_RHINO,    3,         20,         40, IMUNE_MAGIC  |               IMUNE_LIGHTNING                 ,       11,       90,        0, 0              },
	{  MonsterType::RACID,    "Plaguewrath",              "GENERAL", 10,    450, AI_ACIDUNIQ, 2,         20,         30, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::RSTORM,   "The Flayer",               "GENERAL", 10,    501, AI_STORM,    1,         20,         35, RESIST_MAGIC | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::FROSTC,   "Bluehorn",                 "GENERAL", 11,    477, AI_RHINO,    1,         25,         30, IMUNE_MAGIC  | RESIST_FIRE                                   ,       11,       90,        0, 0              },
	{  MonsterType::HELLBURN, "Warpfire Hellspawn",       "GENERAL", 11,    525, AI_FIREMAN,  3,         10,         40, RESIST_MAGIC | IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MonsterType::NSNAKE,   "Fangspeir",                "GENERAL", 11,    444, AI_SKELSD,   1,         15,         32,                IMUNE_FIRE  |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::UDEDBLRG, "Festerskull",              "GENERAL", 11,    600, AI_STORM,    2,         15,         30, IMUNE_MAGIC  |                                  IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::NBLACK,   "Lionskull the Bent",       "GENERAL", 12,    525, AI_SKELSD,   2,         25,         25, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::COUNSLR,  "Blacktongue",              "GENERAL", 12,    360, AI_COUNSLR,  3,         15,         30,                RESIST_FIRE |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::DEATHW,   "Viletouch",                "GENERAL", 12,    525, AI_GARG,     3,         20,         40,                              IMUNE_LIGHTNING  | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::RSNAKE,   "Viperflame",               "GENERAL", 12,    570, AI_SKELSD,   1,         25,         35,                IMUNE_FIRE  | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::BSNAKE,   "Fangskin",                 "BHKA",    14,    681, AI_SKELSD,   2,         15,         50, IMUNE_MAGIC  |               RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::SUCCUBUS, "Witchfire the Unholy",     "GENERAL", 12,    444, AI_SUCC,     3,         10,         20, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::BALROG,   "Blackskull",               "BHKA",    13,    750, AI_SKELSD,   3,         25,         40, IMUNE_MAGIC  |               RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::UNRAV,    "Soulslash",                "GENERAL", 12,    450, AI_SKELSD,   0,         25,         25, IMUNE_MAGIC  |                                  IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::VTEXLRD,  "Windspawn",                "GENERAL", 12,    711, AI_SKELSD,   1,         35,         40, IMUNE_MAGIC  | IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MonsterType::GSNAKE,   "Lord of the Pit",          "GENERAL", 13,    762, AI_SKELSD,   2,         25,         42,                RESIST_FIRE |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::RTBLACK,  "Rustweaver",               "GENERAL", 13,    400, AI_SKELSD,   3,          1,         60, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::HOLOWONE, "Howlingire the Shade",     "GENERAL", 13,    450, AI_SKELSD,   2,         40,         75,                RESIST_FIRE | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::MAEL,     "Doomcloud",                "GENERAL", 13,    612, AI_STORM,    1,          1,         60,                RESIST_FIRE | IMUNE_LIGHTNING                 ,        0,        0,        0, 0              },
	{  MonsterType::PAINMSTR, "Bloodmoon Soulfire",       "GENERAL", 13,    684, AI_SKELSD,   1,         15,         40, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::SNOWWICH, "Witchmoon",                "GENERAL", 13,    310, AI_SUCC,     3,         30,         40,                              RESIST_LIGHTNING                ,        0,        0,        0, 0              },
	{  MonsterType::VTEXLRD,  "Gorefeast",                "GENERAL", 13,    771, AI_SKELSD,   3,         20,         55,                RESIST_FIRE |                    IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::RTBLACK,  "Graywar the Slayer",       "GENERAL", 14,    672, AI_SKELSD,   1,         30,         50,                              RESIST_LIGHTNING | IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::MAGISTR,  "Dreadjudge",               "GENERAL", 14,    540, AI_COUNSLR,  1,         30,         40, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MonsterType::HLSPWN,   "Stareye the Witch",        "GENERAL", 14,    726, AI_SUCC,     2,         30,         50,                IMUNE_FIRE                                    ,        0,        0,        0, 0              },
	{  MonsterType::BTBLACK,  "Steelskull the Hunter",    "GENERAL", 14,    831, AI_SKELSD,   3,         40,         50,                              RESIST_LIGHTNING | IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::RBLACK,   "Sir Gorash",               "GENERAL", 16,   1050, AI_SKELSD,   1,         20,         60,                                                 IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::CABALIST, "The Vizier",               "GENERAL", 15,    850, AI_COUNSLR,  2,         25,         40,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MonsterType::REALWEAV, "Zamphir",                  "GENERAL", 15,    891, AI_SKELSD,   2,         30,         50, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::HLSPWN,   "Bloodlust",                "GENERAL", 15,    825, AI_SUCC,     1,         20,         55, IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::HLSPWN,   "Webwidow",                 "GENERAL", 16,    774, AI_SUCC,     1,         20,         50, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::SOLBRNR,  "Fleshdancer",              "GENERAL", 16,    999, AI_SUCC,     3,         30,         50, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MonsterType::OBLORD,   "Grimspike",                "GENERAL", 19,    534, AI_SNEAK,    1,         25,         40, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MonsterType::STORML,   "Doomlock",                 "GENERAL", 28,    534, AI_SNEAK,    1,         35,         55, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  -1,          NULL,                       NULL,       0,      0, 0,           0,          0,          0, 0                                                            ,        0,        0,        0, 0              },
	// clang-format on
};

DEVILUTION_END_NAMESPACE
