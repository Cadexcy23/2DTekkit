#pragma once
#ifndef SDL_mixer
#include <SDL_mixer.h>
#endif

class Mixer {
public:

	//Declare sounds in a header (maybe here) -> define them in that headers respective file -> load em in the load media funcion in the mixer file
	//how to declare a sound
	//static Mix_Chunk* sTestSound;
	//how to declare music
	//static Mix_Music* sTestMusic;


	bool loadSoundMedia();

	
};
