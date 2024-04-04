#pragma once
#include <time.h>

int lastFramesCheck = 0, displayFPS = 0, addingFPS = 0;

class FPSCounter {
public:
	int getFPS()
	{
		return displayFPS;
	}
	void updateFPS()
	{
		addingFPS++;

		if (clock() >= lastFramesCheck + 1000)
		{
			displayFPS = addingFPS;
			addingFPS = 0;
			lastFramesCheck = clock();
		}
	}
};