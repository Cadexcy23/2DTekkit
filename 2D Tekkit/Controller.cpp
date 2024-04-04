#include <SDL.h>
#include <stdio.h>
#include "Controller.h"
#include "Artist.h"
#include "Map.h"
#include "Entity.h"
#include <time.h>

//variables used to read input from user
int Controller::mouseX = 0;
int Controller::mouseY = 0;
bool Controller::mouseMoved = false;
Artist::pos Controller::lastMousePos = { Controller::mouseX, Controller::mouseY };
Uint32 Controller::SDLMouseStates;
const Uint8* SDLKeyStates;
std::vector<int> Controller::keyboardStates;
std::vector<int> Controller::mouseStates;
Artist::pos Controller::mouseWheelMovment;

//use in an if statment around anything you want to limit to ~62.5 fps or 16 ms gaps
bool Controller::FPSLock = true;
int lastLockUpdate = 0;

//Keeps track if game is running
bool Controller::quit = false;
//Event handler
SDL_Event e;

//loads tables and such to hold input data
bool Controller::loadController()
{
	Controller::keyboardStates.resize(SDL_NUM_SCANCODES);

	for (int i = 0; i < Controller::keyboardStates.size(); i++)
	{
		Controller::keyboardStates[i] = 0;
	}

	Controller::mouseStates.resize(3);

	for (int i = 0; i < Controller::mouseStates.size(); i++)
	{
		Controller::mouseStates[i] = 0;
	}

	return true;
}

void Controller::controller()
{
	//Declare classes
	Map map;
	Entity entity;


	//Read Keyboard Inputs
	SDLKeyStates = SDL_GetKeyboardState(NULL);
	//Mouse Input and pos
	lastMousePos = { mouseX, mouseY };
	SDLMouseStates = SDL_GetMouseState(&Controller::mouseX, &Controller::mouseY);
	mouseX *= float(1920) / float(Artist::SCREEN_WIDTH);
	mouseY *= float(1080) / float(Artist::SCREEN_HEIGHT);


	//debug printing to console of the mouse pos
	//printf("Mouse X: %i Mouse Y: %i\n", Controller::mouseX, Controller::mouseY);

	//mouse scrolling
	mouseWheelMovment.y = 0;

	//keeps track of mouse dragging
	for (int i = 0; i < mouseStates.size(); i++)
	{
		//clear released buttons
		if (mouseStates[i] == 3)
		{
			mouseStates[i] = 0;
		}
		//set buttons to being held from clicked
		if (mouseStates[i] == 1)
		{
			mouseStates[i] = 2;
		}
	}

	//Handle events on queue
	if (SDL_PollEvent(&e) != 0)
	{
		//debug printing to console of the event type, usefull to find and event ID of and action
		//printf("input detected: %i\n", e.type);

		//User requests quit
		if (e.type == SDL_QUIT)
		{
			Controller::quit = true;
		}

		//read mouse clicks/releases
		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			mouseStates[e.button.button - 1] = 1;
		}
		if (e.type == SDL_MOUSEBUTTONUP)
		{
			mouseStates[e.button.button - 1] = 3;
		}

		//read mouse wheel up or down
		if (e.type == SDL_MOUSEWHEEL)
		{
			mouseWheelMovment.y = e.wheel.y;
		}
	}

	//Update keyboard states  MAYBE MAKE IT WORK LIKE MOUSE DOES NOW
	std::vector<int> tempStates;

	tempStates.resize(SDL_NUM_SCANCODES);

	for (int i = 0; i < tempStates.size(); i++)
	{
		tempStates[i] = 0;
	}

	for (int i = 0; i < tempStates.size(); i++)
	{
		if (keyboardStates[i] == 0 || keyboardStates[i] == 3)
		{
			if (SDLKeyStates[i] && e.type == SDL_KEYDOWN)
			{
				tempStates[i] = 1;
			}
		}
		else if (SDLKeyStates[i])
		{
			tempStates[i] = 2;
		}
		else if (keyboardStates[i] == 1 || keyboardStates[i] == 2)
		{
			if (e.type == SDL_KEYUP)
			{
				tempStates[i] = 3;
			}
		}
	}

	Controller::keyboardStates = tempStates;

	//draws keys and their states for debugging
	/*for (int i = 0; i < Controller::keyboardStates.size(); i++)
	{
		if (Controller::keyboardStates[i] != 0)
		{
			printf("%i: %i \n", i, Controller::keyboardStates[i]);
		}
		artist.drawLetters(std::to_string(i) + ":" + std::to_string(Controller::keyboardStates[i]), 100 * (i / 27), 20 * (i % 27), Artist::smallFont);

	}*/

	if (e.type == 1024)
	{
		mouseMoved = true;
	}
	else
	{
		mouseMoved = false;
	}

	//used to keep things running at a set rate
	if (clock() > lastLockUpdate + 16)
	{
		lastLockUpdate = clock();
		FPSLock = true;
	}
	else
	{
		FPSLock = false;
	}
	
	//put controllers from other classes here
	map.controller();
	entity.controller();
}