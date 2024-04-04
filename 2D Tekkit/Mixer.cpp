#include <SDL_mixer.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Mixer.h"



//Declare sounds in a header -> define them in that headers respective file (maybe here) -> load em in the load media funcion in the mixer file
//how to load a sound
//Mix_Chunk* Class::sSoundTest = NULL;
//how to load music
//Mix_Music* Class::sMusicTest = NULL;



bool Mixer::loadSoundMedia()
{
	bool success = true;

	//Declare sounds in a header -> define them in that headers respective file -> load em in here
	//how to load a sound
	//Class::sTestSound = Mix_LoadWAV("Resource/sound/soundFX/testSound.wav");
	//how to load music
	//Class::sTestMusic = Mix_LoadMUS("Resource/sound/music/sestMusic.wav");

	//seperate by class for good organization
	//write load functions here \/

	


	return success;
}