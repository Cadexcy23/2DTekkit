#include "Entity.h"
#include "Artist.h"
#include "Controller.h"
#include <SDL.h>
#include "Projectile.h"
#include <cmath>


std::vector<Entity::entity> Entity::entityList;

bool Entity::frameAdvance = false;
bool Entity::frameByFrame = false;
bool Entity::followPlayer = true;
int lastEntityUpdate = 0;
//float Entity::gravity = 20;
float Entity::terminalVel = 25;
float Entity::friction = .95;
float Entity::airResistance = .99;
int Entity::player = 0;



//Put funcs here for assigning to entities/projectiles
//animate funcs for ents

void entAnimateLoop(int entID, int section)
{
	if (Entity::entityList[entID].sectionSets[section].drawFrame + 1 < Entity::entityList[entID].sectionSets[section].animationData[Entity::entityList[entID].sectionSets[section].state].size())
	{
		Entity::entityList[entID].sectionSets[section].drawFrame++;
	}
	else
	{
		Entity::entityList[entID].sectionSets[section].drawFrame = 0;
	}

}

void entAnimateOnceUnlock(int entID, int section)
{
	if (Entity::entityList[entID].sectionSets[section].drawFrame + 1 < Entity::entityList[entID].sectionSets[section].animationData[Entity::entityList[entID].sectionSets[section].state].size())
	{
		Entity::entityList[entID].sectionSets[section].drawFrame++;
	}
	else
	{
		Entity::entityList[entID].sectionSets[section].drawFrame = 0;
		Entity::entityList[entID].sectionSets[section].animationLocked = false;
		Entity::entityList[entID].sectionSets[section].animate = entAnimateLoop;
	}
}

//update functions for projectiles
void fartUpdate(int proID)
{
	Projectile projectile;
	Projectile::projectile* pro = &Projectile::projectileList[proID];

	//make it slowly stop
	int timeActive = clock() - pro->creationTime;

	if (timeActive > 200 && abs(pro->xVel) > 2)
	{
		if (pro->xVel > 0)
		{
			pro->xVel--;
		}
		else
		{
			pro->xVel++;
		}
	}
	if (timeActive > 400 && abs(pro->xVel) > 1)
	{
		if (pro->xVel > 0)
		{
			pro->xVel--;
		}
		else
		{
			pro->xVel++;
		}
	}
	if (timeActive > 600 && abs(pro->xVel) > 0)
	{
		if (pro->xVel > 0)
		{
			pro->xVel--;
		}
		else
		{
			pro->xVel++;
		}
	}

	if (timeActive > 250 && pro->yVel > 0)
	{
		pro->yVel--;
	}
	if (timeActive > 500 && pro->yVel > -1)
	{
		pro->yVel--;
	}
	if (timeActive > 750 && pro->yVel < 1)
	{
		pro->yVel++;
	}
}

void proAnimateOnce(int proID)
{
	Projectile::projectile* pro = &Projectile::projectileList[proID];

	//make i only do 1 run through the animation
	if (pro->drawFrame + 1 >= pro->animationSets[pro->state].size())
	{

	}
	else
	{
		pro->drawFrame++;
	}
}

void rollWithXVel(int proID)
{
	Projectile::projectile* pro = &Projectile::projectileList[proID];

	if (pro->movementTimeY == 0)
	{
		pro->spin = pro->xVel;
	}
}

void basicProUpdate(int proID)
{
	//get a pointer to the ent we are updating
	Projectile::projectile* pro = &Projectile::projectileList[proID];
	Projectile projectile;

	//keep track of how long we are in the air
	if (abs(pro->yVel) > 0)
	{
		if (pro->movementTimeYStart == 0)
		{
			pro->movementTimeYStart = clock();
		}
		pro->movementTimeY = clock() - pro->movementTimeYStart;
	}
	else
	{
		pro->movementTimeY = 0;
		pro->movementTimeYStart = 0;
		pro->knockBackY = 0;
	}

	//bring us to a dead stop if we go real slow, maybe at some point make it like less than .25 or if accel under that then accel as this may be bad with large numbers
	//if (abs(pro->xVel) < pro->acceleration)
	//{
	//	pro->xVel = 0;
	//}

	//if we dont have gravity do a different check
	if (pro->gravity == 0)
	{
		//*yVel = *yVel * airResistance / *mass;
		////makse sure we come to a complete stop
		//if (abs(*yVel) < *acceleration)
		//{
		//	*yVel = 0;
		//}
	}
	else
	{
		//temp
		float friction = .95;
		float airResistance = .99;
		//int timeActive = clock() - pro->creationTime; //NEED A DIF VARIABLE MAYBE ADD BACK timeMoveY/airTime    DELETE LINE
		//change the vel based on time gravity and a bunch of shit
		pro->yVel = (-1 * pro->mass) / Entity::airResistance * (exp(-1 * (Entity::airResistance * (float(pro->movementTimeY + 1) / 1000.0) / pro->mass))) * (pro->gravity - Entity::airResistance / pro->mass * pro->knockBackY) + pro->mass / Entity::airResistance * pro->gravity;
	}

	//if in the air dont apply friction
	if (pro->movementTimeY > 0)//chance check to if we have tile collision under us maybe
	{
		//xVel = xVel * exp((-1.0 * airResistance) * (float(movementTimeY + 1) / 1000.0) / mass) * 1.0;
		pro->xVel = pro->xVel * Entity::airResistance / pro->mass;
	}
	else
	{
		//xVel = xVel * exp((-1.0 * friction) * (float(movementTimeY + 1) / 1000.0) / mass) * 1.0;
		pro->xVel *= Entity::friction / Entity::airResistance;
	}

	//keep it spinning
	pro->rotation += pro->spin;

	//checks for tile/ent/pro collision and runs the functions the projectile has attached for each case
	Artist::pos collisionPoint = { 0,0 };
	//TEMP ON WHAT IT DOES AFTER we want it tp pull from a list of actions customized per projectile
	if (pro->tileCollisionCheck(Entity::DOWN, &collisionPoint))
	{
		//keep track of if we still need to look
		bool searching = true;
		for (int LoopY = 0; searching; LoopY++)
		{
			//keep track of if current row we are checking is clear
			bool rowClear = true;
			for (int LoopX = 0; LoopX < pro->width; LoopX++)
			{
				//get the spot we are looking at
				Artist::pos checkPos = { pro->x + LoopX - pro->width / 2 , collisionPoint.y - 1 - LoopY };

				if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
				{
					rowClear = false;
				}
			}

			if (rowClear)
			{
				pro->y = collisionPoint.y - 1 - LoopY;
				searching = false;
			}
		}
		//clear y velocity
		pro->yVel = 0;
	}
	if (pro->tileCollisionCheck(Entity::UP, &collisionPoint))
	{
		//keep track of if we still need to look
		bool searching = true;
		for (int LoopY = 0; searching; LoopY++)
		{
			//keep track of if current row we are checking is clear
			bool rowClear = true;
			for (int LoopX = 0; LoopX < pro->width; LoopX++)
			{
				//get the spot we are looking at
				Artist::pos checkPos = { pro->x + LoopX - pro->width / 2 , collisionPoint.y + LoopY };

				if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
				{
					rowClear = false;
				}
			}

			if (rowClear)
			{
				pro->y = collisionPoint.y + pro->height + LoopY;
				searching = false;
			}
		}
		//clear y velocity
		pro->yVel = 0;
	}
	if (pro->tileCollisionCheck(Entity::LEFT, &collisionPoint))
	{
		//keep track of if we still need to look Up
		bool upSearching = true;
		//keep track of if we still need to look in general
		bool searching = true;
		//look for a open space above us so you can go up slopes but cant make it infinate ( prolly based off abs(xVel))
		//only if we are really moving?
		if (abs(pro->xVel) > 1)
		{
			for (int LoopYStart = 0; LoopYStart < abs(pro->xVel) + 5; LoopYStart++)
			{
				//keep track of if current row we are checking is clear
				bool rowClear = true;
				for (int LoopYCurrent = 0; LoopYCurrent < pro->height; LoopYCurrent++)
				{
					//get the spot we are looking at							//maybe \/ add offset here if detecting floor
					Artist::pos checkPos = { collisionPoint.x, pro->y - 1 - LoopYStart - LoopYCurrent };
			
					if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
					{
						rowClear = false;
					}
					if (LoopYCurrent == pro->height)
					{
						upSearching = false;
					}
				}
				if (rowClear && upSearching)
				{
					pro->y = pro->y - LoopYStart;
					//maybe set y to the y we are at?
					upSearching = false;
					searching = false;
				}
			}
			for (int LoopX = 0; searching; LoopX++)
			{
				//keep track of if current row we are checking is clear
				bool rowClear = true;
				for (int LoopY = 0; LoopY < pro->height; LoopY++)
				{
					//get the spot we are looking at							//maybe \/ add offset here if detecting floor
					Artist::pos checkPos = { collisionPoint.x + LoopX , pro->y - LoopY };

					if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
					{
						rowClear = false;
					}
				}
				if (rowClear && searching)
				{
					pro->x = collisionPoint.x + LoopX + pro->width / 2;
					searching = false;
					//clear x velocity
					pro->xVel = 0;
				}
			}
		}
	}
	if (pro->tileCollisionCheck(Entity::RIGHT, &collisionPoint))
	{
		//keep track of if we still need to look Up
		bool upSearching = true;
		//keep track of if we still need to look in general
		bool searching = true;
		//look for a open space above us so you can go up slopes but cant make it infinate ( prolly based off abs(xVel))\
		//only change our pos if we are really moving anywhere meaningful
		if (abs(pro->xVel) > 1)
		{
			//this loop keeps track of where we start to look if there is room to fit the ent
			for (int LoopYStart = 0; LoopYStart < abs(pro->xVel) + 5; LoopYStart++)
			{
				//keep track of if current row we are checking is clear
				bool rowClear = true;
				//this loop keeps track of what unit we are checking is clear
				for (int LoopYCurrent = 0; LoopYCurrent < pro->height && upSearching; LoopYCurrent++)
				{
					//get the spot we are looking at based on our Y pos and X collision point
					Artist::pos checkPos = { collisionPoint.x, pro->y - 1 - LoopYStart - LoopYCurrent };
					//check if that spot has collision
					if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
					{
						//if it does set the flag that we arnt clear for landing
						rowClear = false;
					}
				}
				//if the row was empty and we are still looking for a spot above us
				if (rowClear && upSearching)
				{
					//set the Y up by the loop we found the open spot on
					pro->y = pro->y - LoopYStart;
					//set out search flags to false as we are done looking
					upSearching = false;
					searching = false;
				}
			}


			//Look left for open spot
			for (int LoopX = 0; searching; LoopX++)
			{
				//keep track of if current row we are checking is clear
				bool rowClear = true;
				for (int LoopY = 0; LoopY < pro->height; LoopY++)
				{
					//get the spot we are looking a
					Artist::pos checkPos = { collisionPoint.x - LoopX - 1 , pro->y - LoopY };

					if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
					{
						rowClear = false;
					}
				}

				if (rowClear)
				{
					pro->x = collisionPoint.x - LoopX - pro->width / 2;
					searching = false;
					//clear x velocity
					pro->xVel = 0;
				}
			}
		}
	}


	//apply velocity to the actual position
	pro->x += pro->xVel;
	pro->y += pro->yVel;
}

//ability functions
void throwRock(int entID)
{
	//init projectile stuff
	Projectile projectile;

	//set a var pointer to get the Ents info EZ
	Entity::entity* ent = &Entity::entityList[entID];

	//set projectile frame data
	std::vector<int> frameCounts = { 1 };

	//set projectile update list
	std::vector<void (*)(int)> updateList = { basicProUpdate, rollWithXVel };

	//get a pos to make the rock be thrown from a better starting pos (kinda arbitrary)
	Artist::pos startPos = { ent->x, ent->y };
	if (ent->facingRight)
	{
		startPos.x += ent->width / 2;
	}
	else
	{
		startPos.x -= ent->width / 2;
	}
	startPos.y -= ent->height * .66;

	//get the velocity needed to throw the rock at the right angle from char to mouse
	float xVel;
	float yVel;
	projectile.getVelocityToTarget({ startPos.x, startPos.y }, { Map::mouseInGamePos.x, Map::mouseInGamePos.y }, 10, &xVel, &yVel);

	//load the rock
	projectile.loadProjectile("Rockie", startPos.x, startPos.y, 20, 20, "rock", frameCounts, updateList, 20, 1, xVel, yVel, 0, 0, 10000);

	//set ent state so it plays the rock throw animation
	ent->sectionSets[3].state = 4;
	ent->sectionSets[3].drawFrame = 0;
	ent->sectionSets[3].animationLocked = true;
	ent->sectionSets[3].animate = entAnimateOnceUnlock;
}

void fart(int entID)
{
	//init projectile stuff
	Projectile projectile;

	//set a var pointer to get the Ents info EZ
	Entity::entity* ent = &Entity::entityList[entID];
	//set frame data
	std::vector<int> frameCounts = { 8 };
	//set x and y velocity
	int xVel = 3;
	if (ent->facingRight)
	{
		xVel *= -1;
	}
	xVel += ent->xVel;
	int yVel = 1 + ent->yVel;
	//create list of update funcs for projectiles
	std::vector<void (*)(int)> updateList = { basicProUpdate, fartUpdate, proAnimateOnce };
	projectile.loadProjectile("fart", ent->x, ent->y - 25, 42, 38, "fart", frameCounts, updateList, 0, 1, xVel, yVel, 5, 0, 1000);
}

void walkLeft(int entID)
{
	Entity::entity* ent = &Entity::entityList[entID];
	//turn player around
	ent->facingRight = false;
	//give a boost to when you first hit the key
	if (ent->xVel < -1)
	{
		if (ent->xVel - ent->acceleration < ent->maxRunSpeed * -1)
		{
			float addAmount = ent->maxRunSpeed - abs(ent->xVel);

			if (abs(addAmount) > 0)
			{
				ent->xVel -= addAmount;
			}
		}
		else
		{
			ent->xVel -= ent->acceleration;
		}
	}
	else
	{
		ent->xVel -= 1;
	}
}

void walkRight(int entID)
{
	Entity::entity* ent = &Entity::entityList[entID];
	//turn player around
	ent->facingRight = true;
	//give a boost to when you first hit the key
	if (ent->xVel > 1)
	{
		if (ent->xVel + ent->acceleration > ent->maxRunSpeed)
		{
			float addAmount = ent->maxRunSpeed - ent->xVel;

			if (addAmount > 0)
			{
				ent->xVel += addAmount;
			}
		}
		else
		{
			ent->xVel += ent->acceleration;
		}
	}
	else
	{
		ent->xVel += 1;
	}
}

void jump(int entID)
{
	Entity::entity* ent = &Entity::entityList[entID];
	if (ent->movementTimeY == 0)
	{
		ent->yVel -= 1;
		ent->knockBackY -= 8.5;
	}
}

void flyUp(int entID)
{
	Entity::entity* ent = &Entity::entityList[entID];
	if (ent->gravity == 0)
	{
		if (ent->yVel < -1)
		{
			if (ent->yVel - ent->acceleration < ent->maxRunSpeed * -1)
			{
				float addAmount = ent->maxRunSpeed - abs(ent->yVel);

				if (abs(addAmount) > 0)
				{
					ent->yVel -= addAmount;
				}
			}
			else
			{
				ent->yVel -= ent->acceleration;
			}
		}
		else
		{
			ent->yVel -= 1;
		}
	}
}

void flyDown(int entID)
{
	Entity::entity* ent = &Entity::entityList[entID];
	if (ent->gravity == 0)
	{
		if (ent->yVel > 1)
		{
			if (ent->yVel + ent->acceleration > ent->maxRunSpeed)
			{
				float addAmount = ent->maxRunSpeed - ent->yVel;

				if (addAmount > 0)
				{
					ent->yVel += addAmount;
				}
			}
			else
			{
				ent->yVel += ent->acceleration;
			}
		}
		else
		{
			ent->yVel += 1;
		}
	}
}

void toggleGravity(int entID)
{
	Entity::entity* ent = &Entity::entityList[entID];
	if (abs(ent->gravity) > 0)
	{
		ent->gravity = 0;
		ent->sectionSets[0].state = 4;
		ent->sectionSets[0].drawFrame = 0;
		ent->movementTimeYStart = 0;
		ent->knockBackX = 0;
		ent->knockBackY = 0;
	}
	else
	{
		ent->gravity = 20;
		ent->sectionSets[0].state = 2;
		ent->sectionSets[0].drawFrame = 0;
		ent->movementTimeYStart = 0;
		ent->knockBackX = 0;
		ent->knockBackY = 0;
	}
}

//update funcs for ents
void basicUpdate(int entityID)
{
	//get what ent we are looking at
	Entity::entity* ent = &Entity::entityList[entityID];
	
	//keep track of how long we are in the air
	if (abs(ent->yVel) > 0)
	{
		if (ent->movementTimeYStart == 0)
		{
			ent->movementTimeYStart = clock();
		}
		ent->movementTimeY = clock() - ent->movementTimeYStart;
	}
	else
	{
		ent->movementTimeY = 0;
		ent->movementTimeYStart = 0;
		ent->knockBackY = 0;
	}

	//bring us to a dead stop if we go real slow, maybe at some point make it like less than .25 or if accel under that then accel as this may be bad with large numbers
	if (abs(ent->xVel) < ent->acceleration)
	{
		ent->xVel = 0;
	}

	//if we dont have gravity do a different check
	if (ent->gravity == 0)
	{
		ent->yVel = ent->yVel * Entity::airResistance / ent->mass;
		//makse sure we come to a complete stop
		if (abs(ent->yVel) < ent->acceleration)
		{
			ent->yVel = 0;
		}
	}
	else
	{
		//change the vel based on time gravity and a bunch of shit
		ent->yVel = (-1 * ent->mass) / Entity::airResistance * (exp(-1 * (Entity::airResistance * (float(ent->movementTimeY + 1) / 1000.0) / ent->mass))) * (ent->gravity - Entity::airResistance / ent->mass * ent->knockBackY) + ent->mass / Entity::airResistance * ent->gravity;
	}


	//if in the air dont apply friction
	if (ent->movementTimeY > 0)//chance check to if we have tile collision under us maybe
	{
		//xVel = xVel * exp((-1.0 * airResistance) * (float(movementTimeY + 1) / 1000.0) / mass) * 1.0;
		ent->xVel = ent->xVel * Entity::airResistance / ent->mass;
	}
	else
	{
		//xVel = xVel * exp((-1.0 * friction) * (float(movementTimeY + 1) / 1000.0) / mass) * 1.0;
		ent->xVel *= Entity::friction / Entity::airResistance;
	}

	//terminal velocity check at some point ???????

	//keeps track of where we hit walls
	Artist::pos collisionPoint = { -1, -1 };
	//tile collision checks
	if (Entity::entityList[entityID].tileCollision(Entity::DOWN, &collisionPoint))
	{
		//keep track of if we still need to look
		bool searching = true;
		for (int LoopY = 0; searching; LoopY++)
		{
			//keep track of if current row we are checking is clear
			bool rowClear = true;
			for (int LoopX = 0; LoopX < ent->width; LoopX++)
			{
				//get the spot we are looking at
				Artist::pos checkPos = { ent->x + LoopX - ent->width / 2 , collisionPoint.y - 1 - LoopY };

				if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
				{
					rowClear = false;
				}
			}

			if (rowClear)
			{
				ent->y = collisionPoint.y - 1 - LoopY;
				searching = false;
			}
		}
		//clear y velocity
		ent->yVel = 0;
	}
	if (Entity::entityList[entityID].tileCollision(Entity::UP, &collisionPoint))
	{
		//keep track of if we still need to look
		bool searching = true;
		for (int LoopY = 0; searching; LoopY++)
		{
			//keep track of if current row we are checking is clear
			bool rowClear = true;
			for (int LoopX = 0; LoopX < ent->width; LoopX++)
			{
				//get the spot we are looking at
				Artist::pos checkPos = { ent->x + LoopX - ent->width / 2 , collisionPoint.y + LoopY };

				if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
				{
					rowClear = false;
				}
			}

			if (rowClear)
			{
				ent->y = collisionPoint.y + ent->height + LoopY;
				searching = false;
			}
		}
		//clear y velocity
		ent->yVel = 0;
	}
	if (Entity::entityList[entityID].tileCollision(Entity::LEFT, &collisionPoint))
	{
		//keep track of if we still need to look Up
		bool upSearching = true;
		//keep track of if we still need to look in general
		bool searching = true;
		//look for a open space above us so you can go up slopes but cant make it infinate ( prolly based off abs(xVel))\
		//only if we are really moving?
		if (abs(ent->xVel) > 1)
		{
			for (int LoopYStart = 0; LoopYStart < abs(Entity::entityList[entityID].xVel) + 5; LoopYStart++)
			{
				//keep track of if current row we are checking is clear
				bool rowClear = true;
				for (int LoopYCurrent = 0; LoopYCurrent < ent->height; LoopYCurrent++)
				{
					//get the spot we are looking at							//maybe \/ add offset here if detecting floor
					Artist::pos checkPos = { collisionPoint.x, ent->y - 1 - LoopYStart - LoopYCurrent };

					if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
					{
						rowClear = false;
					}
					if (LoopYCurrent == ent->height)
					{
						upSearching = false;
					}
				}
				if (rowClear && upSearching)
				{
					ent->y = ent->y - LoopYStart;
					//maybe set y to the y we are at?
					upSearching = false;
					searching = false;
				}
			}
			for (int LoopX = 0; searching; LoopX++)
			{
				//keep track of if current row we are checking is clear
				bool rowClear = true;
				for (int LoopY = 0; LoopY < ent->height; LoopY++)
				{
					//get the spot we are looking at							//maybe \/ add offset here if detecting floor
					Artist::pos checkPos = { collisionPoint.x + LoopX , ent->y - LoopY };

					if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
					{
						rowClear = false;
					}
				}
				if (rowClear && searching)
				{
					ent->x = collisionPoint.x + LoopX + ent->width / 2;
					searching = false;
					//clear x velocity
					ent->xVel = 0;
				}
			}
		}
	}
	if (Entity::entityList[entityID].tileCollision(Entity::RIGHT, &collisionPoint))
	{
		//keep track of if we still need to look Up
		bool upSearching = true;
		//keep track of if we still need to look in general
		bool searching = true;
		//look for a open space above us so you can go up slopes but cant make it infinate ( prolly based off abs(xVel))\
		//only change our pos if we are really moving anywhere meaningful
		if (abs(ent->xVel) > 1)
		{
			//this loop keeps track of where we start to look if there is room to fit the ent
			for (int LoopYStart = 0; LoopYStart < abs(Entity::entityList[entityID].xVel) + 5; LoopYStart++)
			{
				//keep track of if current row we are checking is clear
				bool rowClear = true;
				//this loop keeps track of what unit we are checking is clear
				for (int LoopYCurrent = 0; LoopYCurrent < ent->height && upSearching; LoopYCurrent++)
				{
					//get the spot we are looking at based on our Y pos and X collision point
					Artist::pos checkPos = { collisionPoint.x, ent->y - 1 - LoopYStart - LoopYCurrent };
					//check if that spot has collision
					if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
					{
						//if it does set the flag that we arnt clear for landing
						rowClear = false;
					}
				}
				//if the row was empty and we are still looking for a spot above us
				if (rowClear && upSearching)
				{
					//set the Y up by the loop we found the open spot on
					ent->y = ent->y - LoopYStart;
					//set out search flags to false as we are done looking
					upSearching = false;
					searching = false;
				}
			}
			

			//Look left for open spot
			for (int LoopX = 0; searching; LoopX++)
			{
				//keep track of if current row we are checking is clear
				bool rowClear = true;
				for (int LoopY = 0; LoopY < ent->height; LoopY++)
				{
					//get the spot we are looking a
					Artist::pos checkPos = { collisionPoint.x - LoopX - 1 , ent->y - LoopY };

					if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
					{
						rowClear = false;
					}
				}

				if (rowClear)
				{
					ent->x = collisionPoint.x - LoopX - ent->width / 2;
					searching = false;
					//clear x velocity
					ent->xVel = 0;
				}
			}
		}
	}
	


	//make sure we dont go out of the map DONT WORK RN BUT NOT TOO BIG A DEAL
	if (ent->x - ent->width / 2 + ent->xVel < 0)
	{
		ent->x = 0 + ent->width / 2;
		ent->xVel = 0;
	}
	if (ent->y - ent->height + ent->yVel < 0)
	{
		ent->y = 0 + ent->height;
		ent->yVel = 0;
	}
	if (ent->x + ent->width / 2 + ent->xVel > Map::tileMap.size() * 64)
	{
		ent->x = Map::tileMap.size() * 64 - ent->width / 2;
		ent->xVel = 0;
	}
	if (ent->y + ent->yVel > Map::tileMap[0].size() * 64)
	{
		ent->y = Map::tileMap[0].size() * 64;
		ent->yVel = 0;
	}

	//apply velocity to the actual position
	ent->x += int(ent->xVel);
	ent->y += int(ent->yVel);

	//stick to ground
	if (ent->yVel > 0)//toggled off to test collision code
	{
		//scan ground under our feet if there is any collision close, if so find that point and set our y to it
	
		//keep track of if we still need to look
		bool searching = true;
		for (int LoopY = 0; LoopY < abs(Entity::entityList[entityID].xVel) + 5 && searching; LoopY++)
		{
			//keep track of if current row we are checking is clear
			bool rowClear = true;
			for (int LoopX = 0; LoopX < ent->width; LoopX++)
			{
				//get the spot we are looking at
				Artist::pos checkPos = { ent->x + LoopX - ent->width / 2, ent->y + 1 + LoopY };
	
				if (Map::tileMap[checkPos.x / 64][checkPos.y / 64]->microCollision[checkPos.x % 64][checkPos.y % 64] == 1)
				{
					rowClear = false;
				}
			}
	
			if (!rowClear)
			{
				ent->y = ent->y - 1 + LoopY;
				ent->yVel = 0;
				searching = false;
			}
		}
	}
}

void updateHuman(int entID)
{
	Entity::entity* ent = &Entity::entityList[entID];
	//change animation
	//if standing still
	for (int i = 0; i < ent->sectionSets.size(); i++)
	{
		if (abs(ent->xVel) < 1 && ent->sectionSets[i].state != 0 && !ent->sectionSets[i].animationLocked)
		{
			ent->sectionSets[i].state = 0;
			ent->sectionSets[i].drawFrame = 0;
		}
	}
	//if walkling
	for (int i = 0; i < ent->sectionSets.size(); i++)
	{
		if (abs(ent->xVel) > 1 && ent->sectionSets[i].state != 1 && !ent->sectionSets[i].animationLocked)
		{
			ent->sectionSets[i].state = 1;
			ent->sectionSets[i].drawFrame = 0;
		}
	}
	//and for if in air
	if (ent->movementTimeY > 0)
	{
		for (int i = 0; i < ent->sectionSets.size(); i++)
		{
			if (ent->yVel < -1 && ent->sectionSets[i].state != 2 && !ent->sectionSets[i].animationLocked)
			{
				ent->sectionSets[i].state = 2;
				ent->sectionSets[i].drawFrame = 0;
			}
		}
		for (int i = 0; i < ent->sectionSets.size(); i++)
		{
			if (ent->yVel > 1 && ent->sectionSets[i].state != 3 && !ent->sectionSets[i].animationLocked)
			{

				ent->sectionSets[i].state = 3;
				ent->sectionSets[i].drawFrame = 0;
			}
		}
	}
}

void updateBee(int entID)
{
	Entity::entity* ent = &Entity::entityList[entID];
	//change animation
	//if we arnt flying
	if (ent->gravity != 0)
	{
		//if idle
		if (abs(ent->xVel) < 1)
		{
			ent->sectionSets[0].state = 0;
			ent->sectionSets[0].drawFrame = 0;
		}
		//if walking
		if (abs(ent->xVel) > 1)
		{
			//change animation
			ent->sectionSets[0].state = 1;
			ent->sectionSets[0].drawFrame = 0;
		}
		//and for if in air
		if (ent->movementTimeY > 0)
		{
			if (ent->yVel < -1)
			{
				//change animation
				ent->sectionSets[0].state = 2;
				ent->sectionSets[0].drawFrame = 0;
			}
			else
			{
				//change animation
				ent->sectionSets[0].state = 3;
				ent->sectionSets[0].drawFrame = 0;
			}
		}
	}
}

//basic loading func
void Entity::loadEntity(int x, int y, int width, int height, std::string name, std::string texName, std::vector<std::vector<int>> sectionCounts, std::vector<Artist::pos> sectionOffsets,  std::vector<Entity::ability> abilityList, void (*update)(int), float gravity = 20, float acceleration = .25, float maxRunSpeed = 5, float mass = 1, float xVel = 0, float yVel = 0)
{
	Artist artist;

	Entity::entity tempEnt;

	//set pos
	tempEnt.x = x;
	tempEnt.y = y;
	//set size
	tempEnt.width = width;
	tempEnt.height = height;
	//set gravity
	tempEnt.gravity = gravity;
	//set mass
	tempEnt.mass = mass;
	//set vars that keep track of air time
	tempEnt.movementTimeY = 0;
	tempEnt.movementTimeYStart = 0;
	//set knockback to 0
	tempEnt.knockBackX = 0;
	tempEnt.knockBackY = 0;
	//set velocity
	tempEnt.xVel = xVel;
	tempEnt.yVel = yVel;
	//set name
	tempEnt.name = name;
	//set accel speed
	tempEnt.acceleration = acceleration;
	//set max run speed
	tempEnt.maxRunSpeed = maxRunSpeed;
	//set section list size
	tempEnt.sectionSets.resize(sectionCounts.size());
	//go thru each section
	for (int i = 0; i < tempEnt.sectionSets.size(); i++)
	{
		//set starting states to idle
		tempEnt.sectionSets[i].state = 0;
		//set the offset of each section
		tempEnt.sectionSets[i].offset = sectionOffsets[i];
		//set animation locking to false
		tempEnt.sectionSets[i].animationLocked = false;
		// load each animation into memory
		for (int j = 0; j < sectionCounts[i].size(); j++)
		{
			tempEnt.sectionSets[i].animationData.resize(sectionCounts[i].size());
			tempEnt.sectionSets[i].animationData[j] = artist.loadAnimationData("entities", texName, i, j, sectionCounts[i][j]);
		}
		//set the animation type for each section
		tempEnt.sectionSets[i].animate = entAnimateLoop;
	}
	
	
	
	//set starting direction to right
	tempEnt.facingRight = true;
	//set ability list
	tempEnt.abilityList = abilityList;
	//set update func
	tempEnt.update = update;

	//add ent to list
	Entity::entityList.push_back(tempEnt);
}

void Entity::init()
{
	//load player char
	//set amount of animation and frames of each secction
	std::vector<std::vector<int>> sectionCounts = { { 41, 12, 1, 1 }, { 41, 12, 1, 1 }, { 41, 12, 1, 1 }, { 41, 4, 1, 1, 9 } };
	//set the offsets of each section
	std::vector<Artist::pos> sectionOffsets = { {0, -87 }, {0, -48 }, {0, 0}, {0, 0} };
	//set the animation type of each section
	//NEEDS ADDED?
	//set how many and what functions the entity has
	std::vector<ability> abilityList;
	//set amount
	abilityList.resize(5);
	//set each func, its keybind, and scan type
	abilityList[0] = { SDL_SCANCODE_A, 2, walkLeft };
	abilityList[1] = { SDL_SCANCODE_D, 2, walkRight };
	abilityList[2] = { SDL_SCANCODE_SPACE, 1, jump };
	abilityList[3] = { SDL_SCANCODE_F , 1, fart };
	abilityList[4] = { Controller::MOUSE_LEFT_CLICK, 1, throwRock };
	//actualy load the entity with the data
	loadEntity(256, 256, 50, 120, "Player", "player", sectionCounts, sectionOffsets, abilityList, updateHuman);


	//load Ben the bee
	sectionCounts = { { 2, 2, 1, 1, 20 } };
	sectionOffsets = { {0, 0} };
	abilityList.resize(6);
	abilityList[0] = { SDL_SCANCODE_A, 2, walkLeft };
	abilityList[1] = { SDL_SCANCODE_D, 2, walkRight };
	abilityList[2] = { SDL_SCANCODE_W, 2, flyUp };
	abilityList[3] = { SDL_SCANCODE_S, 2, flyDown };
	abilityList[4] = { SDL_SCANCODE_F, 1, toggleGravity };
	abilityList[5] = { SDL_SCANCODE_SPACE, 1, jump };
	loadEntity(356, 256, 22, 20, "Ben", "bee", sectionCounts, sectionOffsets, abilityList, updateBee);
	
	//load Goblino
	//frameCounts = { 4, 4 };
	//loadEntity(456, 256, 40, 59, "Goblino", "goblino", 2, frameCounts);
	//loadEntity(256, 256, 50, 40, "Blob", "slime", .25, 2.5, 2, rand() % 25, rand() % 25);
	//loadEntity(256, 256, 50, 40, "B-lob", "slime", .25, 2.5, 3, rand() % 25, rand() % 25);
	//loadEntity(256, 256, 50, 40, "Blo-b", "slime", .25, 2.5, 4, rand() % 25, rand() % 25);
	//loadEntity(256, 256, 50, 40, "Wheel", "wheel", 1, 100, 1, rand() % 25, rand() % 25);
	//
	////TEMP
	//SDL_SetTextureColorMod(entityList[1].tex, 255, 0, 0);
	//SDL_SetTextureColorMod(entityList[2].tex, 0, 255, 0);
	//SDL_SetTextureColorMod(entityList[3].tex, 0, 0, 255);
}

void Entity::drawEntities()
{
	for (int i = 0; i < Entity::entityList.size(); i++)
	{
		Entity::entityList[i].draw();
	}
}

void Entity::updateEntities()
{
	//add ai here

	if (Entity::followPlayer)
	{
		Map::cameraPOS.x = Entity::entityList[player].x;
		Map::cameraPOS.y = Entity::entityList[player].y;
	}
	//if things need to happen 60 times a sec
	if (Controller::FPSLock && !frameByFrame || frameAdvance)
	{
		for (int i = 0; i < Entity::entityList.size(); i++)
		{
			basicUpdate(i);
			Entity::entityList[i].update(i);
		}
		frameAdvance = false;
	}
	//things happen at the designated animation time
	if (Artist::frameRateLock)
	{
		for (int i = 0; i < Entity::entityList.size(); i++)
		{
			//update section animations via attached func
			for (int j = 0; j < Entity::entityList[i].sectionSets.size(); j++)
			{
				Entity::entityList[i].sectionSets[j].animate(i, j);
			}
		}
	}
}

void controlEnts()
{
	
	Entity::entityList[Entity::player].control();
	
	if (Controller::keyboardStates[SDL_SCANCODE_LALT] == 2 || Controller::keyboardStates[SDL_SCANCODE_RALT] == 2)
	{
		//TEMP gravity flip
		if (Controller::keyboardStates[SDL_SCANCODE_G] == 1)
		{
			Entity::entityList[Entity::player].gravity *= -1;
		}

		//TEMP cycle player entity RE WRITE
		if (Controller::keyboardStates[SDL_SCANCODE_P] == 1)
		{
			if (Entity::player + 1 < Entity::entityList.size())
			{
				Entity::player += 1;
			}
			else
			{
				Entity::player = 0;
			}
			
		}
		
		if (Controller::keyboardStates[SDL_SCANCODE_Z] == 1)
		{
			if (Entity::frameByFrame)
			{
				Entity::frameByFrame = false;
			}
			else
			{
				Entity::frameByFrame = true;
			}
		}
		if (Controller::keyboardStates[SDL_SCANCODE_C] == 1)
		{
			if (Entity::followPlayer)
			{
				Entity::followPlayer = false;
			}
			else
			{
				Entity::followPlayer = true;
			}
		} 
		if (Controller::keyboardStates[SDL_SCANCODE_X] == 1)
		{
			Entity::frameAdvance = true;
		}
	}
}

void Entity::controller()
{
	controlEnts();
}