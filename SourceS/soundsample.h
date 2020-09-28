#pragma once
#include <SDL_mixer.h>

namespace dvl {

typedef struct SoundSample final {
public:
	void Release() ;
	bool IsPlaying() ;
	void Play(int lVolume, int lPan) ;
	void Stop() ;
	int SetChunk(uint8_t *fileData, DWORD dwBytes) ;

private:
	Mix_Chunk *chunk;
} SoundSample;

} // namespace dvl
