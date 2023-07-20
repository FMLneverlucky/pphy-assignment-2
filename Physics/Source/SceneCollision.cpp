#include "SceneCollision.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneCollision::SceneCollision()
{
}

SceneCollision::~SceneCollision()
{
}

void SceneCollision::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	
	Math::InitRNG();

	//Exercise 1: initialize m_objectCount
	m_objectCount = 0;
	//exercise 1 end

	//enable light here
	bLightEnabled = true;

	m_ghost = new GameObject(GameObject::GO_BALL);

	//week 13 
	float angle = Math::QUARTER_PI;//octogon shape thing?
	float wallLength = 40;
	float radius = wallLength * 0.5f / tan(angle * .5f); // refer to week 13 practical A to see octogonal boundary

	for (int i = 0; i < 8; ++i)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->scale.Set(2.f, wallLength + .9f, 1.f);
		go->pos = Vector3(radius * cosf(i * angle) + m_worldWidth / 2, radius * sinf(i * angle) + m_worldHeight / 2, 0.f);
		go->normal = Vector3(cosf(i * angle), sinf(i * angle), 0.f);
		go->color.Set(0, 0, 1);
		go->vel.SetZero(); //no move so yes
	}

	makeThinWall(5.f, 20.f, Vector3(0, 1, 0), Vector3(m_worldWidth / 2, m_worldHeight / 2, 0));
}

GameObject* SceneCollision::FetchGO() //add objects 
{
	//Exercise 2a: implement FetchGO()
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); it++)
	{
		GameObject* go = (GameObject*)*it; //setting pointer to gameobject i think
		if (go->active) //if object is active, skip set active code
			continue;
		go->active = true;	// set object active here -> a bit weird but ok
		++m_objectCount; //since game object active on scene, add to object count
		return go;
	}
	//exercise 2a end

	//limit objects created at a time here
	//Exercise 2b: increase object count every time an object is set to active
	int prevSize = m_goList.size();
	for (int i = 0; i < 10; ++i);
	{
		m_goList.push_back(new GameObject(GameObject::GO_BALL));
	}
	m_goList.at(prevSize)->active = true;
	++m_objectCount;
	//exercise 2b end

	return m_goList[prevSize];
}

void SceneCollision::ReturnGO(GameObject *go) //remove object
{
	//Exercise 3: implement ReturnGO()
	if (go->active)
	{
		go->active = false;
		--m_objectCount;
	}
	//exercise 3 end
}

void SceneCollision::Update(double dt)
{
	SceneBase::Update(dt);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	
	if(Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if(Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}

	//Mouse Section
		//exercise 6a: store mouse pos into ghost
	double x, y, windowwidth, windowheight;	//store mouse position into x and y, windowwidth and windowheight to track mouse position
	Application::GetCursorPos(&x, &y);
	windowwidth = Application::GetWindowWidth();
	windowheight = Application::GetWindowHeight();

	Vector3 mousepos(x * (m_worldWidth / windowwidth), ((windowheight - y) * (m_worldHeight/ windowheight)), 0);

	static bool bLButtonState = false;
	if(!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		//exercise 6b: set position, scale and mass -> also vel but we dont talk about that
		m_ghost->active = true;
		m_ghost->pos = mousepos;
		m_ghost->scale.Set(2, 2, 2);
		m_ghost->mass = 8;
		m_ghost->color.Set(Math::RandFloatMinMax(0,1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));
	}
	else if(bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
		//spawn small ball -> from exercise 6 somewhere idk anymore
		GameObject* go = FetchGO();
		go->type = GameObject::GO_BALL;
		go->pos = m_ghost->pos;
		go->vel = m_ghost->pos - mousepos;
		go->scale = m_ghost->scale;
		go->mass = m_ghost->mass;
		go->color = m_ghost->color;
		m_ghost->active = false;

	}
	static bool bRButtonState = false;
	if(!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
		//exercise 9: right click spawn ball
		m_ghost->active = true;
		m_ghost->pos = mousepos;
		m_ghost->scale.Set(3, 3, 3);
		m_ghost->mass = 27;
		m_ghost->color.Set(Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));
	}
	else if(bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;

		//Exercise 10: spawn large GO_BALL
		float size = Math::Clamp((mousepos - m_ghost->pos).Length(), 2.f, 10.f);
		m_ghost->scale.Set(size, size, size);
		m_ghost->mass = size * size * size;	//damn thats heavy
		//the further the mouse drag away from click position, the faster the vel of ball
	}

	//Physics Simulation Section

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			//Exercise 7a: implement movement for game objects
			go->pos += go->vel * dt * m_speed;
			//Exercise 7bi: handle out of bound game objects
			if (go->pos.x < 0 || go->pos.x > m_worldWidth)
			{
				go->vel.x = -go->vel.x; // simulate bounce back from wall
			}
			//exercise 7bii: despawn object if go out of bounds
			if (go->pos.x < 0 - go->scale.x || go->pos.x > m_worldWidth + go->scale.x)
			{
				ReturnGO(go);
				continue; //is this really necessary?
			}

			//copy paste from above :D
			if (go->pos.y < 0 || go->pos.y > m_worldHeight)
			{
				go->vel.y = -go->vel.y; // simulate bounce back from wall
			}
			if (go->pos.y < 0 - go->scale.y || go->pos.y > m_worldHeight + go->scale.y)
			{
				ReturnGO(go);
				continue;
			}
			//exercise 7 end;

			/*week 13 exercise 4
			for collision response*/
			GameObject* go2 = nullptr; //can be any other obj
			for (std::vector<GameObject*>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
			{
				GameObject* go2 = static_cast<GameObject*>(*it2);
				if (go2->active)
				{
					//when ball collide with wall detected, make ball bounce
					if (checkcollision(go, go2))
					{
						collisionResponse(go, go2);
					}
				}
				GameObject* other(go);//?
				GameObject* other1(go2);//????

				if (go->type != GameObject::GO_BALL)
				{
					other = go2;
					other1 = go;
				}
				if (go2->active && checkcollision(other, other1))
				{
					collisionResponse(other, other1);
				}
			}
			//Exercise 8b: store values in auditing variables

			//Exercise 10: handle collision using momentum swap instead

			//Exercise 12: improve inner loop to prevent double collision

			//Exercise 13: improve collision detection algorithm 
		}
	}
}

//week 13 exercise 2 - create the thin physical wall
void SceneCollision::makeThinWall(float width, float height, const Vector3& normal, const Vector3& pos)
{
	GameObject* thinWall = FetchGO();
	thinWall->type = GameObject::GO_WALL;
	thinWall->scale.Set(width, height, 1.f);
	thinWall->pos = pos;
	thinWall->normal = normal;
	thinWall->color.Set(1, 0, 0);
	thinWall->vel.SetZero();
}

bool SceneCollision::checkcollision(GameObject* go1, GameObject* go2)
{
	//week 13 - check non ball vs ball
	if (go1->type != GameObject::GO_BALL)// if one object is not ball
		return false;

	switch (go2->type)
	{
		case GameObject::GO_BALL:
		{
			//week 12 Exercise 8a: handle collision between GO_BALL and GO_BALL using velocity swap
			Vector3 relativeVel = go2->vel - go1->vel;
			Vector3 distanceDiff = go2->pos - go1->pos;

			if (relativeVel.Dot(distanceDiff) <= 0)
				return false;

			return distanceDiff.LengthSquared() <= (go1->scale.x + go2->scale.x) * (go1->scale.x + go2->scale.x);//returning distance between both objects if no collision
		}
		case GameObject::GO_WALL:
		{
			Vector3 diff = go1->pos - go2->pos;
			Vector3 axisX = go2->normal;	//check which direction its pointing to
			Vector3 axisY = Vector3(-go2->normal.y, go2->normal.x, 0);

			float projectedDist = diff.Dot(axisX);

			if (projectedDist > 0)
				axisX = -axisX;
			return go1->vel.Dot(axisY) >= 0 &&	//check if travelling towards the wall
				go2->scale.x * .5 + go1->scale.x > -diff.Dot(axisX) && //check if radius +thickness vs dist
				go2->scale.y * .5 > fabs(diff.Dot(axisY));	//check the length is within (something she didnt finish her sentence)
		}
	}
}

void SceneCollision::collisionResponse(GameObject* go1, GameObject* go2)
{
	//initial values of objects
	u1 = go1->vel;
	u2 = go2->vel;

	m1 = go1->mass;
	m2 = go2->mass;

	//simulate bouncing of ball
	//vers 1 of collision response -> typed based on formula
	/*
	Vector3 n = go1->pos - go2->pos;//finding normal
	Vector3 un = n.Normalized(); //prevent throwing of 0
	Vector3 ut = Vector3(-un.y, un.x, 0);

	//finding momentum
	float v1n = un.Dot(u1);	//v 1 -> one not l n
	float v1t = ut.Dot(u1);
	float v2n = un.Dot(u2);
	float v2t = ut.Dot(u2);

	// momentum formula in 2d elastic collision pg 3
	float v1nP = (v1n * (m1 - m2) + (2 * m2 * v2n)) / (m1 - m2); 
	float v2nP = (v2n * (m1 - m2) + (2 * m2 * v1n)) / (m1 - m2);

	Vector3 v1nVec = v1nP * un;
	Vector3 v1tVec = v1t * un;

	Vector3 v2nVec = v2nP * un;
	Vector3 v2tVec = v2t * un;

	//final velocity
	go1->vel = v1nVec + v1tVec;
	go2->vel = v2nVec + v2nVec;
	*/

	switch (go2->type)
	{
		case GameObject::GO_BALL:
		{
			//version 2 of check collision response -> shorter version of vers 1
			//this is based on some formula provided in lecture notes or practical 13 -> most likely in practical
			Vector3 n = go1->pos - go2->pos; //finding normal
			Vector3 vec = (u1 - u2).Dot(n)  /(n).LengthSquared() * n;
			go1->vel = u1 - (2 * m2 / (m1 + m2)) * vec; //u = vel, m = mass
			go2->vel = u2 - (2 * m1 / (m2 + m1)) * -vec;
			break;
		}
		case GameObject::GO_WALL:
		{
			go1->vel = u1 - (2. * u1.Dot(go2->normal)) * go2->normal; //formula -> v = u - (2u.N)N
			break;
		}
	}
}


void SceneCollision::RenderGO(GameObject *go)
{
	switch(go->type)
	{
	case GameObject::GO_BALL:
		//Exercise 4: render a sphere using scale and pos
		modelStack.PushMatrix();	//rmb push always comes with pop -> will lead to error if one is missing :::::::)))))
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		//exercise 4 end

		//Exercise 11: think of a way to give balls different colors
		meshList[GEO_BALL]->material.kAmbient.Set(go->color.x, go->color.y, go->color.z); //defining ltr yes yes
		RenderMesh(meshList[GEO_BALL], true);	//parameters: [mesh] [enable lighting]
		modelStack.PopMatrix();
		//exercise 11 end
		break;

	case GameObject::GO_WALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);	//part of some exercise - rotate to normal
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(go->color.x, go->color.y, go->color.z); //setting colour by the material like in any model making software
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	}
}

void SceneCollision::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);
	
	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
						camera.position.x, camera.position.y, camera.position.z,
						camera.target.x, camera.target.y, camera.target.z,
						camera.up.x, camera.up.y, camera.up.z
					);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();
	
	RenderMesh(meshList[GEO_AXES], false);

	//render ghost
	if (m_ghost->active)
	{
		RenderGO(m_ghost);
	}

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}

	//On screen text
	std::ostringstream ss;

	//Exercise 5: Render m_objectCount
	//parameters: [mesh][string][color][text size][x pos][y pos]
	RenderTextOnScreen(meshList[GEO_TEXT], "object count: " + std::to_string(m_objectCount), Color(0, 1, 0), 3, 0, 9);
	//exercise 5 end

	//Exercise 8c: Render initial and final momentum

	ss.precision(3);
	ss << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 6);
	
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);
	
	RenderTextOnScreen(meshList[GEO_TEXT], "Collision", Color(0, 1, 0), 3, 0, 0);
}

void SceneCollision::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}
