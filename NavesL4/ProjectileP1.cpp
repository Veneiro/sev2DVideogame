#include "ProjectileP1.h"

ProjectileP1::ProjectileP1(float x, float y, Game* game) :
	Actor("res/disparo.png", x, y, 20, 20, game) {
	vx = 0;
	vy = 0; 
}
