#pragma once

#include "Layer.h"
#include "Player.h"
#include "Background.h"

#include "Projectile.h"
#include "Text.h"
#include "Tile.h"
#include "Pad.h"

#include "Audio.h"
#include "Space.h" // importar

#include <fstream> // Leer ficheros
#include <sstream> // Leer líneas / String
#include <list>
#include "Player2.h"
#include "Recolectable.h"
#include "Plataforma.h"

class GameLayer : public Layer
{
public:
	GameLayer(Game* game);
	void init() override;
	void processControls() override;
	void update() override;
	void draw() override;
	void keysToControls(SDL_Event event);
	void mouseToControls(SDL_Event event); // USO DE MOUSE
	void gamePadToControls(SDL_Event event); // USO DE GAMEPAD
	void loadMap(string name);
	void loadMapObject(char character, float x, float y);
	void calculateScroll();
	Actor* message;
	bool pause;
	// Elementos de interfaz
	SDL_GameController* gamePad;
	Pad* pad;
	Actor* buttonJump;
	Actor* buttonShoot;

	Tile* cup; // Elemento de final de nivel
	Tile* flag;
	Space* space;
	float scrollX;
	float scrollY;
	int mapWidth;
	list<Tile*> tiles;
	list<Tile*> destructibles;
	list<Plataforma*> plataformas;
	list<Recolectable*> recolectables;

	Audio* audioBackground;
	Text* textPoints;
	Text* textLifes;
	Text* textLifes2;
	Text* textRecolectables;
	int points;
	int recol;
	int newEnemyTime = 0;
	Player* player;
	Player2* player2;
	Background* background;
	Actor* backgroundPoints;
	Actor* backgroundLifes;
	Actor* backgroundLifes2;
	Actor* backgroundRecolectables;
	list<Projectile*> projectiles;

	bool saved = false;
	int savedlifes = 3;
	int savedlifes2 = 3;
	bool controlContinue = false;
	bool controlContinue2 = false;
	bool controlShoot = false;
	bool controlShoot2 = false;
	int controlMoveY = 0;
	int controlMoveY2 = 0;
	int controlMoveX = 0;
	int controlMoveX2 = 0;
	int timeDestroy = 60;


};

