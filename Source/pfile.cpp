#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"
#include "file_util.h"

DEVILUTION_BEGIN_NAMESPACE

#define PASSWORD_SINGLE "xrgyrkj1"
#define PASSWORD_MULTI "szqnlsk1"

/** List of character names for the character selection screen. */
static char hero_names[MAX_CHARACTERS][PLR_NAME_LEN];
bool gbValidSaveFile;

void pfile_write_hero()
{
	DWORD save_num;
	PkPlayerStruct pkplr;

	save_num = pfile_get_save_num_from_name(myplr().data._pName);
	if (pfile_open_archive(true, save_num)) {
		PackPlayer(&pkplr, myplr(), plr.isSingleplayer());
		pfile_encode_hero(&pkplr);
		pfile_flush(plr.isSingleplayer(), save_num);
	}
}

DWORD pfile_get_save_num_from_name(const char *name)
{
	DWORD i;

	for (i = 0; i < MAX_CHARACTERS; i++) {
		if (!strcasecmp(hero_names[i], name))
			break;
	}

	return i;
}

void pfile_encode_hero(const PkPlayerStruct *pPack)
{
	uint8_t *packed;
	DWORD packed_len;
	char password[16] = PASSWORD_SINGLE;

	if (plr.isMultiplayer())
		strcpy(password, PASSWORD_MULTI);

	packed_len = codec_get_encoded_len(sizeof(*pPack));
	packed = (uint8_t *)DiabloAllocPtr(packed_len);
	memcpy(packed, pPack, sizeof(*pPack));
	codec_encode(packed, sizeof(*pPack), packed_len, password);
	mpqapi_write_file("hero", packed, packed_len);
	mem_free_dbg(packed);
}

bool pfile_open_archive(bool update, DWORD save_num)
{
	char FileName[MAX_PATH];

	pfile_get_save_path(FileName, sizeof(FileName), save_num);
	if (OpenMPQ(FileName, save_num))
		return true;

	return false;
}

void pfile_get_save_path(char *pszBuf, DWORD dwBufSize, DWORD save_num)
{
	char path[MAX_PATH];
	const char *fmt = "%smulti_%d.sv";

	if (plr.isSingleplayer())
		fmt = "%ssingle_%d.sv";
	GetPrefPath(path, MAX_PATH);
	snprintf(pszBuf, MAX_PATH, fmt, path, save_num);
}

void pfile_flush(bool is_single_player, DWORD save_num)
{
	char FileName[MAX_PATH];

	pfile_get_save_path(FileName, sizeof(FileName), save_num);
	mpqapi_flush_and_close(FileName, is_single_player, save_num);
}

bool pfile_create_player_description(char *dst, DWORD len)
{
	char desc[128];
	_uiheroinfo uihero;

	plr.setLocal(0);
	pfile_read_player_from_save();
	game_2_ui_player(&plr[0].data, &uihero, gbValidSaveFile);
	UiSetupPlayerInfo(gszHero, &uihero, GAME_ID);

	if (dst != NULL && len) {
		if (UiCreatePlayerDescription(&uihero, GAME_ID, desc) == 0)
			return false;
		SStrCopy(dst, desc, len);
	}
	return true;
}

bool pfile_rename_hero(const char *name_1, const char *name_2)
{
	int i;
	DWORD save_num;
	_uiheroinfo uihero;
	bool found = false;

	if (pfile_get_save_num_from_name(name_2) == MAX_CHARACTERS) {
		for (i = 0; i != MAX_PLRS; i++) {
			if (!strcasecmp(name_1, plr[i].data._pName)) {
				found = true;
				break;
			}
		}
	}

	if (!found)
		return false;
	save_num = pfile_get_save_num_from_name(name_1);
	if (save_num == MAX_CHARACTERS)
		return false;

	SStrCopy(hero_names[save_num], name_2, PLR_NAME_LEN);
	SStrCopy(plr[i].data._pName, name_2, PLR_NAME_LEN);
	if (!strcasecmp(gszHero, name_1))
		SStrCopy(gszHero, name_2, sizeof(gszHero));
	game_2_ui_player(&plr[0].data, &uihero, gbValidSaveFile);
	UiSetupPlayerInfo(gszHero, &uihero, GAME_ID);
	pfile_write_hero();
	return true;
}

void pfile_flush_W()
{
	pfile_flush(true, pfile_get_save_num_from_name(myplr().data._pName));
}

void game_2_ui_player(const PlayerStruct *p, _uiheroinfo *heroinfo, bool bHasSaveFile)
{
	memset(heroinfo, 0, sizeof(*heroinfo));
	strncpy(heroinfo->name, p->_pName, sizeof(heroinfo->name) - 1);
	heroinfo->name[sizeof(heroinfo->name) - 1] = '\0';
	heroinfo->level = p->_pLevel;
	heroinfo->heroclass = game_2_ui_class(p);
	heroinfo->strength = p->_pStrength;
	heroinfo->magic = p->_pMagic;
	heroinfo->dexterity = p->_pDexterity;
	heroinfo->vitality = p->_pVitality;
	heroinfo->gold = p->_pGold;
	heroinfo->hassaved = bHasSaveFile;
	heroinfo->herorank = p->pDiabloKillLevel;
	heroinfo->spawned = false;
}

uint8_t game_2_ui_class(const PlayerStruct *p)
{
	uint8_t uiclass;
	if (p->_pClass == PC_WARRIOR)
		uiclass = UI_WARRIOR;
	else if (p->_pClass == PC_ROGUE)
		uiclass = UI_ROGUE;
	else
		uiclass = UI_SORCERER;

	return uiclass;
}

bool pfile_ui_set_hero_infos(bool(*ui_add_hero_info)(_uiheroinfo *))
{
	DWORD i;
	bool showFixedMsg;

	memset(hero_names, 0, sizeof(hero_names));

	showFixedMsg = true;
	for (i = 0; i < MAX_CHARACTERS; i++) {
		PkPlayerStruct pkplr;
		HANDLE archive = pfile_open_save_archive(&showFixedMsg, i);
		if (archive) {
			if (pfile_read_hero(archive, &pkplr)) {
				_uiheroinfo uihero;
				strcpy(hero_names[i], pkplr.pName);
				UnPackPlayer(&pkplr, 0, false); // unpacks player 0 repeatedly... not sure if this should be fixed
				game_2_ui_player(&plr[0].data, &uihero, pfile_archive_contains_game(archive, i)); // unpacks player 0 repeatedly... not sure if this should be fixed
				ui_add_hero_info(&uihero);
			}
			pfile_SFileCloseArchive(archive);
		}
	}

	return true;
}

bool pfile_read_hero(HANDLE archive, PkPlayerStruct *pPack)
{
	HANDLE file;
	DWORD dwlen, nSize;
	uint8_t *buf;

	if (!SFileOpenFileEx(archive, "hero", 0, &file)) {
		return false;
	} else {
		bool ret = false;
		char password[16] = PASSWORD_SINGLE;
		nSize = 16;

		if (plr.isMultiplayer())
			strcpy(password, PASSWORD_MULTI);

		dwlen = SFileGetFileSize(file, NULL);
		if (dwlen) {
			DWORD read;
			buf = DiabloAllocPtr(dwlen);
			if (SFileReadFile(file, buf, dwlen, &read, NULL)) {
				read = codec_decode(buf, dwlen, password);
				if (read == sizeof(*pPack)) {
					memcpy(pPack, buf, sizeof(*pPack));
					ret = true;
				}
			}
			if (buf)
				mem_free_dbg(buf);
		}
		SFileCloseFile(file);
		return ret;
	}
}

/**
 * @param showFixedMsg Display a dialog if a save file was corrected (deprecated)
 */
HANDLE pfile_open_save_archive(bool *showFixedMsg, DWORD save_num)
{
	char SrcStr[MAX_PATH];
	HANDLE archive;

	pfile_get_save_path(SrcStr, sizeof(SrcStr), save_num);
	if (SFileOpenArchive(SrcStr, 0x7000, FS_PC, &archive))
		return archive;
	return NULL;
}

void pfile_SFileCloseArchive(HANDLE hsArchive)
{
	SFileCloseArchive(hsArchive);
}

bool pfile_archive_contains_game(HANDLE hsArchive, DWORD save_num)
{
	HANDLE file;

	if (plr.isMultiplayer())
		return false;

	if (!SFileOpenFileEx(hsArchive, "game", 0, &file))
		return false;

	SFileCloseFile(file);
	return true;
}

bool pfile_ui_set_class_stats(unsigned int player_class_nr, _uidefaultstats *class_stats)
{
	int c;

	c = pfile_get_player_class(player_class_nr);
	class_stats->strength = classes[c].strength;
	class_stats->magic = classes[c].magic;
	class_stats->dexterity = classes[c].dexterity;
	class_stats->vitality = classes[c].vitality;
	return true;
}

char pfile_get_player_class(unsigned int player_class_nr)
{
	char pc_class;

	if (player_class_nr == UI_WARRIOR)
		pc_class = PC_WARRIOR;
	else if (player_class_nr == UI_ROGUE)
		pc_class = PC_ROGUE;
	else
		pc_class = PC_SORCERER;
	return pc_class;
}

bool pfile_ui_save_create(_uiheroinfo *heroinfo)
{
	DWORD save_num;
	char cl;
	PkPlayerStruct pkplr;

	save_num = pfile_get_save_num_from_name(heroinfo->name);
	if (save_num == MAX_CHARACTERS) {
		for (save_num = 0; save_num < MAX_CHARACTERS; save_num++) {
			if (!hero_names[save_num][0])
				break;
		}
		if (save_num == MAX_CHARACTERS)
			return false;
	}
	if (!pfile_open_archive(false, save_num))
		return false;
	mpqapi_remove_hash_entries(pfile_get_file_name);
	strncpy(hero_names[save_num], heroinfo->name, PLR_NAME_LEN);
	hero_names[save_num][PLR_NAME_LEN - 1] = '\0';
	cl = pfile_get_player_class(heroinfo->heroclass);
	if (!plr.isValidPlayer(0)) plr.addPlayer(0);
	plr[0].CreatePlayer(cl);
	strncpy(plr[0].data._pName, heroinfo->name, PLR_NAME_LEN);
	plr[0].data._pName[PLR_NAME_LEN - 1] = '\0';
	PackPlayer(&pkplr, 0, true);
	pfile_encode_hero(&pkplr);
	game_2_ui_player(&plr[0].data, heroinfo, false);
	pfile_flush(true, save_num);
	return true;
}

bool pfile_get_file_name(DWORD lvl, char *dst)
{
	const char *fmt;

	if (plr.isMultiplayer()) {
		if (lvl)
			return false;
		fmt = "hero";
	} else {
		if (lvl < 17)
			fmt = "perml%02d";
		else if (lvl < 34) {
			lvl -= 17;
			fmt = "perms%02d";
		} else if (lvl == 34)
			fmt = "game";
		else if (lvl == 35)
			fmt = "hero";
		else
			return false;
	}
	sprintf(dst, fmt, lvl);
	return true;
}

bool pfile_delete_save(_uiheroinfo *hero_info)
{
	DWORD save_num;
	char FileName[MAX_PATH];

	save_num = pfile_get_save_num_from_name(hero_info->name);
	if (save_num < MAX_CHARACTERS) {
		hero_names[save_num][0] = '\0';
		pfile_get_save_path(FileName, sizeof(FileName), save_num);
		RemoveFile(FileName);
	}
	return true;
}

void pfile_read_player_from_save()
{
	HANDLE archive;
	DWORD save_num;
	PkPlayerStruct pkplr;

	save_num = pfile_get_save_num_from_name(gszHero);
	archive = pfile_open_save_archive(NULL, save_num);
	if (archive == NULL)
		app_fatal("Unable to open archive");
	if (!pfile_read_hero(archive, &pkplr))
		app_fatal("Unable to load character");

	UnPackPlayer(&pkplr, myplr(), false);
	gbValidSaveFile = pfile_archive_contains_game(archive, save_num);
	pfile_SFileCloseArchive(archive);
}

void GetTempLevelNames(char *szTemp)
{
	// BUGFIX: function call has no purpose
	pfile_get_save_num_from_name(myplr().data._pName);
	if (lvl.setlevel)
		sprintf(szTemp, "temps%02d", lvl.setlvlnum);
	else
		sprintf(szTemp, "templ%02d", lvl.currlevel);
}

void GetPermLevelNames(char *szPerm)
{
	DWORD save_num;
	bool has_file;

	save_num = pfile_get_save_num_from_name(myplr().data._pName);
	GetTempLevelNames(szPerm);
	if (!pfile_open_archive(false, save_num))
		app_fatal("Unable to read to save file archive");

	has_file = mpqapi_has_file(szPerm);
	pfile_flush(true, save_num);
	if (!has_file) {
		if (lvl.setlevel)
			sprintf(szPerm, "perms%02d", lvl.setlvlnum);
		else
			sprintf(szPerm, "perml%02d", lvl.currlevel);
	}
}

void pfile_get_game_name(char *dst)
{
	// BUGFIX: function call with no purpose
	pfile_get_save_num_from_name(myplr().data._pName);
	strcpy(dst, "game");
}

void pfile_remove_temp_files()
{
	if (plr.isSingleplayer()) {
		DWORD save_num = pfile_get_save_num_from_name(myplr().data._pName);
		if (!pfile_open_archive(false, save_num))
			app_fatal("Unable to write to save file archive");
		mpqapi_remove_hash_entries(GetTempSaveNames);
		pfile_flush(true, save_num);
	}
}

bool GetTempSaveNames(DWORD dwIndex, char *szTemp)
{
	const char *fmt;

	if (dwIndex < 17)
		fmt = "templ%02d";
	else if (dwIndex < 34) {
		dwIndex -= 17;
		fmt = "temps%02d";
	} else
		return false;

	sprintf(szTemp, fmt, dwIndex);
	return true;
}

void pfile_rename_temp_to_perm()
{
	DWORD dwChar, dwIndex;
	bool bResult;
	char szTemp[MAX_PATH];
	char szPerm[MAX_PATH];

	dwChar = pfile_get_save_num_from_name(myplr().data._pName);
	assert(dwChar < MAX_CHARACTERS);
	assert(plr.isSingleplayer());
	if (!pfile_open_archive(false, dwChar))
		app_fatal("Unable to write to save file archive");

	dwIndex = 0;
	while (GetTempSaveNames(dwIndex, szTemp)) {
		bResult = GetPermSaveNames(dwIndex, szPerm);
		assert(bResult);
		dwIndex++;
		if (mpqapi_has_file(szTemp)) {
			if (mpqapi_has_file(szPerm))
				mpqapi_remove_hash_entry(szPerm);
			mpqapi_rename(szTemp, szPerm);
		}
	}
	assert(! GetPermSaveNames(dwIndex,szPerm));
	pfile_flush(true, dwChar);
}

bool GetPermSaveNames(DWORD dwIndex, char *szPerm)
{
	const char *fmt;

	if (dwIndex < 17)
		fmt = "perml%02d";
	else if (dwIndex < 34) {
		dwIndex -= 17;
		fmt = "perms%02d";
	} else
		return false;

	sprintf(szPerm, fmt, dwIndex);
	return true;
}

void pfile_write_save_file(const char *pszName, uint8_t *pbData, DWORD dwLen, DWORD qwLen)
{
	DWORD save_num;
	char FileName[MAX_PATH];

	pfile_strcpy(FileName, pszName);
	save_num = pfile_get_save_num_from_name(myplr().data._pName);
	{
		char password[16] = PASSWORD_SINGLE;
		if (plr.isMultiplayer())
			strcpy(password, PASSWORD_MULTI);

		codec_encode(pbData, dwLen, qwLen, password);
	}
	if (!pfile_open_archive(false, save_num))
		app_fatal("Unable to write so save file archive");
	mpqapi_write_file(FileName, pbData, qwLen);
	pfile_flush(true, save_num);
}

void pfile_strcpy(char *dst, const char *src)
{
	strcpy(dst, src);
}

uint8_t *pfile_read(const char *pszName, DWORD *pdwLen)
{
	DWORD save_num, nread;
	char FileName[MAX_PATH];
	HANDLE archive, save;
	uint8_t *buf;

	pfile_strcpy(FileName, pszName);
	save_num = pfile_get_save_num_from_name(myplr().data._pName);
	archive = pfile_open_save_archive(NULL, save_num);
	if (archive == NULL)
		app_fatal("Unable to open save file archive");

	if (!SFileOpenFileEx(archive, FileName, 0, &save))
		app_fatal("Unable to open save file");

	*pdwLen = SFileGetFileSize(save, NULL);
	if (*pdwLen == 0)
		app_fatal("Invalid save file");

	buf = (uint8_t *)DiabloAllocPtr(*pdwLen);
	if (!SFileReadFile(save, buf, *pdwLen, &nread, NULL))
		app_fatal("Unable to read save file");
	SFileCloseFile(save);
	pfile_SFileCloseArchive(archive);

	{
		char password[16] = PASSWORD_SINGLE;
		DWORD nSize = 16;

		if (plr.isMultiplayer())
			strcpy(password, PASSWORD_MULTI);

		*pdwLen = codec_decode(buf, *pdwLen, password);
		if (*pdwLen == 0) {
			app_fatal("Invalid save file");
		}
	}
	return buf;
}

void pfile_update(bool force_save)
{
	// BUGFIX: these tick values should be treated as unsigned to handle overflows correctly
	static int save_prev_tc;

	if (plr.isMultiplayer()) {
		int tick = SDL_GetTicks();
		if (force_save || tick - save_prev_tc > 60000) {
			save_prev_tc = tick;
			pfile_write_hero();
		}
	}
}

DEVILUTION_END_NAMESPACE
