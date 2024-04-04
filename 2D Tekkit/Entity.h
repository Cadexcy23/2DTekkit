#pragma once
#include "Artist.h"
#ifndef SDL
#include <SDL.h>
#endif // !SDL
#ifndef vector
#include <vector>
#endif
#include <time.h>
#include "Map.h"
#include "Controller.h"

class Entity {
public:

	enum directions {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	struct sectionData {
		std::vector <std::vector<SDL_Texture*>> animationData;
		int state;
		int drawFrame;
		bool animateForward;
		bool animationLocked;
		Artist::pos offset;
		void (*animate)(int,int);
	};

	struct ability {
		int keyBind;
		int scanType;
		void (*func)(int);
	};

	struct entity {
		int x;
		int y;
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
		float acceleration;
		float maxRunSpeed;
		std::vector<sectionData> sectionSets;
		std::string name;
		std::vector<ability> abilityList;
		void (*update)(int);
		// AI

		void control()
		{
			for (int i = 0; i < abilityList.size(); i++)
			{
				//if negative its a mouse input
				if (abilityList[i].keyBind < 0)
				{
					//flip number
					int mouseStateCheck = abilityList[i].keyBind * -1 - 1;

					//check that number in the mouse states if it equals the scan type then run the func
					if (abilityList[i].scanType == 2)
					{
						if (Controller::FPSLock && Controller::mouseStates[mouseStateCheck] == abilityList[i].scanType)
						{
							abilityList[i].func(player);
						}
					}
					else
					{
						if (Controller::mouseStates[mouseStateCheck] == abilityList[i].scanType)
						{
							abilityList[i].func(player);
						}
					}
				}
				else
				{
					if (abilityList[i].scanType == 2)
					{
						if (Controller::FPSLock && Controller::keyboardStates[abilityList[i].keyBind] == abilityList[i].scanType)
						{
							abilityList[i].func(player);
						}
					}
					else
					{
						if (Controller::keyboardStates[abilityList[i].keyBind] == abilityList[i].scanType)
						{
							abilityList[i].func(player);
						}
					}
				}
			}
		}

		void draw()
		{
			Artist artist;//JUST LAUNCH IT AND YOU KNOEW SOMTHING SO GET UR ASS IN GEAR AND FIX IT
			//MAKE IT GO THRU ALL SECTIONS AND ADD OFFSET AND FLIP SAID OFFSET IF FACING LEFT
			for (int i = 0; i < sectionSets.size(); i++)
			{
				//find the size of the texture so we can draw it from the center properly
				Artist::pos drawSize;
				SDL_QueryTexture(sectionSets[i].animationData[sectionSets[i].state][sectionSets[i].drawFrame], NULL, NULL, &drawSize.x, &drawSize.y);
				Artist::pos drawOffset;
				drawOffset.x = drawSize.x / 2;
				drawOffset.y = drawSize.y;
				//use the vars to draw it
				if (facingRight)
				{
					Artist::pos screenPos = { (x - drawOffset.x + sectionSets[i].offset.x) * Map::mapZoom - Map::topLeftPOS.x , (y - drawOffset.y + sectionSets[i].offset.y) * Map::mapZoom - Map::topLeftPOS.y };
					artist.drawImage(sectionSets[i].animationData[sectionSets[i].state][sectionSets[i].drawFrame], screenPos.x, screenPos.y, drawSize.x * Map::mapZoom, drawSize.y * Map::mapZoom, /*rotation*/NULL, NULL, SDL_FLIP_NONE);
					//artist.drawAnimation(sectionSets[0].animationData[sectionSets[0].state].animationFrames, (x - drawOffset.x) * Map::mapZoom - Map::topLeftPOS.x, (y - drawOffset.y) * Map::mapZoom - Map::topLeftPOS.y, 33.33, animationStartTime, drawSize.x * Map::mapZoom, drawSize.y * Map::mapZoom, 0, SDL_FLIP_NONE);
				}
				else
				{
					Artist::pos screenPos = { (x - drawOffset.x - sectionSets[i].offset.x) * Map::mapZoom - Map::topLeftPOS.x , (y - drawOffset.y + sectionSets[i].offset.y) * Map::mapZoom - Map::topLeftPOS.y };
					artist.drawImage(sectionSets[i].animationData[sectionSets[i].state][sectionSets[i].drawFrame], screenPos.x, screenPos.y, drawSize.x * Map::mapZoom, drawSize.y * Map::mapZoom, /*rotation*/NULL, NULL, SDL_FLIP_HORIZONTAL);
					//artist.drawAnimation(animationSets[state], (x - drawOffset.x) * Map::mapZoom - Map::topLeftPOS.x, (y - drawOffset.y) * Map::mapZoom - Map::topLeftPOS.y, 33.33, animationStartTime, drawSize.x * Map::mapZoom, drawSize.y * Map::mapZoom, 0, SDL_FLIP_HORIZONTAL);
				}
			}
		}

		bool tileCollision(int direction, Artist::pos* collisionPos)
		{
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
			float velAddCheckY = yVel;
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
			//maybe run it again if we do collide so we make sure we arnt ending up in one either( idk cuz this wont really effect them only let em know so ill have to look into it in the update)
		}
	};
	//keeps track of who we are controlling
	static int player;

	//restricts maximum speed
	static float airResistance;

	//keeps the player form sliding forever, multiplied by ur speed so cant be over 1, .99 to .9 seems best
	static float friction;

	//keeps track of how fast you will gain speed towards the earth NEEDS IMPLEMENTING
	//static float gravity;

	//controls how fast you can move
	static float terminalVel;

	//true if the camera should follow the player
	static bool followPlayer;

	//false for normal gameplay true if you want it to go frame by frame from a button press
	static bool frameByFrame;

	//setting to true will make the game advance one frame if in frame by frame mode
	static bool frameAdvance;

	//list of all active entities
	static std::vector<entity> entityList;

	
	
	
	
	
	
	
	
	void loadEntity(int x, int y, int width, int height, std::string name, std::string texName, std::vector<std::vector<int>> sectionCounts, std::vector<Artist::pos> sectionOffsets, std::vector<Entity::ability> abilityList, void(*update)(int), float gravity, float acceleration, float maxRunSpeed, float mass, float xVel, float yVel);
	void init();
	void drawEntities();
	void updateEntities();
	void controller();
};