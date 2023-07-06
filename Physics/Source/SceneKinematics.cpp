#include "SceneKinematics.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneKinematics::SceneKinematics()
{
}

SceneKinematics::~SceneKinematics()
{
}

void SceneKinematics::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;

	m_gravity.Set(0, -9.8f, 0); //init gravity as 9.8ms-2 downwards
	Math::InitRNG();

	m_ghost = new GameObject(GameObject::GO_BALL);
	//Exercise 1: construct 10 GameObject with type GO_BALL and add into m_goList
	for (int n = 0; n < 100; ++n)
	{
		GameObject* go = new GameObject(GameObject::GO_BALL);
		m_goList.push_back(go);
	}
}

void SceneKinematics::Update(double dt)
{
	SceneBase::Update(dt);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Keyboard Section
	if(Application::IsKeyPressed('1'))
		glEnable(GL_CULL_FACE);
	if(Application::IsKeyPressed('2'))
		glDisable(GL_CULL_FACE);
	if(Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	if(Application::IsKeyPressed(0x6B))
	{
		//Exercise 6: adjust simulation speed
		m_speed += 10.f * dt;
	}
	if(Application::IsKeyPressed(0x6D))
	{
		//Exercise 6: adjust simulation speed
		m_speed -= 10.f * dt;

		if (m_speed < 0)
			m_speed = 0;
	}
	if(Application::IsKeyPressed('C'))
	{
		//Exercise 9: clear screen
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); it++)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				go->active = false;
			}
		}
	}
	if (Application::IsKeyPressed('B'))
	{
		//Exercise 9: spawn balls
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (!go->active)
			{
				go->active = true;
				go->type = GameObject::GO_BALL;
				int w = Application::GetWindowWidth();
				int h = Application::GetWindowHeight();
				go->pos.Set(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0);
				go->vel.Set(rand() % 200 - 100, rand() % 200 - 100, 0); //random range between -100 - 100
			}
		}
	}
	if (Application::IsKeyPressed('V'))
	{
		//Exercise 9: spawn obstacles
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (!go->active)
			{
				go->active = true;
				go->type = GameObject::GO_CUBE;
				int w = Application::GetWindowWidth();
				int h = Application::GetWindowHeight();
				go->pos.Set(rand() % int(m_worldWidth), rand() % int(m_worldHeight), 0);
				break;
			}
		}
	}

	//Mouse Section
	static bool bLButtonState = false;
	//Exercise 10: ghost code here
	if(!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		

		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();

		//Exercise 10: spawn ghost ball
	
		m_ghost->active = true;
		m_ghost->pos.Set(x / w * m_worldWidth, (h - y) / h * m_worldHeight);
	}
	else if(bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
		
		//Exercise 4: spawn ball
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); it++)
		{
			GameObject* go = (GameObject*)*it;
			if (!go->active)
			{
				double x, y;
				Application::GetCursorPos(&x, &y);
				int w = Application::GetWindowWidth();
				int h = Application::GetWindowHeight();

				float posX = x / w * m_worldWidth;
				float posY = (h - y) / h * m_worldHeight;
				m_ghost->vel.x = m_ghost->pos.x - posX;
				m_ghost->vel.y = m_ghost->pos.y - posY;
				m_ghost->vel.z = 0;
				m_ghost->active = false;

				go->active = true;
				go->type = GameObject::GO_BALL;
				go->pos.Set(m_ghost->pos.x, m_ghost->pos.y);
				go->vel.Set(m_ghost->vel.x, m_ghost->vel.y, m_ghost->vel.z);
				break;
			}
		}
						
		//Exercise 10: replace Exercise 4 code and use ghost to determine ball velocity

		//Exercise 11: kinematics equation
		//v = u + a * t
		//t = (v - u ) / a


		//v * v = u * u + 2 * a * s
		//s = - (u * u) / (2 * a)
						
		//s = u * t + 0.5 * a * t * t
		//(0.5 * a) * t * t + (u) * t + (-s) = 0
	}
	
	static bool bRButtonState = false;
	if(!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;

		//Exercise 7: spawn obstacles using GO_CUBE
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); it++)
		{
			GameObject* go = (GameObject*)*it;
			if (!go->active)
			{
				double x, y;
				Application::GetCursorPos(&x, &y);
				int w = Application::GetWindowWidth();
				int h = Application::GetWindowHeight();

				go->active = true;
				go->type = GameObject::GO_CUBE;
				std::cout << x << y << std::endl;
				float posX = x / w * m_worldWidth;
				float posY = (h - y) / h * m_worldHeight;
				go->pos.Set(posX, posY);
				std::cout << go->pos.x << ", " << go->pos.y << std::endl;
				break;
			}
		}

	}
	else if(bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;
	}

	//Physics Simulation Section
	fps = (float)(1.f / dt);

	//Exercise 11: update kinematics information
	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			if(go->type == GameObject::GO_BALL)
			{
				//Exercise 2: implement equation 1 & 2
				go->vel += m_gravity * dt * m_speed;
				go->pos += go->vel * dt * m_speed;
				
				//Exercise 12: replace Exercise 2 code and use average speed instead

				//Exercise 8: check collision with GO_CUBE
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* temp = (GameObject*)*it2;
					if (temp->active && temp->type == GameObject::GO_CUBE)
					{
						Vector3 displacement = temp->pos - go->pos;
						float combinedRadii = go->scale.x + temp->scale.x;
						if (displacement.LengthSquared() <= combinedRadii * combinedRadii)
						{
							go->active = false;
							temp->active = false;
							break;
						}
					}
				}
			}

			//Exercise 5: unspawn ball when outside window
			if (go->pos.x > m_worldWidth + go->scale.x * 0.5f || go->pos.x < 0 - go->scale.x * 0.5f || go->pos.y > m_worldHeight + go->scale.y * 0.5f || go->pos.y < 0 - go->scale.y * 0.5f)
			{
				go->active = false;
				std::cout << "active is false" << std::endl;
			}
		}
	}
}

void SceneKinematics::RenderGO(GameObject *go)
{
	switch(go->type)
	{
	case GameObject::GO_BALL:
		//Exercise 3: render a sphere with radius 1
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();

		break;
	case GameObject::GO_CUBE:
		//Exercise 7: render a cube with length 2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_SHIP:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_ASTEROID:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(0.2f, 0.2f, 0.2f);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;

	}
}

void SceneKinematics::Render()
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

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}
	if(m_ghost->active)
	{
		RenderGO(m_ghost);
	}

	//On screen text
	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);
	
	//Exercise 6: print simulation speed
	std::ostringstream sp;
	sp.precision(3);
	sp << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], sp.str(), Color(0, 1, 0), 3, 0, 6);

	//Exercise 10: print m_ghost position and velocity information
	std::ostringstream pos;
	pos.precision(5);
	pos << "x:" << m_ghost->pos.x << " y:" << m_ghost->pos.y;
	RenderTextOnScreen(meshList[GEO_TEXT], pos.str(), Color(0, 1, 0), 3, 0, 9);

	std::ostringstream vel;
	vel.precision(5);
	vel << "velocity:" << m_ghost->vel.x << "," << m_ghost->vel.y << "," << m_ghost->vel.z;
	RenderTextOnScreen(meshList[GEO_TEXT], vel.str(), Color(0, 1, 0), 3, 0, 12);


	//Exercise 11: print kinematics information

	RenderTextOnScreen(meshList[GEO_TEXT], "Kinematics", Color(0, 1, 0), 3, 0, 0);
}

void SceneKinematics::Exit()
{
	// Cleanup VBO
	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if(meshList[i])
			delete meshList[i];
	}
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
	
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
