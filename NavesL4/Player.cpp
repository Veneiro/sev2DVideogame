#include "Player.h"

Player::Player(float x, float y, Game* game)
	: Actor("res/jugador.png", x, y, 35, 35, game) {

	orientation = game->orientationRight;
	state = game->stateMoving;
	audioShoot = new Audio("res/efecto_disparar.wav", false);
	aShootingRight = new Animation("res/player1.png", width, height,
		250, 250, 1, 1, true, game);
	aShootingLeft = new Animation("res/player1_left.png", width, height,
		250, 250, 1, 1, true, game);

	aShootingUp = new Animation("res/player1_up.png", width, height,
		250, 250, 1, 1, true, game);

	aShootingDown = new Animation("res/player1_down.png", width, height,
		250, 250, 1, 1, true, game);

	aIdleRight = new Animation("res/player1.png", width, height,
		250, 250, 1, 1, true, game);

	aIdleLeft = new Animation("res/player1_left.png", width, height,
		250, 250, 1, 1, true, game);

	aIdleTop = new Animation("res/player1_up.png", width, height,
		250, 250, 1, 1, true, game);

	aIdleDown = new Animation("res/player1_down.png", width, height,
		250, 250, 1, 1, true, game);

	aRunningRight = new Animation("res/player1.png", width, height,
		250, 250, 1, 1, true, game);

	aRunningLeft = new Animation("res/player1_left.png", width, height,
		250, 250, 1, 1, true, game);

	aRunningDown = new Animation("res/player1_down.png", width, height,
		250, 250, 1, 1, true, game);

	aRunningUp = new Animation("res/player1_up.png", width, height,
		250, 250, 1, 1, true, game);

	animation = aIdleRight;

}


void Player::update() {


	if (invulnerableTime > 0) {
		invulnerableTime--;
	}

	bool endAnimation = animation->update();


	// Acabo la animación, no sabemos cual
	if (endAnimation) {
		// Estaba disparando
		if (state == game->stateShooting) {
			state = game->stateMoving;
		}
	}


	// Establecer orientación
	if (vy > 0) {
		orientation = game->orientationDown;
	}
	if (vy < 0) {
		orientation = game->orientationTop;
	}
	if (vx > 0) {
		orientation = game->orientationRight;
	}
	if (vx < 0) {
		orientation = game->orientationLeft;
	}

	// Selección de animación basada en estados
	if (state == game->stateShooting) {
		if (orientation == game->orientationTop) {
			animation = aShootingUp;
		}
		if (orientation == game->orientationDown) {
			animation = aShootingDown;
		}
		if (orientation == game->orientationRight) {
			animation = aShootingRight;
		}
		if (orientation == game->orientationLeft) {
			animation = aShootingLeft;
		}
	}
	if (state == game->stateMoving) {
		if (vx != 0) {
			if (orientation == game->orientationRight) {
				animation = aRunningRight;
			}
			if (orientation == game->orientationLeft) {
				animation = aRunningLeft;
			}
		}

		if (vy != 0) {
			if (orientation == game->orientationTop) {
				animation = aRunningUp;
			}
			if (orientation == game->orientationDown) {
				animation = aRunningDown;
			}
		}

		if (vx == 0 && vy == 0) {
			if (orientation == game->orientationRight) {
				animation = aIdleRight;
			}
			if (orientation == game->orientationLeft) {
				animation = aIdleLeft;
			}
			if (orientation == game->orientationTop) {
				animation = aIdleTop;
			}
			if (orientation == game->orientationDown) {
				animation = aIdleDown;
			}
		}
	}


	if (shootTime > 0) {
		shootTime--;
	}

}

void Player::moveX(float axis) {
	vx = axis * 3;
}

void Player::moveY(float axis) {
	vy = axis * 3;
}

ProjectileP1* Player::shoot() {

	if (shootTime == 0) {
		state = game->stateShooting;
		audioShoot->play();
		aShootingLeft->currentFrame = 0; //"Rebobinar" aniamción
		aShootingRight->currentFrame = 0; //"Rebobinar" aniamción
		aShootingUp->currentFrame = 0; //"Rebobinar" aniamción
		aShootingDown->currentFrame = 0; //"Rebobinar" aniamción
		shootTime = shootCadence;
		ProjectileP1* projectile = new ProjectileP1(x, y, game);
		if (orientation == game->orientationLeft) {
			projectile->vx = projectile->vx - 12;
		}
		if (orientation == game->orientationRight) {
			projectile->vx = projectile->vx + 12;
		}
		if (orientation == game->orientationDown) {
			projectile->vy = projectile->vy + 12;
		}
		if (orientation == game->orientationTop) {
			projectile->vy = projectile->vy - 12;
		}
		return projectile;
	}
	else {
		return NULL;
	}
}

void Player::draw() {
	if (invulnerableTime == 0) {
		animation->draw(x, y);
	}
	else {
		if (invulnerableTime % 10 >= 0 && invulnerableTime % 10 <= 5) {
			animation->draw(x, y);
		}
	}
}

void Player::loseLife() {
	if (invulnerableTime <= 0) {
		if (lifes > 0) {
			lifes--;
			invulnerableTime = 100;
			// 100 actualizaciones 
		}
	}
}
