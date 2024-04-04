#include "Artist.h"
#include "Wall.h"

std::vector<Wall::wall> Wall::masterWallSet;


void loadWall(std::string name, std::string texName)
{
	Artist artist;

	Wall::wall tempWall;

	tempWall.name = name;
	tempWall.tex = artist.loadTexture("Resource/walls/" + texName + ".png");

	Wall::masterWallSet.push_back(tempWall);
}

void Wall::init()
{
	loadWall("Air", "air");
	loadWall("Stone", "stone");
	loadWall("Dirt", "dirt");
}