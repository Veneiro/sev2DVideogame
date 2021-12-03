#include "Pad.h"

Pad::Pad(float x, float y, Game* game)
	: Actor("", x, y, 120, 120, game) {

}


float Pad::getOrientationX(float clickX) {
	return clickX - x;
}
