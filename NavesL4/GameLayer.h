#pragma once

#include "Layer.h"
#include "Player.h"
#include "Background.h"

#include "ProjectileP1.h"
#include "ProjectileP2.h"
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
	Actor* message;
	bool pause;
	// Elementos de interfaz
	SDL_GameController* gamePad;
	Pad* pad;

	Space* space;
	int mapWidth;
	list<Tile*> tiles;
	list<Tile*> destructibles;
	list<Plataforma*> plataformas;
	list<Recolectable*> recolectables;

	Audio* audioBackground;
	Text* textLifes;
	Text* textLifes2;
	int recol;
	int newEnemyTime = 0;
	Player* player;
	Player2* player2;
	Background* background;
	Actor* backgroundLifes;
	Actor* backgroundLifes2;
	list<ProjectileP1*> projectiles1;
	list<ProjectileP2*> projectiles2;

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

