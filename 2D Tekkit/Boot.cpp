#include <stdio.h>
#include "Artist.h"
#include "Controller.h"
#include "Tile.h"
#include "Wall.h"
#include "Map.h"
#include "Entity.h"
#include "Projectile.h"

int main(int argc, char* args[])
{
	//Declare Artist and controller
	Artist artist;
	Controller controller;
	Tile tile;
	Wall wall;
	Map map;
	Entity entity;
	Projectile projectile;
	

	//Start up SDL and create window
	if (!artist.init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!artist.loadMedia())
		{
			printf("Failed to load media!\n");
		}
		
		
		//loading setting ect
		controller.loadController();
		tile.init();
		wall.init();
		map.init();
		entity.init();
		projectile.init();


		//While application is running
		while (!Controller::quit)
		{
			//Clear screen
			artist.clearScreen();


			//Checks for user input
			controller.controller();

			//Updates game world
			entity.updateEntities();
			projectile.updateProjectiles();

			//Draw everything
			artist.draw();

			




			//Update screen
			artist.updateScreen();
		}
		
	}

	//Free resources and close SDL
	artist.close();

	return 0;
}