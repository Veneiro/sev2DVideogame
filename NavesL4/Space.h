#pragma once
#include "Actor.h"
#include <list>

class Space
{
public:
	Space(float gravity);
	void update();
	void updateMoveRightNonGravity(Actor* dynamicAct);
	void updateMoveLeftNonGravity(Actor* dynamicAct);
	void updateMoveRight(Actor* dynamicAct);
	void updateMoveLeft(Actor* dynamicAct);
	void updateMoveTop(Actor* dynamicAct);
	void updateMoveDown(Actor* dynamicAct);
	void addDynamicActor(Actor* actor);
	void addStaticActor(Actor* actor);
	void removeDynamicActor(Actor* actor);
	void removeStaticActor(Actor* actor);
	void addNonGravityActors(Actor* actor);
	void removeNonGravityActors(Actor* actor);
	float gravity;
	list<Actor*> dynamicActors;
	list<Actor*> staticActors;
	list<Actor*> nonGravityActors;
};

