#include "Projectile.h"
#include "Controller.h"
#include "Artist.h"
#include "Entity.h"


std::vector<Projectile::projectile> Projectile::projectileList;


void Projectile::loadProjectile(std::string name, int x, int y, int width, int height, std::string texName, std::vector<int> frameCounts, std::vector<void (*)(int)> updateList, float gravity = 20, float mass = 1, float xVel = 0, float yVel = 0, float spin = 0, float rotation = 0, int duration = 1000)
{
	Artist artist;

	Projectile::projectile tempPro;

	//set pos
	tempPro.x = x;
	tempPro.y = y;
	//set size
	tempPro.width = width;
	tempPro.height = height;
	//set gravity
	tempPro.gravity = gravity;
	//set mass
	tempPro.mass = mass;
	//set vars that keep track of air time
	tempPro.movementTimeY = 0;
	tempPro.movementTimeYStart = 0;
	//set knockback to 0
	tempPro.knockBackX = xVel;
	tempPro.knockBackY = yVel;
	//set velocity
	tempPro.xVel = xVel;
	tempPro.yVel = yVel;
	//set name
	tempPro.name = name;
	//set creation time
	tempPro.creationTime = clock();
	//set duration
	tempPro.duration = duration;
	//set spin
	tempPro.spin = spin;
	//set rotation
	tempPro.rotation = rotation;
	//set starting state to idle
	tempPro.state = 0;
	//set animation list size
	tempPro.animationSets.resize(frameCounts.size());
	// load each animation into memory
	for (int i = 0; i < frameCounts.size(); i++)
	{
		tempPro.animationSets[i] = artist.loadAnimationData("projectiles", texName, 0, i, frameCounts[i]);//NEEDS EDITED TO ENABLE SECTIONS LATER
	}
	//set starting direction to right
	tempPro.facingRight = true;
	//set update func
	tempPro.updateList = updateList;

	//add ent to list
	Projectile::projectileList.push_back(tempPro);
}

void Projectile::unloadProjectile(int proID)
{
	projectileList.erase(projectileList.begin() + proID);
}

void Projectile::getVelocityToTarget(Artist::pos startPos, Artist::pos targetPos, float totalVel, float *xReturnVel, float *yReturnVel)
{
	int xDirection = 0;
	int yDirection = 0;
	if (targetPos.x > startPos.x)
	{
		xDirection = 1;
	}
	else if (targetPos.x < startPos.x)
	{
		xDirection = -1;
	}
	if (targetPos.y > startPos.y)
	{
		yDirection = 1;
	}
	else if (targetPos.y < startPos.y)
	{
		yDirection = -1;
	}

	//determine distance from start to mouse pos
	float xDistance = 0;
	float yDistance = 0;
	xDistance = abs(startPos.x - targetPos.x);
	yDistance = abs(startPos.y - targetPos.y);

	//allocate total velocity to both scaled by ratio
	float xVel = 0;
	float yVel = 0;
	
	xVel = totalVel / sqrt(pow(yDistance / xDistance, 2) + 1);
	yVel = totalVel / sqrt(pow(xDistance / yDistance, 2) + 1);

	//correct the direction
	if (xDirection > 0 && xVel < 0)
	{
		xVel *= -1;
	}
	else if (xDirection < 0 && xVel > 0)
	{
		xVel *= -1;
	}
	if (yDirection > 0 && yVel < 0)
	{
		yVel *= -1;
	}
	else if (yDirection < 0 && yVel > 0)
	{
		yVel *= -1;
	}

	*xReturnVel = xVel;
	*yReturnVel = yVel;
}





void Projectile::init()
{
	//set amount of animation and frames of each
	//std::vector<int> frameCounts = { 1 };
	//actualy load the projectile with the data
	//loadProjectile(256, 226, 50, 120, "fart", frameCounts, fartUpdate, 0, 1, 1, 0);
	

	//SDL_SetTextureColorMod(entityList[1].tex, 255, 0, 0);
	//SDL_SetTextureColorMod(entityList[2].tex, 0, 255, 0);
	//SDL_SetTextureColorMod(entityList[3].tex, 0, 0, 255);

}

void Projectile::updateProjectiles()
{
	//if things need to happen 60 times a sec
	if (Controller::FPSLock && !Entity::frameByFrame || Entity::frameAdvance)
	{
		for (int i = 0; i < Projectile::projectileList.size(); i++)
		{
			//delete after expired   NEEDS MOVED maybe
			if (clock() >= Projectile::projectileList[i].creationTime + Projectile::projectileList[i].duration)
			{
				unloadProjectile(i);
			}
			else
			{
				for (int j = 0; j < Projectile::projectileList[i].updateList.size(); j++)
				{
					projectileList[i].updateList[j](i);
				}
			}
		}
		//remove this or the other one
		Entity::frameAdvance = false;
	}

}

void Projectile::drawProjectiles()
{
	for (int i = 0; i < Projectile::projectileList.size(); i++)
	{
		Projectile::projectileList[i].draw();
	}
}