#pragma once
#ifndef vector
#include <vector>
#endif
#include "Tile.h"
#include "Wall.h"
#ifndef SDL
#include <SDL.h>
#endif // !SDL
#include "Artist.h"
class Map {
public:
	static std::vector<std::vector<Tile::tile*>> tileMap;
	static std::vector<std::vector<Wall::wall*>> wallMap;
	static int tileSize;
	static SDL_Texture* background;
	static float mapZoom;
	static Artist::pos cameraPOS;
	static Artist::pos topLeftPOS;
	static Artist::pos mouseInGamePos;
	void init();
	void draw();
	void controller();
};