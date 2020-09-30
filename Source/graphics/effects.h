/**
 * @file effects.h
 *
 * Interface of functions for loading and playing sounds.
 */
#ifndef __EFFECTS_H__
#define __EFFECTS_H__

DEVILUTION_BEGIN_NAMESPACE

//#ifdef __cplusplus
//extern "C" {
//#endif

extern int sfxdelay;
extern int sfxdnum;
extern HANDLE sghStream;
extern TSFX *sgpStreamSFX;

bool effect_is_playing(int nSFX);
void stream_stop();
void InitMonsterSND(int monst);
void FreeMonsterSnd();
void PlayEffect(int i, int mode);
bool calc_snd_position(V2Di pos, int *plVolume, int *plPan);
void PlaySFX(int psfx);
void PlaySFX_priv(TSFX *pSFX, bool loc, V2Di pos);
void stream_play(TSFX *pSFX, int lVolume, int lPan);
int RndSFX(int psfx);
void PlaySfxLoc(int psfx, V2Di pos);
void sound_stop();
void sfx_stop();
void sound_update();
void stream_update();
void effects_cleanup_sfx();
void sound_init();
void priv_sound_init(uint8_t bLoadMask);
void ui_sound_init();
void effects_play_sound(char *snd_file);

/* rdata */

extern const char MonstSndChar[];

/* data */
//#ifdef __cplusplus
//}
//#endif

DEVILUTION_END_NAMESPACE

#endif /* __EFFECTS_H__ */
