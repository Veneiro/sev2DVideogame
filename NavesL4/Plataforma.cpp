#include "Plataforma.h"

Plataforma::Plataforma(float x, float y, Game* game)
	: Actor("res/bloque_metal.png", x, y, 40, 32, game) {

	vx = 1;
}

