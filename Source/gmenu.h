/**
 * @file gmenu.h
 *
 * Interface of the in-game navigation and interaction.
 */
#ifndef __GMENU_H__
#define __GMENU_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

extern uint8_t *optbar_cel;
extern bool mouseNavigation;
extern uint8_t *PentSpin_cel;
extern uint8_t *BigTGold_cel;
extern int dword_634474;
extern char byte_634478;
extern void (*dword_63447C)(TMenuItem *);
extern TMenuItem *sgpCurrentMenu;
extern uint8_t *option_cel;
extern int sgCurrentMenuIdx;

void gmenu_draw_pause();
void gmenu_print_text(int x, int y, char *pszStr);
void FreeGMenu();
void gmenu_init_menu();
bool gmenu_is_active();
void gmenu_set_items(TMenuItem *pItem, void (*gmFunc)(TMenuItem *));
void gmenu_up_down(bool isDown);
void gmenu_draw();
void gmenu_draw_menu_item(TMenuItem *pItem, int y);
void gmenu_clear_buffer(int x, int y, int width, int height);
int gmenu_get_lfont(TMenuItem *pItem);
bool gmenu_presskeys(int vkey);
void gmenu_left_right(bool isRight);
bool gmenu_on_mouse_move();
bool gmenu_get_mouse_slider(int *plOffset);
bool gmenu_left_mouse(bool isDown);
void gmenu_enable(TMenuItem *pMenuItem, bool enable);
void gmenu_slider_set(TMenuItem *pItem, int min, int max, int gamma);
int gmenu_slider_get(TMenuItem *pItem, int min, int max);
void gmenu_slider_steps(TMenuItem *pItem, int dwTicks);

/* rdata */

extern const uint8_t lfontframe[];
extern const uint8_t lfontkern[];

//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __GMENU_H__ */
