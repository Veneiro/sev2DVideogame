#pragma once

#include "Actor.h"
#include "ProjectileP1.h" 
#include "Audio.h"
#include "Animation.h" // incluir animacion 

class Player : public Actor
{
public:
	Player(float x, float y, Game* game);
	ProjectileP1* shoot();
	void update();
	void moveX(float axis);
	void moveY(float axis);
	void draw() override; // Va a sobrescribir
	void loseLife();
	int lifes = 3;
	int invulnerableTime = 0;
	int orientation;
	int state;
	Animation* aIdleRight;
	Animation* aIdleLeft;
	Animation* aIdleTop;
	Animation* aIdleDown;
	Animation* aRunningRight;
	Animation* aRunningLeft;
	Animation* aRunningUp;
	Animation* aRunningDown;
	Animation* aShootingRight;
	Animation* aShootingLeft;
	Animation* aShootingUp;
	Animation* aShootingDown;
	Animation* animation; // Referencia a la animación mostrada
	Audio* audioShoot;
	int shootCadence = 60;
	int shootTime = 0;
};

