#pragma once
#include <string>
#ifndef SDL
#include <SDL.h>
#endif // !SDL
#ifndef vector
#include <vector>
#endif
class Artist {
public:
	static int SCREEN_WIDTH, SCREEN_HEIGHT;
	static float SCREEN_SCALE;
	static bool FULLSCREEN;
	static bool drawMenus;
	static bool displayFPS;
	static bool displayCursor;
	static bool frameRateLock;
	SDL_Texture * loadTexture(std::string path);

	struct pos {
		int x;
		int y;
	};

	struct fontTextureSet {
		SDL_Texture* LowerA;
		SDL_Texture* LowerB;
		SDL_Texture* LowerC;
		SDL_Texture* LowerD;
		SDL_Texture* LowerE;
		SDL_Texture* LowerF;
		SDL_Texture* LowerG;
		SDL_Texture* LowerH;
		SDL_Texture* LowerI;
		SDL_Texture* LowerJ;
		SDL_Texture* LowerK;
		SDL_Texture* LowerL;
		SDL_Texture* LowerM;
		SDL_Texture* LowerN;
		SDL_Texture* LowerO;
		SDL_Texture* LowerP;
		SDL_Texture* LowerQ;
		SDL_Texture* LowerR;
		SDL_Texture* LowerS;
		SDL_Texture* LowerT;
		SDL_Texture* LowerU;
		SDL_Texture* LowerV;
		SDL_Texture* LowerW;
		SDL_Texture* LowerX;
		SDL_Texture* LowerY;
		SDL_Texture* LowerZ;
		SDL_Texture* CapA;
		SDL_Texture* CapB;
		SDL_Texture* CapC;
		SDL_Texture* CapD;
		SDL_Texture* CapE;
		SDL_Texture* CapF;
		SDL_Texture* CapG;
		SDL_Texture* CapH;
		SDL_Texture* CapI;
		SDL_Texture* CapJ;
		SDL_Texture* CapK;
		SDL_Texture* CapL;
		SDL_Texture* CapM;
		SDL_Texture* CapN;
		SDL_Texture* CapO;
		SDL_Texture* CapP;
		SDL_Texture* CapQ;
		SDL_Texture* CapR;
		SDL_Texture* CapS;
		SDL_Texture* CapT;
		SDL_Texture* CapU;
		SDL_Texture* CapV;
		SDL_Texture* CapW;
		SDL_Texture* CapX;
		SDL_Texture* CapY;
		SDL_Texture* CapZ;
		SDL_Texture* num1;
		SDL_Texture* num2;
		SDL_Texture* num3;
		SDL_Texture* num4;
		SDL_Texture* num5;
		SDL_Texture* num6;
		SDL_Texture* num7;
		SDL_Texture* num8;
		SDL_Texture* num9;
		SDL_Texture* num0;
		SDL_Texture* Pound;
		SDL_Texture* $;
		SDL_Texture* Percent;
		SDL_Texture* Ampersand;
		SDL_Texture* LeftParentheses;
		SDL_Texture* RightParentheses;
		SDL_Texture* Comma;
		SDL_Texture* Apostrophe;
		SDL_Texture* Minus;
		SDL_Texture* SemiColon;
		SDL_Texture* Colon;
		SDL_Texture* At;
		SDL_Texture* LeftBracket;
		SDL_Texture* RightBracket;
		SDL_Texture* Caret;
		SDL_Texture* UnderScore;
		SDL_Texture* Tilde;
		SDL_Texture* Acute;
		SDL_Texture* LeftBrace;
		SDL_Texture* RightBrace;
		SDL_Texture* Plus;
		SDL_Texture* Equals;
		SDL_Texture* Asterisk;
		SDL_Texture* BackSlash;
		SDL_Texture* ForwardSlash;
		SDL_Texture* ExclamationMark;
		SDL_Texture* QuestionMark;
		SDL_Texture* GreaterThan;
		SDL_Texture* LessThan;
		SDL_Texture* Period;
		SDL_Texture* QuotationMark;
		SDL_Texture* VerticalBar;
	};

	struct RGB {
		int r;
		int g;
		int b;
	};

	

	//Declare textures in a header (maybe here) -> define them in that headers respective file -> load em in the load funcion in the artist file
	//how to declare a texture
	//static SDL_Texture* gTestTexture;
	//how to declare a animation
	//static std::vector<SDL_Texture*> gTestAnimation;
	
	static SDL_Texture* gCursor;


	//fonts
	static fontTextureSet largeFont;
	static fontTextureSet smallFont;



	void drawImage(SDL_Texture* tex, int x, int y, int w = 0, int h = 0, double r = 0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, bool scaled = true);

	void drawAnimation(std::vector<SDL_Texture*> texSet, int x, int y, int frameTime, int animationTime, int width, int height, float r, SDL_RendererFlip flip);

	

	void drawLetters(std::string string, int x, int y, Artist::fontTextureSet font);

	void draw();
	//Clears the screen
	void clearScreen();

	//Updates the screen
	void updateScreen();

	//Starts up SDL and creates window
	bool init();

	std::vector<SDL_Texture*> loadAnimationData(std::string textureFolderLocation, std::string textureSetName, int sectionNumber, int textureSetNumber, int textureAmount);

	//Loads media
	bool loadMedia();

	//Frees media and shuts down SDL
	void close();

	void changeRenderColor(int r, int g, int b, int alpha);

	void drawLineFromPoints(int sX, int sY, int eX, int eY);

	void drawRectangle(int x, int y, int w, int h);

};