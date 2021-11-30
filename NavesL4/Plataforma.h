#pragma once
#include "Actor.h"
class Plataforma :
    public Actor
{
public:
    Plataforma(float x, float y, Game* game);
    float vxIntelligence;
};

