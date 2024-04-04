#pragma once
#include <string>
#ifndef SDL
#include <SDL.h>
#endif // !SDL
#ifndef vector
#include <vector>
#endif
class Tile {
public:
	struct tile {
		std::string name;
		SDL_Texture* tex;
		std::vector<std::vector<int>> microCollision;
	};

	static std::vector<tile> masterTileSet;

	void init();
};