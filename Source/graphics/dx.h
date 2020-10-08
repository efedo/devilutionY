/**
 * @file dx.h
 *
 * Interface of functions setting up the graphics pipeline.
 */
#ifndef __DX_H__
#define __DX_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

extern uint8_t *gpBuffer;

void dx_init(HWND hWnd);
void lock_buf(uint8_t idx);
void unlock_buf(uint8_t idx);
void dx_cleanup();
void dx_reinit();

void InitPalette();
void BltFast(SDL_Rect *src_rect, SDL_Rect *dst_rect);
void Blit(SDL_Surface *src, SDL_Rect *src_rect, SDL_Rect *dst_rect);
void RenderPresent();
void PaletteGetEntries(DWORD dwNumEntries, SDL_Color *lpEntries);
void PaletteSetEntries(DWORD dwCount, SDL_Color *lpEntries);

/* data */
//#ifdef __cplusplus
//}
//#endif

}

#endif /* __DX_H__ */
