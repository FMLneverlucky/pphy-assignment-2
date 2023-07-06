#ifndef SCENE_ASTEROID_H
#define SCENE_ASTEROID_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneAsteroid : public SceneBase
{
	static const int MAX_SPEED = 10;
	static const int BULLET_SPEED = 50;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;
	static const int MAX_ANGULAR_SPEED = 50;

public:
	SceneAsteroid();
	~SceneAsteroid();

	virtual void Init();
	virtual void Update(double dt);
	void Reset();
	void StartGame();
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO();
protected:

	//Physics
	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	float m_rotation;
	GameObject *m_ship;
	Vector3 m_force;
	int m_objectCount;
	int m_lives;
	int m_score;
	int currentLevel;
	bool firstLevel, secLevel, thirdLevel;
	bool levelLoaded;
	bool loadlevel;
	double timer;
	float zvalue;
	int EnemyCount;

	Vector3 m_torque;
};

#endif