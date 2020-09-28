/**
 * @file level.h
 *
 * Game level class
 */
#ifndef __LEVEL_H__
#define __LEVEL_H__

DEVILUTION_BEGIN_NAMESPACE

enum class DunType {
	town = 0x0,
	cathedral = 0x1,
	catacombs = 0x2,
	caves = 0x3,
	hell = 0x4,
	none = 0xFF,
};

class Level {
public:
	Level(DunType duntype);
	~Level();

	virtual void create(int lvldir);
	virtual void loadGFX();
	virtual void DRLG_CreateThemeRoom(int themeIndex);


	void setType(DunType duntype);
	void setpc(RECT32 newsetpc);
	const RECT32 &getpc();
	DunType type();

	V2Di dmin; // min dungeon walkable position
	V2Di dmax; // max dungeon walkable position

	BYTE currlevel;
	BOOLEAN setlevel;
	BYTE setlvlnum;
	char setlvltype;

	char TransVal; // Transfer???
	BOOLEAN TransList[256];

	int themeCount;
	THEME_LOC themeLoc[MAXTHEMES];

	// Loaded with level
	BYTE *pDungeonCels = 0;
	BYTE *pMegaTiles = 0;
	BYTE *pLevelPieces = 0;
	BYTE *pSpecialCels = 0;

	std::string automapFile();
	bool hasAutomapFile();
protected:
	std::string _automapFile;
	DunType _leveltype;
	RECT32 _setpc;
};

extern Level lvl;

DEVILUTION_END_NAMESPACE

#endif /* __LEVEL_H__ */
