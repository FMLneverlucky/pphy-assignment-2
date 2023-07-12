#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_CUBE,
		GO_ASTEROID,
		GO_SHIP,
		GO_BULLET,
		GO_ENEMY_BULLET,
		GO_ENEMY,
		GO_MISSLE,
		GO_POWERUP,
		GO_TOTAL, //must be last
	};
	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	Vector3 dir;
	Vector3 color;
	int health;
	bool active;
	float mass;
	float momentOfIntertia;
	float angularVelocity;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif