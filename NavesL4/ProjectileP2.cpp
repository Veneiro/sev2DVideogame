#include "ProjectileP2.h"

ProjectileP2::ProjectileP2(float x, float y, Game* game) :
	Actor("res/disparo.png", x, y, 20, 20, game) {
	vx = 0;
	vy = 0;
}