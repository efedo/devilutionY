#ifndef __RENDER_H__
#define __RENDER_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

void RenderTile(uint8_t *pBuff, int level_piece_id, int trans, int foliage);
void world_draw_black_tile(int sx, int sy);
void world_draw_red_tile(int sx, int sy);
void world_draw_gray_tile(int sx, int sy);
void world_draw_color_tile(int sx, int sy, const uint8_t color);
void trans_rect(int sx, int sy, int width, int height);

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __RENDER_H__ */
