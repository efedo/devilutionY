#ifndef __RENDER_H__
#define __RENDER_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

void RenderTile(BYTE *pBuff, int level_piece_id);
void world_draw_black_tile(int sx, int sy);
void world_draw_red_tile(int sx, int sy);
void world_draw_color_tile(int sx, int sy, const uint8_t color);
void trans_rect(int sx, int sy, int width, int height);

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __RENDER_H__ */
