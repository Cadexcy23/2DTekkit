#include "Map.h"
#include "Tile.h"
#include "Wall.h"
#include "Artist.h"
#include "Controller.h"
#include "Entity.h"


std::vector<std::vector<Tile::tile*>> Map::tileMap;
std::vector<std::vector<Wall::wall*>> Map::wallMap;

int Map::tileSize = 64;
int tileSizeCurrent = 64;
Artist::pos Map::cameraPOS = { Artist::SCREEN_WIDTH / 2, Artist::SCREEN_HEIGHT / 2 };
float topLeftOffsetX = 0;
float topLeftOffsetY = 0;
Artist::pos Map::topLeftPOS = { cameraPOS.x, cameraPOS.y };
Artist::pos Map::mouseInGamePos = { 0,0 };
//used for makinn panning work how it should (chnges speed based on zoom)
float camChangeX;
float camChangeY;
float Map::mapZoom;
//change map size
Artist::pos mapSize = { 300, 300 };

SDL_Texture* Map::background = NULL;


void Map::init()
{
	//temp
	srand(2);
	//srand(clock());
	
	//tiles
	Map::tileMap.resize(mapSize.x);
	for (int i = 0; i < mapSize.x; i++)
	{
		Map::tileMap[i].resize(mapSize.y);
	}

	for (int x = 0; x < Map::tileMap.size(); x++)
	{
		for (int y = 0; y < Map::tileMap[0].size(); y++)
		{
			Map::tileMap[x][y] = &Tile::masterTileSet[rand() % (Tile::masterTileSet.size() - 0) + 0];
		}
	}
	// clear to rows
	/*for (int x = 0; x < Map::tileMap.size(); x++)
	{
		for (int y = 0; y < 10; y++)
		{
			Map::tileMap[x][y] = &Tile::masterTileSet[0];
		}
	}*/
	for (int x = 0; x < Map::tileMap.size(); x++)
	{
		for (int y = 2; y < 4; y++)
		{
			Map::tileMap[x][y] = &Tile::masterTileSet[0];
		}
	}
	for (int y = 1; y < Map::tileMap.size() - 1; y++)
	{
		for (int x = 2; x < 4; x++)
		{
			Map::tileMap[x][y] = &Tile::masterTileSet[0];
		}
	}
	//TEMP tiles sets
	

	//walls
	Map::wallMap.resize(mapSize.x);
	for (int i = 0; i < mapSize.x; i++)
	{
		Map::wallMap[i].resize(mapSize.y);
	}

	for (int x = 0; x < Map::wallMap.size(); x++)
	{
		for (int y = 0; y < Map::wallMap[0].size(); y++)
		{
			Map::wallMap[x][y] = &Wall::masterWallSet[rand() % (Wall::masterWallSet.size() - 0) + 0];
		}
	}
	// clear top rows
	for (int x = 0; x < Map::wallMap.size(); x++)
	{
		for (int y = 0; y < 10; y++)
		{
			Map::wallMap[x][y] = &Wall::masterWallSet[0];
		}
	}
}

void updateZoom()
{
	if (Controller::FPSLock)
	{

		//find if we are zooming in or out (or nothing)
		int direction;
		if (tileSizeCurrent < Map::tileSize)
		{
			direction = 1;
		}
		else if (tileSizeCurrent > Map::tileSize)
		{
			direction = -1;
		}
		else
		{
			direction = 0;
		}

		//scale up the speed of zooming if its a large change
		int speedScale = 1;
		if (abs(tileSizeCurrent - Map::tileSize) > 50)
		{
			speedScale = 3;
		}
		else if (abs(tileSizeCurrent - Map::tileSize) > 25)
		{
			speedScale = 2;
		}
		else
		{
			speedScale = 1;
		}

		//actualy do that zooming
		tileSizeCurrent += speedScale * direction;

		//adjust so the screen is centered
		Map::mapZoom = float(tileSizeCurrent) / 64;
		Map::topLeftPOS.x = Map::cameraPOS.x * Map::mapZoom - Artist::SCREEN_WIDTH / 2;
		Map::topLeftPOS.y = Map::cameraPOS.y * Map::mapZoom - Artist::SCREEN_HEIGHT / 2;
		//keep track of where the mouse is in in-game units
		Map::mouseInGamePos = { int((Map::topLeftPOS.x + Controller::mouseX) / (Map::tileSize / 64.0)) , int((Map::topLeftPOS.y + Controller::mouseY) / (Map::tileSize / 64.0)) };
	
	}
}

void drawTiles()
{
	Artist artist;
	
	Artist::pos tilesToDraw;
	tilesToDraw.x = Artist::SCREEN_WIDTH / tileSizeCurrent + 2;
	tilesToDraw.y = Artist::SCREEN_HEIGHT / tileSizeCurrent + 2;


	for (int x = 0; x < tilesToDraw.x; x++)
	{
		for (int y = 0; y < tilesToDraw.y; y++)
		{
			Artist::pos tileToDraw;
			tileToDraw.x = x + Map::topLeftPOS.x / tileSizeCurrent;
			tileToDraw.y = y + Map::topLeftPOS.y / tileSizeCurrent;

			Artist::pos tileDrawPos;
			tileDrawPos.x = x * tileSizeCurrent - Map::topLeftPOS.x % tileSizeCurrent;
			tileDrawPos.y = y * tileSizeCurrent - Map::topLeftPOS.y % tileSizeCurrent;

			//make sure we dont draw out of map
			if (tileToDraw.x < Map::tileMap.size() && tileToDraw.x >= 0 && tileToDraw.y < Map::tileMap[0].size() && tileToDraw.y >= 0)
			{
				artist.drawImage(Map::tileMap[tileToDraw.x][tileToDraw.y]->tex, tileDrawPos.x, tileDrawPos.y, tileSizeCurrent, tileSizeCurrent);
			}
		}
	}

	//print debug info in the top left
	//artist.drawLetters("Cam X:" + std::to_string(Map::cameraPOS.x) + " Y:" + std::to_string(Map::cameraPOS.y), 0, 30, Artist::smallFont);
	artist.drawLetters("CorrectedCam X:" + std::to_string(Map::topLeftPOS.x) + " Y:" + std::to_string(Map::topLeftPOS.y), 0, 60, Artist::smallFont);
	//artist.drawLetters("Tile Size:" + std::to_string(tileSizeCurrent) + "Goal:" + std::to_string(Map::tileSize), 0, 90, Artist::smallFont);
	//artist.drawLetters("Cam Change X:" + std::to_string(camChangeX) + "Cam Change Y:" + std::to_string(camChangeX), 0, 120, Artist::smallFont);
	artist.drawLetters("Player X:" + std::to_string(Entity::entityList[Entity::player].x) + " Y:" + std::to_string(Entity::entityList[Entity::player].y), 0, 150, Artist::smallFont);
	artist.drawLetters("Player Vel X:" + std::to_string(Entity::entityList[Entity::player].xVel) + " Y:" + std::to_string(Entity::entityList[Entity::player].yVel), 0, 180, Artist::smallFont);
	//artist.drawLetters("Player Move Time Y:" + std::to_string(Entity::entityList[Entity::player].movementTimeY), 0, 210, Artist::smallFont);
	//artist.drawLetters("Player Move Start Time Y:" + std::to_string(Entity::entityList[Entity::player].movementTimeYStart), 0, 240, Artist::smallFont);
}	

void drawWalls()
{
	Artist artist;

	Artist::pos wallsToDraw;
	wallsToDraw.x = Artist::SCREEN_WIDTH / tileSizeCurrent + 2;
	wallsToDraw.y = Artist::SCREEN_HEIGHT / tileSizeCurrent + 2;


	for (int x = 0; x < wallsToDraw.x; x++)
	{
		for (int y = 0; y < wallsToDraw.y; y++)
		{
			Artist::pos wallToDraw;
			wallToDraw.x = x + Map::topLeftPOS.x / tileSizeCurrent;
			wallToDraw.y = y + Map::topLeftPOS.y / tileSizeCurrent;

			Artist::pos wallDrawPos;
			wallDrawPos.x = x * tileSizeCurrent - Map::topLeftPOS.x % tileSizeCurrent;
			wallDrawPos.y = y * tileSizeCurrent - Map::topLeftPOS.y % tileSizeCurrent;

			//make sure we dont draw out of map
			if (wallToDraw.x < Map::tileMap.size() && wallToDraw.x >= 0 && wallToDraw.y < Map::tileMap[0].size() && wallToDraw.y >= 0)
			{
			artist.drawImage(Map::wallMap[wallToDraw.x][wallToDraw.y]->tex, wallDrawPos.x, wallDrawPos.y, tileSizeCurrent, tileSizeCurrent);
			}
		}
	}
}

void drawBG()
{
	Artist artist;

	int offset = (Map::cameraPOS.x * -1) / 4 % Artist::SCREEN_WIDTH;

	artist.drawImage(Map::background, offset + Artist::SCREEN_WIDTH, 0, Artist::SCREEN_WIDTH, Artist::SCREEN_HEIGHT);
	artist.drawImage(Map::background, offset, 0, Artist::SCREEN_WIDTH, Artist::SCREEN_HEIGHT);
}

void Map::draw()
{
	updateZoom();

	drawBG();

	drawWalls();
	drawTiles();

	
}

void controlMap()
{
	if (Controller::mouseWheelMovment.y == 1)
	{
		Map::tileSize += 1;
		//if (Map::tileSize > 128)
		//{
		//	Map::tileSize = 128;
		//}
		if (Map::tileSize > 256)
		{
			Map::tileSize = 256;
		}
	}
	if (Controller::mouseWheelMovment.y == -1)
	{
		Map::tileSize -= 1;
		if (Map::tileSize < 32)
		{
			Map::tileSize = 32;
		}
	}
	if (Controller::mouseStates[0] == 2)
	{
		Artist::pos mouseMovment = { Controller::lastMousePos.x - Controller::mouseX, Controller::lastMousePos.y - Controller::mouseY };

		float mapZoom = float(tileSizeCurrent) / 64;

		camChangeX += mouseMovment.x / mapZoom;
		camChangeY += mouseMovment.y / mapZoom;

		if (abs(camChangeX) >= 1)
		{
			Map::cameraPOS.x += int(camChangeX);
			camChangeX -= int(camChangeX);
		}
		 
		if (abs(camChangeY) >= 1)
		{
			Map::cameraPOS.y += int(camChangeY);
			camChangeY -= int(camChangeY);
		}
	}
	
}

void Map::controller()
{
	controlMap();
}