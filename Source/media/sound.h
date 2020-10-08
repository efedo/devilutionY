//HEADER_GOES_HERE
#ifndef __SOUND_H__
#define __SOUND_H__

namespace dvl {

//#ifdef __cplusplus
//extern "C" {
//#endif

extern SoundSample *DSBs[8];
extern bool gbSndInited;
extern HMODULE hDsound_dll;

void snd_update(bool bStopAll);
void snd_stop_snd(TSnd *pSnd);
bool snd_playing(TSnd *pSnd);
void snd_play_snd(TSnd *pSnd, int lVolume, int lPan);
SoundSample *sound_dup_channel(SoundSample *DSB);
bool sound_file_reload(TSnd *sound_file, SoundSample *DSB);
TSnd *sound_file_load(char *path);
void sound_CreateSoundBuffer(TSnd *sound_file);
void sound_file_cleanup(TSnd *sound_file);
void snd_init(HWND hWnd);
void snd_get_volume(char *value_name, int *value);
void sound_create_primary_buffer(HANDLE music_track);
void sound_cleanup();
void snd_set_volume(char *key, int value);
void music_stop();
void music_start(int nTrack);
void sound_disable_music(bool disable);
int sound_get_or_set_music_volume(int volume);
int sound_get_or_set_sound_volume(int volume);

/* rdata */

/* data */

extern bool gbMusicOn;
extern bool gbSoundOn;
extern bool gbDupSounds;
extern char unk_volume[4][2];

//#ifdef __cplusplus
//}
//#endif

}

#endif /* __SOUND_H__ */
