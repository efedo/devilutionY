//HEADER_GOES_HERE
#ifndef __DIABLOUI_H__
#define __DIABLOUI_H__

namespace dvl {

void UiDestroy();
void UiTitleDialog();
void UiSetSpawned(bool bSpawned);
void UiInitialize();
bool UiValidPlayerName(char *name); /* check */
bool UiSelHeroMultDialog(bool(*fninfo)(bool(*fninfofunc)(_uiheroinfo *)), bool(*fncreate)(_uiheroinfo *), bool(*fnremove)(_uiheroinfo *), bool(*fnstats)(unsigned int, _uidefaultstats *), int *dlgresult, bool *hero_is_created, char *name);
bool UiSelHeroSingDialog(bool(*fninfo)(bool(*fninfofunc)(_uiheroinfo *)), bool(*fncreate)(_uiheroinfo *), bool(*fnremove)(_uiheroinfo *), bool(*fnstats)(unsigned int, _uidefaultstats *), int *dlgresult, char *name, int *difficulty);
bool UiCreditsDialog(int a1);
bool UiMainMenuDialog(char *name, int *pdwResult, void(*fnSound)(char *file), int attractTimeOut);
bool UiProgressDialog(char *msg, int enable, int(*fnfunc)(), int rate);
const char **UiProfileGetString();
void UiProfileCallback();
void UiProfileDraw();
bool UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, DWORD *a6, DWORD *a7);
bool UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5);
bool UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, char *lpBuffer, int cchBufferMax);
bool UiSoundCallback(int a1, int type, int a3);
void UiMessageBoxCallback(HWND hWnd, char *lpText, const char *lpCaption, UINT uType);
bool UiDrawDescCallback(int game_type, DWORD color, const char *lpString, char *a4, int a5, UINT align, time_t a7, HDC *a8);
bool UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6);
bool UiArtCallback(int game_type, unsigned int art_code, SDL_Color *pPalette, uint8_t *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp);
int UiSelectGame(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *a6);
int UiSelectProvider(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *type);
bool UiCreatePlayerDescription(_uiheroinfo *info, DWORD mode, char *desc);
void UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, DWORD type);
}

#endif /* __DIABLOUI_H__ */
