/**
 * @file movie.h
 *
 * Interface of video playback.
 */
#ifndef __MOVIE_H__
#define __MOVIE_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

extern uint8_t movie_playing;
extern bool loop_movie;

void play_movie(char *pszMovie, bool user_can_close);
LRESULT MovieWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

/* rdata */

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __MOVIE_H__ */
