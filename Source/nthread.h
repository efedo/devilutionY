/**
 * @file nthread.h
 *
 * Interface of functions for managing game ticks.
 */
#ifndef __NTHREAD_H__
#define __NTHREAD_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

extern uint8_t sgbNetUpdateRate;
extern DWORD gdwMsgLenTbl[MAX_PLRS];
extern DWORD gdwDeltaBytesSec;
extern bool nthread_should_run;
extern DWORD gdwTurnsInTransit;
extern uintptr_t glpMsgTbl[MAX_PLRS];
extern SDL_threadID glpNThreadId;
extern int turn_upper_bit;
extern bool sgbThreadIsRunning;
extern DWORD gdwLargestMsgSize;
extern DWORD gdwNormalMsgSize;
extern int last_tick;

void nthread_terminate_game(const char *pszFcn);
DWORD nthread_send_and_recv_turn(DWORD cur_turn, int turn_delta);
bool nthread_recv_turns(bool *pfSendAsync);
void nthread_set_turn_upper_bit();
void nthread_start(bool set_turn_upper_bit);
unsigned int nthread_handler(void *data);
void nthread_cleanup();
void nthread_ignore_mutex(bool bStart);
bool nthread_has_500ms_passed(bool unused);

/* rdata */
//#ifdef __cplusplus
//}
//#endif

}

#endif /* __NTHREAD_H__ */
