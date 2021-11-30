#include "Recolectable.h"

Recolectable::Recolectable(float x, float y, Game* game)
	: Actor("res/icono_recolectable.png", x, y, 31, 64, game) {

	aNormal = new Animation("res/recolectable.png", 40, 40, 256, 32, 8, 8, true, game);
	animation = aNormal;
}

void Recolectable::update() {
	animation->update();
	animation = aNormal;
}

void Recolectable::draw(float scrollX) {
	animation->draw(x - scrollX, y);
}
