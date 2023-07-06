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

	//Physics code here
	m_speed = 1.f;
	
	Math::InitRNG();

	//Exercise 1: initialize m_objectCount
	m_objectCount = 0;

	m_ghost = new GameObject(GameObject::GO_BALL);
}

GameObject* SceneCollision::FetchGO()
{
	//Exercise 2a: implement FetchGO()
	for (int i = 0; i < 10; i++) 
	{
		GameObject* newGO = new GameObject(GameObject::GO_BALL);
		m_goList.push_back(newGO);
	}

	//Exercise 2b: increase object count every time an object is set to active
	m_objectCount++;

	return m_goList.back();
}

void SceneCollision::ReturnGO(GameObject *go)
{
	//Exercise 3: implement ReturnGO()
	go->active = false;
	m_objectCount--;
}

void SceneCollision::Update(double dt)
{
	SceneBase::Update(dt);
	
	if(Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if(Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}

	//Mouse Section
	static bool bLButtonState = false;
	if(!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();

		m_ghost->pos.Set(x / w * m_worldWidth, m_worldHeight * 0.5f);

		std::cout << "LBUTTON DOWN" << std::endl;
	}
	else if(bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();

		//Exercise 6: spawn small GO_BALL
		GameObject* go = FetchGO();
		go->active = true;
		go->pos = m_ghost->pos;

		go->vel.x = m_ghost->pos.x - (x / w * m_worldWidth);
		go->vel.y = 0;
		go->scale.Set(2, 2, 2);
		go->mass = 8;
	}
	static bool bRButtonState = false;
	if(!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();

		m_ghost->pos.Set(x / w * m_worldWidth, m_worldHeight * 0.5f);

		std::cout << "RBUTTON DOWN" << std::endl;
	}
	else if(bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;

		//Exercise 10: spawn large GO_BALL
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();

		//Exercise 6: spawn small GO_BALL
		GameObject* go = FetchGO();
		go->active = true;
		go->pos = m_ghost->pos;

		go->vel.x = m_ghost->pos.x - (x / w * m_worldWidth);
		go->vel.y = 0;
		go->scale.Set(3, 3, 3);
		go->mass = 27;

	}

	//Physics Simulation Section

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			go->pos += go->vel * dt * m_speed;

			//Exercise 7: handle out of bound game objects
			if (go->pos.x <= 0 || go->pos.x >= m_worldWidth || go->pos.y <= 0 || go->pos.y >= m_worldHeight)
			{
				go->vel.Set(-go->vel.x, -go->vel.y, -go->vel.z);
			}

			if (go->pos.x < go->scale.x && go->vel.x < 0 || go->vel.x )
			//Exercise 8a: handle collision between GO_BALL and GO_BALL using velocity swap
			for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
			{
				GameObject* go2 = (GameObject*)*it2;

				if (go2->active && go != go2)
				{
					float distSq = (go2->pos - go->pos).LengthSquared();
					float combinedDist = go->scale.x + go2->scale.x;

					//Exercise 8b: store values in auditing variables

					//swapping vel
					if (distSq <= combinedDist * combinedDist)
					{

						m1 = go->mass;
						m2 = go2->mass;

						u1 = go->vel;
						u2 = go2->vel;
						Vector3 tempGOvel = go->vel;
						go->vel = go2->vel;
						go2->vel = tempGOvel;

						v1 = go->vel * go->mass;
						v2 = go2->vel * go2->mass;
						break;
					}
				}
			}

			//Exercise 10: handle collision using momentum swap instead

			//Exercise 12: improve inner loop to prevent double collision

			//Exercise 13: improve collision detection algorithm [solution to be given later] 
		}
	}
}


void SceneCollision::RenderGO(GameObject *go)
{
	switch(go->type)
	{
	case GameObject::GO_BALL:
		//Exercise 4: render a sphere using scale and pos
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();

		//Exercise 11: think of a way to give balls different colors
		break;
	}
}

void SceneCollision::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

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

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}

	//On screen text

	//Exercise 5: Render m_objectCount
	std::ostringstream oc;
	oc << "object count: " << m_objectCount;
	RenderTextOnScreen(meshList[GEO_TEXT], oc.str(), Color(0, 1, 0), 3, 0, 9);

	//Exercise 8c: Render initial and final momentum
	std::ostringstream go1;
	go1.precision(3);
	go1 << "u1: " << u1 << " v1" << v1;
	RenderTextOnScreen(meshList[GEO_TEXT], go1.str(), Color(0, 1, 0), 3, 0, 15);
																	  
	std::ostringstream go2;											  
	go2.precision(3);												  
	go2 << "u2: " << u2 << " v2" << v2;							  
	RenderTextOnScreen(meshList[GEO_TEXT], go2.str(), Color(0, 1, 0), 3, 0, 12);

	std::ostringstream ss2;
	ss2.precision(3);
	ss2 << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 6);
	
	std::ostringstream ss;
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
