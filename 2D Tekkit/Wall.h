#pragma once
#include <string>
#ifndef SDL
#include <SDL.h>
#endif // !SDL
#ifndef vector
#include <vector>
#endif
class Wall {
public:
	struct wall {
		std::string name;
		SDL_Texture* tex;
	};

	static std::vector<wall> masterWallSet;

	void init();
};