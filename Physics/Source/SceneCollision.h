#ifndef SCENE_COLLISION_H
#define SCENE_COLLISION_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneCollision : public SceneBase
{
public:
	SceneCollision();
	~SceneCollision();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO();
	void ReturnGO(GameObject *go);

	// No.8
	bool CheckCollision(GameObject* go1, GameObject* go2);
	void CollisionResponse(GameObject* go1, GameObject* go2);
	//

	// Week 13 Exercise 2
	void MakeThinWall(float width, float height, const Vector3& normal, const Vector3& pos);

	//destroy ball that return to starting state
	bool destroyShootingBall(GameObject* go);

	//detect ball outside of window
	bool reachWindowWidthBoundary(GameObject* go, float windowWidth);

	bool hitWallBoundary(GameObject* go, float min_x, float max_x);

	bool skipTurn(GameObject* go);

	void DividePlayArea();
	void addRowOfBricks(int hp);

	void updateBrickPos();

protected:
	enum GAME_STATE
	{
		AIMING, //before ball release
		SHOOTING, //after ball release
		WAITING, //after 1st ball returns to starting line
		GAME_OVER,
		NUM_STATES
	};

	GAME_STATE currentGameState;

	//Physics
	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ghost;
	int m_objectCount;

	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2;

	//starting line
	float startingLine_pos;

	//store min and max of play area
	float minX, maxX;

	std::vector<GameObject* > grid; //stores every possible position a brick can be in
};

#endif