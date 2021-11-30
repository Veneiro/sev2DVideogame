#pragma once
#include "Actor.h"
#include "Animation.h"

class Tile : public Actor
{
public:
	Tile(string filename, float x, float y, Game* game);
};

