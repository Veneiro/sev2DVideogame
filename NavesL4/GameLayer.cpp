#include "GameLayer.h"

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

	buttonJump = new Actor("res/boton_salto.png", WIDTH * 0.9, HEIGHT * 0.55, 100, 100, game);
	buttonShoot = new Actor("res/boton_disparo.png", WIDTH * 0.75, HEIGHT * 0.83, 100, 100, game);

	space = new Space(1);
	scrollX = 0;
	tiles.clear();

	//audioBackground = new Audio("res/musica_ambiente.mp3", true);
	//audioBackground->play();

	points = 0;
	textPoints = new Text("hola", WIDTH * 0.92, HEIGHT * 0.04, game);
	textPoints->content = to_string(points);

	recol = 0;
	textRecolectables = new Text("hola", WIDTH - 60, HEIGHT - 30, game);
	textRecolectables->content = to_string(recol);

	textLifes = new Text("lives", 40, 40, game);
	textLifes->content = "P1:   " + to_string(3);
	
	textLifes2 = new Text("lives", 40, 80, game);
	textLifes2->content = "P2:   " + to_string(3);

	
	background = new Background("res/fondo_2.png", WIDTH * 0.5, HEIGHT * 0.5, -1, game);
	backgroundPoints = new Actor("res/icono_puntos.png",
		WIDTH * 0.85, HEIGHT * 0.05, 24, 24, game);
	backgroundLifes = new Actor("res/corazon.png", 65, 40, 44, 36, game);
	backgroundLifes2 = new Actor("res/corazon.png", 65, 80, 44, 36, game);
	backgroundRecolectables = new Actor("res/icono_recolectable.png", WIDTH - 30, HEIGHT - 30, 40, 40, game);

	enemies.clear(); // Vaciar por si reiniciamos el juego
	projectiles.clear(); // Vaciar por si reiniciamos el juego

	loadMap("res/" + to_string(game->currentLevel) + ".txt");
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
	case 'C': {
		cup = new Tile("res/copa.png", x, y, game);
		// modificación para empezar a contar desde el suelo.
		cup->y = cup->y - cup->height / 2;
		space->addDynamicActor(cup); // Realmente no hace falta
		break;
	}
	case 'E': {
		Enemy* enemy = new Enemy(x, y, game);
		// modificación para empezar a contar desde el suelo.
		enemy->y = enemy->y - enemy->height / 2;
		enemies.push_back(enemy);
		space->addDynamicActor(enemy);
		break;
	}
	case '1': {
		player = new Player(x, y, game);
		// modificación para empezar a contar desde el suelo.
		player->y = player->y - player->height / 2;
		space->addDynamicActor(player);
		break;
	}
	case '#': {
		Tile* tile = new Tile("res/bloque_tierra.png", x, y, game);
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
		Tile* tile = new Tile("res/bloque_tierra.png", x, y, game);
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
	case 'F': {
		flag = new Tile("res/flag2.png", x, y, game);
		// modificación para empezar a contar desde el suelo.
		flag->y = flag->y - flag->height / 2;
		space->addDynamicActor(flag); // Realmente no hace falta
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
		Projectile* newProjectile = player->shoot();
		if (newProjectile != NULL) {
			space->addDynamicActor(newProjectile);
			projectiles.push_back(newProjectile);
		}

	}

	if (controlShoot2) {
		Projectile* newProjectile = player2->shoot();
		if (newProjectile != NULL) {
			space->addDynamicActor(newProjectile);
			projectiles.push_back(newProjectile);
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
	
	}
	else if (controlMoveY < 0) {
		player->jump();
	}
	else if (controlMoveY == 0) {

	}

	if (controlMoveY2 > 0) {

	}
	else if (controlMoveY2 < 0) {
		player2->jump();
	}
	else if (controlMoveY2 = 0) {

	}



}

void GameLayer::update() {
	if (pause) {
		return;
	}

	// Nivel superado
	if (cup->isOverlap(player) && cup->isOverlap(player2)) {
		game->currentLevel++;
		if (game->currentLevel > game->finalLevel) {
			game->currentLevel = 0;
		}
		saved = false;
		message = new Actor("res/mensaje_ganar.png", WIDTH * 0.5, HEIGHT * 0.5,
			WIDTH, HEIGHT, game);
		pause = true;
		init();
	}

	if ((flag->isOverlap(player) || flag->isOverlap(player2)) && !saved) {
		saved = true;
		savedlifes = player->lifes;
		savedlifes2 = player2->lifes;
	}

	//Los dos jugadores se han caido
	if (player->y > HEIGHT + 80 && player2->y > HEIGHT + 80) {
		if (saved) {
			player->x = flag->x;
			player->y = flag->y;
			player2->x = flag->x;
			player2->y = flag->y;
			player->lifes = savedlifes;
			player2->lifes = savedlifes2;
			textLifes->content = "P1:   " + to_string(player->lifes);
			textLifes2->content = "P2:   " + to_string(player2->lifes);
		}
		else {
			message = new Actor("res/mensaje_perder.png", WIDTH * 0.5, HEIGHT * 0.5,
				WIDTH, HEIGHT, game);
			pause = true;
			init();
		}
	}

	// Jugador 1 se cae
	if (player->y > HEIGHT + 80) {
		player->loseLife();
		textLifes->content = "P1:   " + to_string(player->lifes);
		player->x = player2->x + 10;
		player->y = player2->y;
	}

	//Jugador 2 se cae
	if (player2->y > HEIGHT + 80) {
		player2->loseLife();
		textLifes2->content = "P2:   " + to_string(player2->lifes);
		player2->x = player->x + 10;
		player2->y = player->y;
	}

	space->update();
	background->update();
	player->update();
	player2->update();
	for (auto const& enemy : enemies) {
		enemy->update();
	}
	for (auto const& projectile : projectiles) {
		projectile->update();
	}

	for (auto const& recolectable : recolectables) {
		recolectable->update();
	}

	// Colisiones
	timeDestroy--;
	list <Tile*> deleteDestr;
	for (auto const& destructible : destructibles) {
		if (destructible->collisionUp == true && timeDestroy <= 0) {
			bool pInList = std::find(deleteDestr.begin(),
				deleteDestr.end(),
				destructible) != deleteDestr.end();

			if (!pInList) {
				deleteDestr.push_back(destructible);
			}
			timeDestroy = 60;
		}
	}

	for (auto const& enemy : enemies) {
		if (player->isOverlap(enemy) && enemy->state == game->stateMoving) {
			player->loseLife();
			textLifes->content = "P1:   " + to_string(player->lifes);
			if (player->lifes <= 0) {
				if (saved) {
					player->x = flag->x;
					player->y = flag->y;
					player2->x = flag->x;
					player2->y = flag->y;
					player->lifes = savedlifes;
					player2->lifes = savedlifes2;
					textLifes->content = "P1:   " + to_string(player->lifes);
					textLifes2->content = "P2:   " + to_string(player2->lifes);
				}
				else {
					message = new Actor("res/mensaje_perder.png", WIDTH * 0.5, HEIGHT * 0.5,
						WIDTH, HEIGHT, game);
					pause = true;
					init();
					return;
				}
			}
		}
	}

	for (auto const& enemy : enemies) {
		if (player2->isOverlap(enemy) && enemy->state == game->stateMoving) {
			player2->loseLife();
			textLifes2->content = "P2:   " + to_string(player2->lifes);
			if (player2->lifes <= 0) {
				if (saved) {
					player->x = flag->x;
					player->y = flag->y;
					player2->x = flag->x;
					player2->y = flag->y;
					player->lifes = savedlifes;
					player2->lifes = savedlifes2;
					textLifes->content = "P1:   " + to_string(player->lifes);
					textLifes2->content = "P2:   " + to_string(player2->lifes);
				}
				else {
					message = new Actor("res/mensaje_perder.png", WIDTH * 0.5, HEIGHT * 0.5,
						WIDTH, HEIGHT, game);
					pause = true;
					init();
					return;
				}
			}
		}
	}

	//Colision recolectable jugador
	list<Recolectable*> deleteRecol;
	for (auto const& recolectable : recolectables) {
		if (player->isOverlap(recolectable) || player2->isOverlap(recolectable)) {
			recol++;
			textRecolectables->content = to_string(recol);
			bool pInList = std::find(deleteRecol.begin(),
				deleteRecol.end(),
				recolectable) != deleteRecol.end();

			if (!pInList) {
				deleteRecol.push_back(recolectable);
			}
		}
	}


	// Colisiones , Enemy - Projectile

	list<Enemy*> deleteEnemies;
	list<Projectile*> deleteProjectiles;
	for (auto const& projectile : projectiles) {
		if (projectile->isInRender(scrollX) == false || projectile->vx == 0) {

			bool pInList = std::find(deleteProjectiles.begin(),
				deleteProjectiles.end(),
				projectile) != deleteProjectiles.end();

			if (!pInList) {
				deleteProjectiles.push_back(projectile);
			}
		}
	}



	for (auto const& enemy : enemies) {
		for (auto const& projectile : projectiles) {
			if (enemy->isOverlap(projectile) && enemy->state == game->stateMoving) {
				bool pInList = std::find(deleteProjectiles.begin(),
					deleteProjectiles.end(),
					projectile) != deleteProjectiles.end();

				if (!pInList) {
					deleteProjectiles.push_back(projectile);
				}


				enemy->impacted();
				points++;
				textPoints->content = to_string(points);


			}
		}
	}

	for (auto const& enemy : enemies) {
		if (enemy->state == game->stateDead) {
			bool eInList = std::find(deleteEnemies.begin(),
				deleteEnemies.end(),
				enemy) != deleteEnemies.end();

			if (!eInList) {
				deleteEnemies.push_back(enemy);
			}
		}
	}

	for (auto const& delEnemy : deleteEnemies) {
		enemies.remove(delEnemy);
		space->removeDynamicActor(delEnemy);
	}
	deleteEnemies.clear();

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

	for (auto const& delProjectile : deleteProjectiles) {
		projectiles.remove(delProjectile);
		space->removeDynamicActor(delProjectile);
		delete delProjectile;
	}
	deleteProjectiles.clear();


	cout << "update GameLayer" << endl;
}

void GameLayer::calculateScroll() {
	// limite izquierda
	if (player->x > WIDTH * 0.3) {
		if (player->x - scrollX < WIDTH * 0.3) {
			scrollX = player->x - WIDTH * 0.3;
		}
	}

	// limite derecha
	if (player->x < mapWidth - WIDTH * 0.3) {
		if (player->x - scrollX > WIDTH * 0.7) {
			scrollX = player->x - WIDTH * 0.7;
		}
	}

	// limite izquierda
	if (player2->x > WIDTH * 0.3) {
		if (player2->x - scrollX < WIDTH * 0.3) {
			scrollX = player2->x - WIDTH * 0.3;
		}
	}

	// limite derecha
	if (player2->x < mapWidth - WIDTH * 0.3) {
		if (player2->x - scrollX > WIDTH * 0.7) {
			scrollX = player2->x - WIDTH * 0.7;
		}
	}
}


void GameLayer::draw() {
	calculateScroll();

	background->draw();
	for (auto const& tile : tiles) {
		tile->draw(scrollX);
	}

	for (auto const& tile : destructibles) {
		tile->draw(scrollX);
	}

	for (auto const& plataforma : plataformas) {
		plataforma->draw(scrollX);
	}

	for (auto const& projectile : projectiles) {
		projectile->draw(scrollX);
	}
	cup->draw(scrollX);
	flag->draw(scrollX);
	player->draw(scrollX);
	player2->draw(scrollX);
	for (auto const& enemy : enemies) {
		enemy->draw(scrollX);
	}

	for (auto const& recolectable : recolectables) {
		recolectable->draw(scrollX);
	}

	backgroundPoints->draw();
	textPoints->draw();

	backgroundLifes->draw();
	textLifes->draw();

	backgroundLifes2->draw();
	textLifes2->draw();

	backgroundRecolectables->draw();
	textRecolectables->draw();

	// HUD
	if (game->input == game->inputMouse) {
		buttonJump->draw(); // NO TIENEN SCROLL, POSICION FIJA
		buttonShoot->draw(); // NO TIENEN SCROLL, POSICION FIJA
		pad->draw(); // NO TIENEN SCROLL, POSICION FIJA
	}
	if (pause) {
		message->draw();
	}

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
		if (buttonShoot->containsPoint(motionX, motionY)) {
			controlShoot = true;
		}
		if (buttonJump->containsPoint(motionX, motionY)) {
			controlMoveY = -1;
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
		if (buttonShoot->containsPoint(motionX, motionY) == false) {
			controlShoot = false;
		}
		if (buttonJump->containsPoint(motionX, motionY) == false) {
			controlMoveY = 0;
		}

	}
	// Cada vez que levantan el click
	if (event.type == SDL_MOUSEBUTTONUP) {
		if (pad->containsPoint(motionX, motionY)) {
			pad->clicked = false;
			// LEVANTAR EL CLICK TAMBIEN TE PARA
			controlMoveX = 0;
		}

		if (buttonShoot->containsPoint(motionX, motionY)) {
			controlShoot = false;
		}
		if (buttonJump->containsPoint(motionX, motionY)) {
			controlMoveY = 0;
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

