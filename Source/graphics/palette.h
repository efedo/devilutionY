//HEADER_GOES_HERE
#ifndef __PALETTE_H__
#define __PALETTE_H__

namespace dvl {

enum class Duntype;

extern SDL_Color logical_palette[256];
extern SDL_Color system_palette[256];
extern SDL_Color orig_palette[256];

void palette_update();
void SaveGamma();
void palette_init();
void LoadPalette(char *pszFileName);
void LoadRndLvlPal(DunType l);
void ResetPal();
void IncreaseGamma();
void ApplyGamma(SDL_Color *dst, const SDL_Color *src, int n);
void DecreaseGamma();
int UpdateGamma(int gamma);
void BlackPalette();
void SetFadeLevel(DWORD fadeval);
void PaletteFadeIn(int fr);
void PaletteFadeOut(int fr);
void palette_update_caves();
void palette_update_quest_palette(int n);
bool palette_get_color_cycling();
bool palette_set_color_cycling(bool enabled);

extern int gamma_correction;
extern bool color_cycling_enabled;

}

#endif /* __PALETTE_H__ */
