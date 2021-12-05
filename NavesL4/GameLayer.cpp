#include "GameLayer.h"
#include <time.h>

GameLayer::GameLayer(Game* game)
	: Layer(game) {
	//llama al constructor del padre : Layer(renderer)
	pause = true;
	message = new Actor("res/mensaje_como_jugar.png", WIDTH * 0.5, HEIGHT * 0.5,
		WIDTH, HEIGHT, game);

	gamePad = SDL_GameControllerOpen(0);

	init();
}


void GameLayer::init() {
	pad = new Pad(WIDTH * 0.15, HEIGHT * 0.80, game);

	space = new Space(1);
	tiles.clear();
	destructibles.clear();
	plataformas.clear();

	srand(time(NULL));
	int max = 5;
	int random = (rand() % max);
	audioBackground = new Audio("res/" + to_string(random) + ".wav", true);
	audioBackground->play();

	if (p1Rounds == 2 || p2Rounds == 2) {
		audioBackground = new Audio("res/final.wav", true);
		audioBackground->play();
	}

	textLifes = new Text("lives", 95, 80, game);
	textLifes->content = "P1:   " + to_string(3);
	
	textLifes2 = new Text("lives", WIDTH - 126, 80, game);
	textLifes2->content = "P2:   " + to_string(3);

	
	background = new Background("res/suelo.png", WIDTH * 0.5, HEIGHT * 0.5, -1, game);
	backgroundLifes = new Actor("res/corazon.png", 120, 80, 44, 36, game);
	backgroundLifes2 = new Actor("res/corazon.png", WIDTH - 100, 80, 44, 36, game);

	backgroundAmmo = new Actor("res/icono_recolectable.png",
		120, 120, 44, 40, game);
	backgroundAmmo2 = new Actor("res/icono_recolectable.png",
		WIDTH - 100, 120, 44, 40, game);

	textAmmo = new Text("ammo", 76, 120, game);
	textAmmo->content = to_string(10);

	textAmmo2 = new Text("ammo", WIDTH - 146, 120, game);
	textAmmo2->content = to_string(10);

	P1Rounds = new Text("p1R", (WIDTH * 0.5) - 40, 120, game);
	P1Rounds->content = "P1: " + to_string(p1Rounds);

	P2Rounds = new Text("p2R", (WIDTH * 0.5) + 40, 120, game);
	P2Rounds->content = to_string(p2Rounds) + ": P2";

	projectiles1.clear(); // Vaciar por si reiniciamos el juego
	projectiles2.clear(); // Vaciar por si reiniciamos el juego
	hearts.clear();
	powerUpAmmo.clear();
	powerUpDamage.clear();

	srand(time(NULL));
	max = 5;
	random = (rand()%max);

	loadMap("res/" + to_string(random) + ".txt");
}

void GameLayer::loadMap(string name) {
	char character;
	string line;
	ifstream streamFile(name.c_str());
	if (!streamFile.is_open()) {
		cout << "Falla abrir el fichero de mapa" << endl;
		return;
	}
	else {
		// Por línea
		for (int i = 0; getline(streamFile, line); i++) {
			istringstream streamLine(line);
			mapWidth = line.length() * 40; // Ancho del mapa en pixels
			// Por carácter (en cada línea)
			for (int j = 0; !streamLine.eof(); j++) {
				streamLine >> character; // Leer character 
				cout << character;
				float x = 40 / 2 + j * 40; // x central
				float y = 32 + i * 32; // y suelo
				loadMapObject(character, x, y);
			}

			cout << character << endl;
		}
	}
	streamFile.close();
}

void GameLayer::loadMapObject(char character, float x, float y)
{
	switch (character) {
	case '1': {
		player = new Player(x, y, game);
		// modificación para empezar a contar desde el suelo.
		player->y = player->y - player->height / 2;
		space->addDynamicActor(player);
		break;
	}
	case '#': {

		Tile* tile = new Tile("res/bloque_solido.png", x, y, game);
		// modificación para empezar a contar desde el suelo.
		tile->y = tile->y - tile->height / 2;
		tiles.push_back(tile);
		space->addStaticActor(tile);
		break;
	}
	case '?': { //MOVILES
		Plataforma* tile = new Plataforma(x, y, game);
		// modificación para empezar a contar desde el suelo.
		tile->y = tile->y - tile->height / 2;
		plataformas.push_back(tile);
		space->addNonGravityActors(tile);
		break;
	}
	case '!': { //DESTRUCTIBLES
		srand(time(NULL));
		int max = 2;
		int random = (rand() % max);
		Tile* tile = new Tile("res/planeta" + to_string(random) + ".png", x, y, game);
		// modificación para empezar a contar desde el suelo.
		tile->y = tile->y - tile->height / 2;
		destructibles.push_back(tile);
		space->addStaticActor(tile);
		break;
	}
	case '2': {
		player2 = new Player2(x, y, game);
		// modificación para empezar a contar desde el suelo.
		player2->y = player2->y - player2->height / 2;
		space->addDynamicActor(player2);
		break;
	}
	case 'R': {
		Recolectable* recolectable = new Recolectable(x, y, game);
		// modificación para empezar a contar desde el suelo.
		recolectable->y = recolectable->y - recolectable->height / 2;
		recolectables.push_back(recolectable);
		space->addDynamicActor(recolectable); // Realmente no hace falta
		break;
	}
	}
}


void GameLayer::processControls() {
	// obtener controles
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_CONTROLLERDEVICEADDED) {
			gamePad = SDL_GameControllerOpen(0);
			if (gamePad == NULL) {
				cout << "error en GamePad" << endl;
			}
			else {
				cout << "GamePad conectado" << endl;
			}
		}

		// Cambio automático de input
		// PONER el GamePad
		if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERAXISMOTION) {
			game->input = game->inputGamePad;
		}
		if (event.type == SDL_KEYDOWN) {
			game->input = game->inputKeyboard;
		}
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			game->input = game->inputMouse;
		}
		// Procesar teclas
		if (game->input == game->inputKeyboard) {
			keysToControls(event);
		}
		if (game->input == game->inputMouse) {
			mouseToControls(event);
		}
		// Procesar Mando
		if (game->input == game->inputGamePad) {  // gamePAD
			gamePadToControls(event);
		}


	}
	//procesar controles
	//procesar controles
	// Disparar
	if (controlContinue) {
		pause = false;
		controlContinue = false;
	}
	if (controlShoot) {
		ProjectileP1* newProjectile = player->shoot();
		if (newProjectile != NULL) {
			space->addDynamicActor(newProjectile);
			projectiles1.push_back(newProjectile);
		}

	}

	if (controlShoot2) {
		ProjectileP2* newProjectile = player2->shoot();
		if (newProjectile != NULL) {
			space->addDynamicActor(newProjectile);
			projectiles2.push_back(newProjectile);
		}

	}

	// Eje X
	if (controlMoveX > 0) {
		player->moveX(1);
	}
	else if (controlMoveX < 0) {
		player->moveX(-1);
	}
	else if (controlMoveX == 0) {
		player->moveX(0);
	}

	if (controlMoveX2 > 0) {
		player2->moveX(1);
	}
	else if (controlMoveX2 < 0) {
		player2->moveX(-1);
	}
	else if (controlMoveX2 == 0){
		player2->moveX(0);
	}

	// Eje Y
	if (controlMoveY > 0) {
		player->moveY(1);
	}
	else if (controlMoveY < 0) {
		player->moveY(-1);
	}
	else if (controlMoveY == 0) {
		player->moveY(0);
	}

	if (controlMoveY2 > 0) {
		player2->moveY(1);
	}
	else if (controlMoveY2 < 0) {
		player2->moveY(-1);
	}
	else if (controlMoveY2 == 0) {
		player2->moveY(0);
	}



}

void GameLayer::update() {	

	//Gana uno de los jugadores
	if (p1Rounds == 3) {
		pause = true;
		message = new Actor("res/Ganador1.png", WIDTH * 0.5, HEIGHT * 0.5,
			WIDTH, HEIGHT, game);
	}
	else if (p2Rounds == 3) {
		pause = true;
		message = new Actor("res/Ganador2.png", WIDTH * 0.5, HEIGHT * 0.5,
			WIDTH, HEIGHT, game);
	}

	if (pause) {
		return;
	}

	space->update();
	player->update();
	player2->update();

	//Gana una ronda
	if (player->state == game->stateDead) {
		p2Rounds++;
		srand(time(NULL));
		int max = 5;
		int random = (rand() % max);
		game->currentLevel = random;
		init();
		return;
	}
	else if (player2->state == game->stateDead) {
		p1Rounds++;
		srand(time(NULL));
		int max = 5;
		int random = (rand() % max);
		game->currentLevel = random;
		init();
		return;
	}

	//Update Ammo Left for both players
	textAmmo->content = to_string(player->ammoLeftP1);
	textAmmo2->content = to_string(player2->ammoLeftP2);

	//Update Ammo Objects
	for (auto const& recolectable : recolectables) {
		recolectable->update();
	}

	//Maps Objects Generation
	newHeartTime--;
	newAmmoTime--;
	if (newHeartTime <= 0) {
		int rX = (rand() % WIDTH);
		int rY = (rand() % HEIGHT);
		hearts.push_back(new Heart(rX, rY, game));
		newHeartTime = 500;
	}
	if (newAmmoTime <= 0) {
		int rX = (rand() % WIDTH);
		int rY = (rand() % HEIGHT);
		recolectables.push_back(new Recolectable(rX, rY, game));
		newAmmoTime = 300;
	}
	if (newPuAmmoTime <= 0) {
		int rX = (rand() % WIDTH);
		int rY = (rand() % HEIGHT);
		powerUpAmmo.push_back(new PU_Ammo(rX, rY, game));
		newPuAmmoTime = 1300;
	}
	if (newPuDamageTime <= 0) {
		int rX = (rand() % WIDTH);
		int rY = (rand() % HEIGHT);
		powerUpDamage.push_back(new PU_Damage(rX, rY, game));
		newPuDamageTime = 1500;
	}

	//Power Ups Cooldown
	if (powerUpDamage1 == true) {
		cooldownDamage1--;
	}
	if (cooldownDamage1 <= 0) {
		powerUpDamage1 = false;
		cooldownDamage1 = 250;
	}
	if (powerUpDamage2 == true) {
		cooldownDamage2--;
	}
	if (cooldownDamage2 <= 0) {
		powerUpDamage2 = false;
		cooldownDamage2 = 250;
	}

	if (cooldownAmmo1 <= 0) {
		powerUpAmmo1 = false;
		cooldownAmmo1 = 500;
		player->shootCadence = 60;
	}
	if (cooldownAmmo2 <= 0) {
		powerUpAmmo2 = false;
		cooldownAmmo2 = 500;
		player2->shootCadence = 20;
	}

	if (powerUpAmmo1 == true) {
		cooldownAmmo1--;
		player->shootCadence = 20;
		player->ammoLeftP1 = 10;
	}
	if (powerUpAmmo2 == true) {
		cooldownAmmo2--;
		player2->shootCadence = 20;
		player2->ammoLeftP2 = 10;
	}

	// Colisiones
	timeDestroy--;
	list <Tile*> deleteDestr;
	for (auto const& destructible : destructibles) {
		if ((destructible->collisionUp == true || destructible->collisionDown == true || destructible->collisionRight == true || destructible->collisionLeft == true) && timeDestroy <= 0) {
			bool pInList = std::find(deleteDestr.begin(),
				deleteDestr.end(),
				destructible) != deleteDestr.end();

			if (!pInList) {
				deleteDestr.push_back(destructible);
			}
			timeDestroy = 20;
		}
	}

	//Colision Munición jugador
	list<Recolectable*> deleteRecol;
	for (auto const& recolectable : recolectables) {
		if (player->isOverlap(recolectable)) {
			player->ammoLeftP1++;
			textAmmo->content = to_string(player->ammoLeftP1);
			bool pInList = std::find(deleteRecol.begin(),
				deleteRecol.end(),
				recolectable) != deleteRecol.end();

			if (!pInList) {
				deleteRecol.push_back(recolectable);
			}
		}
	}

	for (auto const& recolectable : recolectables) {
		if (player2->isOverlap(recolectable)) {
			player2->ammoLeftP2++;
			textAmmo->content = to_string(player->ammoLeftP1);
			bool pInList = std::find(deleteRecol.begin(),
				deleteRecol.end(),
				recolectable) != deleteRecol.end();

			if (!pInList) {
				deleteRecol.push_back(recolectable);
			}
		}
	}

	//Colision Jugadores con Corazón
	list<Heart*> deleteHeart;
	for (auto const& heart : hearts) {
		if (player->isOverlap(heart)) {
			player->lifes++;
			textLifes->content = "P1:   " + to_string(player->lifes);
			bool pInList = std::find(deleteHeart.begin(),
				deleteHeart.end(),
				heart) != deleteHeart.end();

			if (!pInList) {
				deleteHeart.push_back(heart);
			}
		}
	}
	for (auto const& heart : hearts) {
		if (player2->isOverlap(heart)) {
			player2->lifes++;
			textLifes2->content = "P2:   " + to_string(player2->lifes);
			bool pInList = std::find(deleteHeart.begin(),
				deleteHeart.end(),
				heart) != deleteHeart.end();

			if (!pInList) {
				deleteHeart.push_back(heart);
			}
		}
	}

	//Colision Jugador con Cadence Power UP
	list<PU_Ammo*> deletePuAmmo;
	for (auto const& ammo : powerUpAmmo) {
		if (player->isOverlap(ammo)) {
			
			powerUpAmmo1 = true;

			bool pInList = std::find(deletePuAmmo.begin(),
				deletePuAmmo.end(),
				ammo) != deletePuAmmo.end();

			if (!pInList) {
				deletePuAmmo.push_back(ammo);
			}
		}
	}
	for (auto const& ammo : powerUpAmmo) {
		if (player2->isOverlap(ammo)) {

			powerUpAmmo2 = true;

			bool pInList = std::find(deletePuAmmo.begin(),
				deletePuAmmo.end(),
				ammo) != deletePuAmmo.end();

			if (!pInList) {
				deletePuAmmo.push_back(ammo);
			}
		}
	}

	//Colision Player with Damage power Up
	list<PU_Damage*> deletePuDamage;
	for (auto const& damage : powerUpDamage) {
		if (player->isOverlap(damage)) {

			powerUpDamage1 = true;

			bool pInList = std::find(deletePuDamage.begin(),
				deletePuDamage.end(),
				damage) != deletePuDamage.end();

			if (!pInList) {
				deletePuDamage.push_back(damage);
			}
		}
	}
	for (auto const& damage : powerUpDamage) {
		if (player2->isOverlap(damage)) {

			powerUpDamage2 = true;

			bool pInList = std::find(deletePuDamage.begin(),
				deletePuDamage.end(),
				damage) != deletePuDamage.end();

			if (!pInList) {
				deletePuDamage.push_back(damage);
			}
		}
	}


	// Colisiones Proyectiles
	
	list<ProjectileP1*> deleteProjectiles1;
	list<ProjectileP2*> deleteProjectiles2;
	for (auto const& projectile : projectiles2) {
		if (player->isOverlap(projectile)) {
			if (powerUpDamage2 == true) {
				player->lifes--;
				player->loseLife();
				textLifes->content = "P1:   " + to_string(player->lifes);
			}
			else if(powerUpDamage2 == false) {
				player->loseLife();
				textLifes->content = "P1:   " + to_string(player->lifes);
			}
			bool pInList = std::find(deleteProjectiles2.begin(),
				deleteProjectiles2.end(),
				projectile) != deleteProjectiles2.end();

			if (!pInList) {
				deleteProjectiles2.push_back(projectile);
			}

			if (player->lifes <= 0) {
				player->state = game->stateDead;
			}
		}

		if (projectile->isInRender() == false || (projectile->vx == 0 && projectile->vy == 0)) {

			bool pInList = std::find(deleteProjectiles2.begin(),
				deleteProjectiles2.end(),
				projectile) != deleteProjectiles2.end();

			if (!pInList) {
				deleteProjectiles2.push_back(projectile);
			}
		}
	}

	for (auto const& projectile : projectiles1) {
		if (player2->isOverlap(projectile)) {
			if (powerUpDamage1 == true) {
				player2->lifes--;
				player2->loseLife();
				textLifes2->content = "P2:   " + to_string(player2->lifes);
			}
			else if (powerUpDamage1 == false) {
				player2->loseLife();
				textLifes2->content = "P2:   " + to_string(player2->lifes);
			}

			bool pInList = std::find(deleteProjectiles1.begin(),
				deleteProjectiles1.end(),
				projectile) != deleteProjectiles1.end();

			if (!pInList) {
				deleteProjectiles1.push_back(projectile);
			}

			if (player2->lifes <= 0) {
				player2->state = game->stateDead;
			}
		}

		if (projectile->isInRender() == false || (projectile->vx == 0 && projectile->vy == 0)) {

			bool pInList = std::find(deleteProjectiles1.begin(),
				deleteProjectiles1.end(),
				projectile) != deleteProjectiles1.end();

			if (!pInList) {
				deleteProjectiles1.push_back(projectile);
			}
		}
	}

	for (auto const& delHeart : deleteHeart) {
		hearts.remove(delHeart);
	}
	deleteHeart.clear();

	for (auto const& delDestr : deleteDestr) {
		destructibles.remove(delDestr);
		space->removeStaticActor(delDestr);
	}
	deleteDestr.clear();

	for (auto const& delRecol : deleteRecol) {
		recolectables.remove(delRecol);
		space->removeDynamicActor(delRecol);
	}
	deleteRecol.clear();

	for (auto const& delProjectile : deleteProjectiles1) {
		projectiles1.remove(delProjectile);
		space->removeDynamicActor(delProjectile);
		delete delProjectile;
	}
	deleteProjectiles1.clear();

	for (auto const& delProjectile : deleteProjectiles2) {
		projectiles2.remove(delProjectile);
		space->removeDynamicActor(delProjectile);
		delete delProjectile;
	}
	deleteProjectiles2.clear();

	for (auto const& delPuAmmo : deletePuAmmo) {
		powerUpAmmo.remove(delPuAmmo);
		delete delPuAmmo;
	}
	deletePuAmmo.clear();

	for (auto const& delPuDamage : deletePuDamage) {
		powerUpDamage.remove(delPuDamage);
		delete delPuDamage;
	}
	deletePuDamage.clear();


	cout << "update GameLayer" << endl;
}


void GameLayer::draw() {

	background->draw();
	for (auto const& tile : tiles) {
		tile->draw();
	}

	for (auto const& tile : destructibles) {
		tile->draw();
	}

	for (auto const& plataforma : plataformas) {
		plataforma->draw();
	}

	for (auto const& projectile : projectiles1) {
		projectile->draw();
	}

	for (auto const& projectile : projectiles2) {
		projectile->draw();
	}

	for (auto const& heart : hearts) {
		heart->draw();
	}

	for (auto const& ammo : powerUpAmmo) {
		ammo->draw();
	}

	for (auto const& damage : powerUpDamage) {
		damage->draw();
	}

	player->draw();
	player2->draw();

	for (auto const& recolectable : recolectables) {
		recolectable->draw();
	}


	backgroundLifes->draw();
	textLifes->draw();

	backgroundLifes2->draw();
	textLifes2->draw();

	backgroundAmmo->draw();
	textAmmo->draw();

	backgroundAmmo2->draw();
	textAmmo2->draw();

	// HUD
	if (game->input == game->inputMouse) {
		pad->draw(); // NO TIENEN SCROLL, POSICION FIJA
	}
	if (pause) {
		message->draw();
	}

	P1Rounds->draw();
	P2Rounds->draw();

	SDL_RenderPresent(game->renderer); // Renderiza
}

void GameLayer::gamePadToControls(SDL_Event event) {

	// Leer los botones
	bool buttonA = SDL_GameControllerGetButton(gamePad, SDL_CONTROLLER_BUTTON_A);
	bool buttonB = SDL_GameControllerGetButton(gamePad, SDL_CONTROLLER_BUTTON_B);
	// SDL_CONTROLLER_BUTTON_A, SDL_CONTROLLER_BUTTON_B
	// SDL_CONTROLLER_BUTTON_X, SDL_CONTROLLER_BUTTON_Y
	cout << "botones:" << buttonA << "," << buttonB << endl;
	int stickX = SDL_GameControllerGetAxis(gamePad, SDL_CONTROLLER_AXIS_LEFTX);
	cout << "stickX" << stickX << endl;

	// Retorna aproximadamente entre [-32800, 32800], el centro debería estar en 0
	// Si el mando tiene "holgura" el centro varia [-4000 , 4000]
	if (stickX > 4000) {
		controlMoveX = 1;
	}
	else if (stickX < -4000) {
		controlMoveX = -1;
	}
	else {
		controlMoveX = 0;
	}

	if (buttonA) {
		controlShoot = true;
	}
	else {
		controlShoot = false;
	}

	if (buttonB) {
		controlMoveY = -1; // Saltar
	}
	else {
		controlMoveY = 0;
	}
}


void GameLayer::mouseToControls(SDL_Event event) {
	// Modificación de coordenadas por posible escalado
	float motionX = event.motion.x / game->scaleLower;
	float motionY = event.motion.y / game->scaleLower;
	// Cada vez que hacen click
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		controlContinue = true;
		if (pad->containsPoint(motionX, motionY)) {
			pad->clicked = true;
			// CLICK TAMBIEN TE MUEVE
			controlMoveX = pad->getOrientationX(motionX);
		}

	}
	// Cada vez que se mueve
	if (event.type == SDL_MOUSEMOTION) {
		if (pad->clicked && pad->containsPoint(motionX, motionY)) {
			controlMoveX = pad->getOrientationX(motionX);
			// Rango de -20 a 20 es igual que 0
			if (controlMoveX > -20 && controlMoveX < 20) {
				controlMoveX = 0;
			}

		}
		else {
			pad->clicked = false; // han sacado el ratón del pad
			controlMoveX = 0;
		}

	}
	// Cada vez que levantan el click
	if (event.type == SDL_MOUSEBUTTONUP) {
		if (pad->containsPoint(motionX, motionY)) {
			pad->clicked = false;
			// LEVANTAR EL CLICK TAMBIEN TE PARA
			controlMoveX = 0;
		}
	}
}


void GameLayer::keysToControls(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		int code = event.key.keysym.sym;
		// Pulsada
		switch (code) {
		case SDLK_ESCAPE:
			game->loopActive = false;
			break;
		case SDLK_1:
			game->scale();
			break;
		case SDLK_d: // derecha
			controlMoveX = 1;
			break;
		case SDLK_a: // izquierda
			controlMoveX = -1;
			break;
		case SDLK_w: // arriba
			controlMoveY = -1;
			break;
		case SDLK_s: // abajo
			controlMoveY = 1;
			break;
		case SDLK_SPACE: // dispara
			controlShoot = true;
			break;
		case SDLK_RIGHT:
			controlMoveX2 = 1;
			break;
		case SDLK_LEFT:
			controlMoveX2 = -1;
			break;
		case SDLK_UP:
			controlMoveY2 = -1;
			break;
		case SDLK_DOWN:
			controlMoveY2 = 1;
			break;
		case SDLK_RSHIFT:
			controlShoot2 = true;
			break;
		}


	}
	if (event.type == SDL_KEYUP) {
		int code = event.key.keysym.sym;
		// Levantada
		switch (code) {
		case SDLK_d: // derecha
			if (controlMoveX == 1) {
				controlMoveX = 0;
			}
			break;
		case SDLK_a: // izquierda
			if (controlMoveX == -1) {
				controlMoveX = 0;
			}
			break;
		case SDLK_w: // arriba
			if (controlMoveY == -1) {
				controlMoveY = 0;
			}
			break;
		case SDLK_s: // abajo
			if (controlMoveY == 1) {
				controlMoveY = 0;
			}
			break;
		case SDLK_SPACE: // dispara
			controlShoot = false;
			break;
		case SDLK_RIGHT:
			if (controlMoveX2 == 1) {
				controlMoveX2 = 0;
			}
			break;
		case SDLK_LEFT:
			if (controlMoveX2 == -1) {
				controlMoveX2 = 0;
			}
			break;  
		case SDLK_UP:
			if (controlMoveY2 == -1) {
				controlMoveY2 = 0;
			}
			break;
		case SDLK_DOWN:
			if (controlMoveY2 == 1) {
				controlMoveY2 = 0;
			}
			break;
		case SDLK_RSHIFT:
			controlShoot2 = false;
			break;
		}

	}

}

