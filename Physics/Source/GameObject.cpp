
#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	health(1),
	mass(1.f),
	dir(0, 1, 0),
	momentOfIntertia(1.f),
	angularVelocity(0.f)
{
}

GameObject::~GameObject()
{
}