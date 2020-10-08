/**
 * @file interfac.h
 *
 * Interface of load screens.
 */
#ifndef __INTERFAC_H__
#define __INTERFAC_H__

namespace dvl {

extern int progress_id;

void interface_msg_pump();
bool IncProgress();
void DrawCutscene();
void DrawProgress(int screen_x, int screen_y, int progress_id);
void ShowProgress(unsigned int uMsg);
void FreeInterface();
void InitCutscene(unsigned int uMsg);

/* rdata */

extern const uint8_t BarColor[3];
extern const int BarPos[3][2];

}

#endif /* __INTERFAC_H__ */
