#pragma once
#include "Artist.h"
#ifndef SDL
#include <SDL.h>
#endif // !SDL
#ifndef vector
#include <vector>
#endif
#include "Map.h"

class Projectile {
public:
	struct projectile {
		std::string name;
		float x;
		float y;
		int width;
		int height;
		int movementTimeY;
		int movementTimeYStart;
		bool facingRight;
		float gravity;
		float mass;
		float knockBackY;
		float knockBackX;
		float xVel;
		float yVel;
		int drawFrame = 0;
		int state;
		float spin;
		float rotation;
		int creationTime;
		int duration;
		//add friction/airresist
		//add function slots for when you hit walls, tiles, entities and so on
		std::vector<std::vector<SDL_Texture*>> animationSets;
		
		std::vector<void (*)(int)> updateList;

		void draw()
		{
			Artist artist;

			//find the size of the texture so we can draw it from the center properly
			Artist::pos drawSize;
			SDL_QueryTexture(animationSets[state][0], NULL, NULL, &drawSize.x, &drawSize.y);
			Artist::pos drawOffset;
			drawOffset.x = drawSize.x / 2;
			drawOffset.y = drawSize.y;
			//get the screen pos of where to draw the image
			Artist::pos screenPos = { (x - drawOffset.x) * Map::mapZoom - Map::topLeftPOS.x , (y - drawOffset.y) * Map::mapZoom - Map::topLeftPOS.y };
			//use the vars to draw it
			if (facingRight)
			{
				artist.drawImage(animationSets[state][drawFrame], screenPos.x , screenPos.y , drawSize.x * Map::mapZoom, drawSize.y * Map::mapZoom, rotation, NULL, SDL_FLIP_NONE);
				//artist.drawAnimation(animationSets[state], (x - drawOffset.x) * Map::mapZoom - Map::topLeftPOS.x, (y - drawOffset.y) * Map::mapZoom - Map::topLeftPOS.y, 33.33, animationStartTime, drawSize.x * Map::mapZoom, drawSize.y * Map::mapZoom, 0, SDL_FLIP_NONE);
			}
			else
			{
				artist.drawImage(animationSets[state][drawFrame], (x - drawOffset.x) * Map::mapZoom - Map::topLeftPOS.x, (y - drawOffset.y) * Map::mapZoom - Map::topLeftPOS.y, drawSize.x * Map::mapZoom, drawSize.y * Map::mapZoom, rotation, NULL, SDL_FLIP_HORIZONTAL);
				//artist.drawAnimation(animationSets[state], (x - drawOffset.x) * Map::mapZoom - Map::topLeftPOS.x, (y - drawOffset.y) * Map::mapZoom - Map::topLeftPOS.y, 33.33, animationStartTime, drawSize.x * Map::mapZoom, drawSize.y * Map::mapZoom, 0, SDL_FLIP_HORIZONTAL);
			}
		}

		bool tileCollisionCheck(int direction, Artist::pos* collisionPos)
		{
			enum directions {
				UP,
				DOWN,
				LEFT,
				RIGHT
			};

			//make it basicaly a copy of this func for entities BUT MAKE A TILE COLLISION DTECTOR IN THE TILE FILE THAT LOOKS AT A SPECIFIC POS OR ROWS/COLLOMS TO MAKE LIFE EASIER
			bool returnFlag = false;

			// end early if arnt moving in that direction
			if (xVel > 0 && direction == LEFT)
			{
				return false;
			}
			else if (xVel < 0 && direction == RIGHT)
			{
				return false;
			}
			if (yVel > 0 && direction == UP)
			{
				return false;
			}
			else if (yVel < 0 && direction == DOWN)
			{
				return false;
			}

			//add a solid 1 to the check every time if its over 0 but under 1
			float velAddCheckX = xVel;
			float velAddCheckY = yVel;//LOOK INTO REMOVING THIS
			if (xVel > 0 && xVel < 1)
			{
				velAddCheckX = 1;
			}
			if (yVel > 0 && yVel < 1)
			{
				velAddCheckY = 1;
			}
			if (xVel < 0 && xVel > -1)
			{
				velAddCheckX = -1;
			}
			if (yVel < 0 && yVel > -1)
			{
				velAddCheckY = -1;
			}

			//UP
			if (direction == UP)
			{
				for (int j = 0; j > velAddCheckY - 1; j--)
				{
					for (int i = 0; i < width; i++)
					{
						//get new pos
						Artist::pos newPos = { x - width / 2 + i, y - height + j };
						Artist::pos newGridPos = { newPos.x / 64 , newPos.y / 64 };

						//get tile we are to look at
						Tile::tile* currentTile = Map::tileMap[newGridPos.x][newGridPos.y];

						//get the spot in the tile micro collision to check
						Artist::pos microPos = { newPos.x % 64, newPos.y % 64 };

						//check if the unit we are going to be at has collision
						if (currentTile->microCollision[microPos.x][microPos.y] == 1)
						{
							*collisionPos = newPos;
							returnFlag = true;
							//end check
							i = width;
							j = velAddCheckY - 1;
						}
					}
				}
			}

			//DOWN
			if (direction == DOWN)
			{
				for (int j = 0; j < velAddCheckY + 1; j++)
				{
					for (int i = 0; i < width; i++)
					{
						//get new pos
						Artist::pos newPos = { x - width / 2 + i, y + j };
						Artist::pos newGridPos = { newPos.x / 64 , newPos.y / 64 };

						//get tile we are to look at
						Tile::tile* currentTile = Map::tileMap[newGridPos.x][newGridPos.y];

						//get the spot in the tile micro collision to check
						Artist::pos microPos = { newPos.x % 64, newPos.y % 64 };

						//check if the unit we are going to be at has collision
						if (currentTile->microCollision[microPos.x][microPos.y] == 1)
						{
							*collisionPos = newPos;
							returnFlag = true;
							//end check
							i = width;
							j = velAddCheckY + 1;
						}
					}
				}
			}

			//LEFT
			if (direction == LEFT)
			{
				for (int j = 0; j > velAddCheckX - 1; j--)
				{
					for (int i = 0; i < height; i++)
					{
						//get new pos
						Artist::pos newPos = { x - width / 2 + j , y - height + i };
						Artist::pos newGridPos = { newPos.x / 64 , newPos.y / 64 };

						//get tile we are to look at
						Tile::tile* currentTile = Map::tileMap[newGridPos.x][newGridPos.y];

						//get the spot in the tile micro collision to check
						Artist::pos microPos = { newPos.x % 64, newPos.y % 64 };

						//check if the unit we are going to be at has collision

if (currentTile->microCollision[microPos.x][microPos.y] == 1)
						{
							*collisionPos = newPos;
							returnFlag = true;
							//end check
							i = height;
							j = velAddCheckX - 1;
						}
					}
				}
			}

			//RIGHT
			if (direction == RIGHT)
			{
				for (int j = 0; j < velAddCheckX + 1; j++)
				{
					for (int i = 0; i < height; i++)
					{
						//get new pos
						Artist::pos newPos = { x + width / 2 + j, y - height + i };
						Artist::pos newGridPos = { newPos.x / 64 , newPos.y / 64 };

						//get tile we are to look at
						Tile::tile* currentTile = Map::tileMap[newGridPos.x][newGridPos.y];

						//get the spot in the tile micro collision to check
						Artist::pos microPos = { newPos.x % 64, newPos.y % 64 };

						//check if the unit we are going to be at has collision
						if (currentTile->microCollision[microPos.x][microPos.y] == 1)
						{
							*collisionPos = newPos;
							returnFlag = true;
							//end check
							i = height;
							j = velAddCheckX + 1;
						}
					}
				}
			}



			return returnFlag;
		}
	};

	


	//list of all active entities
	static std::vector<projectile> projectileList;


	void loadProjectile(std::string name, int x, int y, int width, int height, std::string texName, std::vector<int> frameCounts, std::vector<void (*)(int)> updateList, float gravity, float mass, float xVel, float yVel, float spin, float rotation, int duration);

	void unloadProjectile(int proID);

	void getVelocityToTarget(Artist::pos startPos, Artist::pos targetPos, float totalVel, float* xReturnVel, float* yReturnVel);

	void init();

	void updateProjectiles();

	void drawProjectiles();

	
};