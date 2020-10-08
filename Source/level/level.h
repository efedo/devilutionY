/**
 * @file level.h
 *
 * Game level class
 */
#ifndef __LEVEL_H__
#define __LEVEL_H__

//#include "../enums.h"
//#include "../structs.h"

namespace dvl {

class Level {
public:
	Level();
	Level(DunType duntype);
	~Level();

	virtual void create(int lvldir);
	virtual void loadGFX();
	virtual void DRLG_CreateThemeRoom(int themeIndex);
	virtual void LoadSetMap();
	void PlrClrTrans(V2Di pos);
	void PlrDoTrans(V2Di pos);


	void setType(DunType duntype);
	void setpc(RECT32 newsetpc);
	const RECT32 &getpc();
	DunType type();

	V2Di dmin; // min dungeon walkable position
	V2Di dmax; // max dungeon walkable position

	uint8_t currlevel;
	bool setlevel;
	SetLvl setlvlnum;
	DunType setlvltype;

	char TransVal; // Transfer???
	bool TransList[256];

	int themeCount;
	THEME_LOC themeLoc[MAXTHEMES];

	// Loaded with level
	uint8_t *pDungeonCels = 0;
	uint8_t *pMegaTiles = 0;
	uint8_t *pLevelPieces = 0;
	uint8_t *pSpecialCels = 0;

	std::string automapFile();
	bool hasAutomapFile();
protected:
	std::string _automapFile = "";
	DunType _leveltype = DunType::town;
	RECT32 _setpc;
};

void PlrClrTrans(V2Di pos);
void PlrDoTrans(V2Di pos);


extern Level lvl;

}

#endif /* __LEVEL_H__ */
