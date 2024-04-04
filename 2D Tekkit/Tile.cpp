#include "Tile.h"
#include "Artist.h"

std::vector<Tile::tile> Tile::masterTileSet;


std::vector<std::vector<int>> getMicroCollision(int type)
{
	std::vector<std::vector<int>> tempCollision;

	//set size too 64x64
	tempCollision.resize(64);
	for (int i = 0; i < tempCollision.size(); i++)
	{
		tempCollision[i].resize(64);
	}

	
	for (int x = 0; x < tempCollision.size(); x++)
	{
		for (int y = 0; y < tempCollision.size(); y++)
		{
			tempCollision[x][y] = 1;
		}
	}

	switch (type)
	{
		//empty
	case -1:
		for (int x = 0; x < tempCollision.size(); x++)
		{
			for (int y = 0; y < tempCollision.size(); y++)
			{
				tempCollision[x][y] = 0;
			}
		}
		break;
		//solid
	case 0:
		return tempCollision;
		break;
	case 1:
		//slope bottem right corner
		for (int y = 0; y < 64; y++)
		{
			for (int x = 0; x < 63 - y; x++)
			{
				tempCollision[x][y] = 0;
			}
		}
		
		
		return tempCollision;
		break;
	case 2:
		//slope bottem left corner
		for (int y = 0; y < 64; y++)
		{
			for (int x = 0; x < 63 - y; x++)
			{
				tempCollision[x + 1 + y][y] = 0;
			}
		}
		


		return tempCollision;
		break;
	case 3:
		// hole in the center
		for (int y = 0; y < 64; y++)
		{
			for (int x = 0; x < 32; x++)
			{
				tempCollision[x + 16][y] = 0;
			}
		}
		return tempCollision;
		break;
	}
	


	return tempCollision;
}

void loadTile(std::string name, std::string texName, int collisionType)
{
	Artist artist;

	//create temp tile
	Tile::tile tempTile;

	//set perams
	tempTile.name = name;
	tempTile.tex = artist.loadTexture("Resource/tiles/" + texName + ".png");
	tempTile.microCollision = getMicroCollision(collisionType);

	//add tile too list
	Tile::masterTileSet.push_back(tempTile);
}

void Tile::init()
{
	loadTile("Air", "air", -1);
	loadTile("Stone", "stone", 0);
	loadTile("Dirt", "dirt", 0);
	loadTile("Grass", "grass", 0);
	loadTile("Coal", "coal", 0);
	loadTile("Iron", "iron", 0);
	loadTile("Diamond", "diamond", 0);
	//slope test
	loadTile("SlopeTest", "slope", 1);
	loadTile("SlopeTest", "slopeFlip", 2);
	//weird stuff test
	loadTile("HoleTest", "holeTileTest", 3);

	//temp
	//loadTile("Me", "Me", 0);
}