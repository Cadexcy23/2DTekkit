#include "Button.h"
#include "Controller.h"

Button::button Button::makeButton(std::vector<SDL_Texture*> texSet, int x, int y, int frameTime, void(*activateFunc)(), int w, int h)
{
	button tempButton;

	tempButton.texSet = texSet;
	tempButton.frameTime = frameTime;
	tempButton.x = x;
	tempButton.y = y;
	if (w == 0)
	{
		SDL_QueryTexture(texSet[0], NULL, NULL, &tempButton.w, NULL);
	}
	else
	{
		tempButton.w = w;
	}
	if (h == 0)
	{
		SDL_QueryTexture(texSet[0], NULL, NULL, NULL, &tempButton.h);
	}
	else
	{
		tempButton.h = h;
	}
	tempButton.activate = activateFunc;

	return tempButton;
}

void Button::updateButtonVector(std::vector<Button::button> buttonList)
{
	for (int i = 0; i < buttonList.size(); i++)
	{
		if (Controller::mouseX >= buttonList[i].x && Controller::mouseX < buttonList[i].x + buttonList[i].w && Controller::mouseY > buttonList[i].y && Controller::mouseY < buttonList[i].y + buttonList[i].h)
		{
			buttonList[i].drawHovered();
			if (Controller::mouseStates[0] == 1)
			{
				buttonList[i].activate();
			}
		}
		else
		{
			buttonList[i].draw();
		}
	}
}


