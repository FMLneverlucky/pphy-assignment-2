#include "SceneAsteroid.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneAsteroid::SceneAsteroid()
{
}

SceneAsteroid::~SceneAsteroid()
{
}

void SceneAsteroid::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	currentLevel = 0;
	firstLevel = secLevel = thirdLevel = false;
	loadlevel = false;
	levelLoaded = false;

	EnemyCount = 0;
	timer = 0.f;

	//Physics code here
	m_speed = 1.f;
	m_rotation = 0.f;

	Math::InitRNG();

	//Exercise 2a: Construct 100 GameObject with type GO_ASTEROID and add into m_goList
	for (int n = 0; n < 200; ++n)
	{
		GameObject* go = new GameObject(GameObject::GO_ASTEROID);
		m_goList.push_back(go);
	}

	//Exercise 2b: Initialize m_lives and m_score
	m_lives = 3;
	m_score = 0;

	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_SHIP);
	m_ship->active = true;
	m_ship->mass = 1;
	m_ship->pos.x = m_worldWidth * 0.5f;
	m_ship->pos.y = m_worldHeight * 0.5f;
	m_ship->pos.z = 0;
	m_ship->scale.Set(4, 4, 4);
	m_ship->momentOfIntertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;
	m_ship->angularVelocity = 0;
}


GameObject* SceneAsteroid::FetchGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (auto go : m_goList)
	{
		if (!go->active)
		{
			return go;
		}
	}
	//for (int i = 0; i < 10; i++) //???
	//{
	//	GameObject* newGO = new GameObject(GameObject::GO_ASTEROID);
	//	m_goList.push_back(newGO);
	//}
	return m_goList.back();
}

void SceneAsteroid::Update(double dt)
{
	SceneBase::Update(dt);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Mouse Section
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;

		double x, y;
		Application::GetCursorPos(&x, &y);
		//int w = Application::GetWindowWidth();
		//int h = Application::GetWindowHeight();

		if ((x >= 250 && x <= 550) && (y >= 210 && y <= 265))
			loadlevel = true;
		std::cout << x << ' ' << y << std::endl;

	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
	static bool bRButtonState = false;
	if (!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
	}
	else if (bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;
	}


	if (loadlevel == true && currentLevel <= 0)
	{
		currentLevel++;
		StartGame();
	}
	else if (loadlevel == true && currentLevel > 0)
		StartGame();
	
	if (currentLevel == 2)
		timer -= dt * 60;

	if (currentLevel >= 4)
		loadlevel = false;

	if(Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if(Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}
	m_force.SetZero();
	m_torque.SetZero();

	//Exercise 6: set m_force values based on WASD
	if(Application::IsKeyPressed('W'))
	{
		m_force += 100 * m_ship->dir;
		m_torque = Vector3(0, -1, 0).Cross(Vector3(0, 100, 0));
	}
	if(Application::IsKeyPressed('A'))
	{
		m_force += 5 * m_ship->dir;
		m_torque = Vector3(1, -1, 0).Cross(Vector3(0, 5, 0));

	}
	if(Application::IsKeyPressed('S'))
	{
		m_force += 100 * -m_ship->dir;
		m_torque = Vector3(0, 1, 0).Cross(Vector3(0, -100, 0));

	}
	if(Application::IsKeyPressed('D'))
	{
		m_force += 5 * m_ship->dir;
		m_torque = Vector3(-1, -1, 0).Cross(Vector3(0, 5, 0));

	}
	m_ship->momentOfIntertia = m_ship->mass * (m_ship->scale.x) * (m_ship->scale.x);

	//Exercise 8: use 2 keys to increase and decrease mass of ship
	if (Application::IsKeyPressed(VK_OEM_PLUS))
	{
		m_ship->mass = Math::Min(10.f, m_ship->mass + 1.f * (float)dt); //if ship mass is above 10, it will return 10.f
		m_ship->momentOfIntertia += 1.f * dt;

	}
	if (Application::IsKeyPressed(VK_OEM_MINUS))
	{
		m_ship->mass = Math::Max(0.1f, m_ship->mass - 1.f * (float)dt); //if ship mass drops below 0.1, it will return 0.1
		m_ship->momentOfIntertia -= 1.f * dt;
		//to floor the value use max
		//to cap the value use min
	}
	if (Application::IsKeyPressed('R'))
		Reset();

	//Exercise 11: use a key to spawn some asteroids
	static bool bVButtonState = false; //prevent V to get spam

	if (!bVButtonState && Application::IsKeyPressed('V'))
	{
		bVButtonState = true; //press V once
		for (int i = 0; i <= 10; i++) //creating 10 asteroid
		{
			GameObject* GO = FetchGO(); //calling FetchGo function that returns a gameobject
			GO->type = GameObject::GO_ASTEROID; //Initializing the type to be GO_ASTEROID
			//randomise health of asteroid
			GO->health = Math::RandIntMinMax(1, 3);
			GO->scale.Set(GO->health, GO->health, GO->health); //scale asteroid size according to amount of health
			GO->pos.Set(Math::RandFloatMinMax(GO->scale.x, m_worldWidth - GO->scale.x), Math::RandFloatMinMax(GO->scale.y, m_worldHeight - GO->scale.x)); //setting position
			GO->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting
			GO->active = true;
		}
	}
	else if (bVButtonState && !Application::IsKeyPressed('V'))
	{
		bVButtonState = false;
	}

	//spawn enemy
	if (!bVButtonState && Application::IsKeyPressed('C'))
	{
		for (int i = 0; i <= 3; i++) //creating 5 enemy
		{
			bVButtonState = true;
			GameObject* GO = FetchGO(); //calling FetchGo function that returns a gameobject
			GO->type = GameObject::GO_ENEMY; 
			GO->dir.Set(0, -1, 0); //enemy faces downwards so yes direction vector downwards
			GO->active = true;
			GO->scale.Set(1, 1, 1); //scaling
			GO->pos.Set(Math::RandFloatMinMax(GO->scale.x, m_worldWidth - GO->scale.x), Math::RandFloatMinMax(GO->scale.y, m_worldHeight - GO->scale.x)); //setting position
			GO->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting
		}
	}
	else if (bVButtonState && !Application::IsKeyPressed('C'))
	{
		bVButtonState = false;
	}

	static double timeLastBulletSpawn = 0;
	timeLastBulletSpawn += dt * m_speed;
	//Exercise 14: use a key to spawn a bullet
	if (Application::IsKeyPressed(VK_SPACE) && timeLastBulletSpawn >= 0.1f)
	{
		timeLastBulletSpawn = 0;
		GameObject* go = FetchGO();
		go->active = true;
		go->scale.Set(0.2f, 0.2f, 0.2f);
		go->type = GameObject::GO_BULLET;
		go->pos = m_ship->pos;
		try
		{
			go->pos = m_ship->pos + m_ship->dir.Normalized() * m_ship->scale.Length();
			go->vel = m_ship->dir.Normalized() * BULLET_SPEED;
		}
		catch (DivideByZero)
		{
			go->pos = m_ship->pos + Vector3(1, 0) * m_ship->scale.Length();
			go->vel = Vector3(1, 0, 0) * BULLET_SPEED;
		}
	}
	//Exercise 15: limit the spawn rate of bullets

	//Physics Simulation Section

	//Exercise 7: Update ship's velocity based on m_force
	Vector3 accel = m_force * (1.f / m_ship->mass);

	// required for dynamic games
	//dont use operator / because of divide by 0 error
	m_ship->vel += accel * dt * m_speed;
	m_ship->pos += m_ship->vel * dt * m_speed;


	if (m_ship->vel.Length() > MAX_SPEED)
		m_ship->vel = m_ship->vel.Normalized() * MAX_SPEED;

	//prac3 ex4
	float alpha = m_torque.z / m_ship->momentOfIntertia;

	m_ship->angularVelocity += alpha * dt * m_speed;
	m_ship->angularVelocity = Math::Clamp(m_ship->angularVelocity, -(float)MAX_ANGULAR_SPEED, (float)MAX_ANGULAR_SPEED);

	float dTheta = m_ship->angularVelocity * dt * m_speed;

	m_ship->dir.Set(m_ship->dir.x * cosf(dTheta) - m_ship->dir.y * sinf(dTheta),
		m_ship->dir.x * sinf(dTheta) + m_ship->dir.y * cosf(dTheta));

	//Exercise 9: wrap ship position if it leaves screen
	if (m_ship->pos.x > m_worldWidth + m_ship->scale.x * 0.5f)
	{
		m_ship->pos.x = 0 - m_ship->scale.x * 0.5f;
	}
	else if (m_ship->pos.x < 0 - m_ship->scale.x * 0.5f)
	{
		m_ship->pos.x = m_worldWidth + m_ship->scale.x * 0.5f;
	}
	if (m_ship->pos.y > m_worldHeight + m_ship->scale.y * 0.5f)
	{
		m_ship->pos.y = 0 - m_ship->scale.y * 0.5f;
	}
	else if (m_ship->pos.y < 0 - m_ship->scale.y * 0.5f)
	{
		m_ship->pos.y = m_worldHeight + m_ship->scale.y * 0.5f;
	}

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		//Exercise 12: handle collision between GO_SHIP and GO_ASTEROID using simple distance-based check
		if(go->type == GameObject::GO_ASTEROID)
		{
			go->pos += go->vel * dt * m_speed; //make asteroid move 

			//Exercise 13: asteroids should wrap around the screen like the ship
			go->pos.x = Math::Wrap(go->pos.x, 0 - go->scale.x, m_worldWidth + go->scale.x);
			go->pos.y = Math::Wrap(go->pos.y, 0 - go->scale.y, m_worldHeight + go->scale.y);

			if ((go->pos - m_ship->pos).Length() <= go->scale.x + m_ship->scale.x) //asteroid collision, will respawn even if asteroid health > 1
			{
				m_ship->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0);
				m_ship->vel.SetZero();
				m_lives -= 1;
				//to prevent accidental collision, set asteroid false
				go->active = false;
				//randomise asteroid position
				do
				{
				go->pos.Set(Math::RandFloatMinMax(go->scale.x, m_worldWidth - go->scale.x), Math::RandFloatMinMax(go->scale.y, m_worldHeight - go->scale.x)); //setting position
				} while ((go->pos - m_ship->pos).Length() >= 100.f); //100 is dist away from ship asteroid should spawn
				go->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting velocity
				go->health = Math::RandIntMinMax(1, 3); //randomise health
				go->active = true; //spawn asteroid
				m_ship->angularVelocity = 0;
				m_ship->dir.Set(0, 1, 0);

				break;
			}

			//Exercise 16: unspawn bullets when they leave screen

			//Exercise 18: collision check between GO_BULLET and GO_ASTEROID
			for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
			{
				GameObject* go2 = (GameObject*)*it2;

				if (go2->active && go2->type == GameObject::GO_BULLET)
				{

					float distSq = (go2->pos - go->pos).LengthSquared();
					float combinedDist = go->scale.x + go2->scale.x;

					if (distSq <=  combinedDist)
					{
						if (go->health <= 1)
						{
							go->health -= 1;							
						}
						else
						{
							go->active = false;
							do
							{
								go->pos.Set(Math::RandFloatMinMax(go->scale.x, m_worldWidth - go->scale.x), Math::RandFloatMinMax(go->scale.y, m_worldHeight - go->scale.x)); //setting position
							} while ((go->pos - m_ship->pos).Length() >= 100.f); //100 is dist away from ship asteroid should spawn
							go->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting velocity
							go->health = Math::RandIntMinMax(1, 3); //randomise health
							go->active = true; //spawn asteroid
						}
						m_score += 2;
						go->pos.Set(Math::RandFloatMinMax(go->scale.x, m_worldWidth - go->scale.x), Math::RandFloatMinMax(go->scale.y, m_worldHeight - go->scale.x)); //setting position
						go->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting
						break;
					}
				}
				else if (go2->active && go2->type == GameObject::GO_ENEMY)
				{

					float distSq = (go2->pos - go->pos).LengthSquared();
					float combinedDist = go->scale.x + go2->scale.x;

					if (distSq <= (combinedDist * combinedDist))
					{
						//asteroid respawn thing
						go->active = false;
						go->pos.Set(Math::RandFloatMinMax(go->scale.x, m_worldWidth - go->scale.x), Math::RandFloatMinMax(go->scale.y, m_worldHeight - go->scale.x)); //setting position
						go->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting velocity
						go->health = Math::RandIntMinMax(1, 3); //randomise health
						go->active = true; //spawn asteroid

						//enemy health thing
						go2->health--;

						break;
					}
				}

			}
		}
		else if (go->type == GameObject::GO_BULLET)
		{
			go->pos += go->vel * dt * m_speed; // make bullet move

			if (go->pos.x <= 0 || go->pos.x >= m_worldWidth || go->pos.y <= 0 || go->pos.y >= m_worldHeight)
			{
				go->active = false;
			}

			//checking for bullet hitting enemy
			for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
			{
				GameObject* go2 = (GameObject*)*it2;
				if (go2->active && go2->type == GameObject::GO_ENEMY)
				{
					float distSq = (go->pos - go2->pos).LengthSquared();
					float combinedDist = go->scale.x + go2->scale.x;
					if (distSq <=  (combinedDist * combinedDist))
					{
						if (go2->health - 1 >= 1)
						{
							go2->health -= 1;
						}
						else
						{
							go2->active = false;
							EnemyCount--;
						}
						m_score += 5;
						break;
					}
				}
			}

		}
		else if (go->type == GameObject::GO_ENEMY && go->active)
		{
			if (go->health <= 0)
			{
				go->active = false;
				EnemyCount--;
			}

			// enemy should wrap around the screen like the ship
			go->pos.x = Math::Wrap(go->pos.x, 0 - go->scale.x, m_worldWidth + go->scale.x);
			go->pos.y = Math::Wrap(go->pos.y, 0 - go->scale.y, m_worldHeight + go->scale.y);
			
			go->dir = (m_ship->pos - go->pos).Normalized();
			go->pos += go->vel * dt * m_speed; //make enemy move 

			for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
			{
				GameObject* go2 = (GameObject*)*it2;

				static double timeLastEnemyBulletSpawn = 0;
				timeLastEnemyBulletSpawn += dt * 60;

				if (timeLastEnemyBulletSpawn >= 120.f) 
				{
					timeLastEnemyBulletSpawn = 0;
					GameObject* go2 = FetchGO();
					go2->active = true;
					go2->scale.Set(1, 1, 1);
					go2->type = GameObject::GO_ENEMY_BULLET;
					go2->pos = go->pos;
					try
					{
						go2->vel = go->dir.Normalized() * MISSILE_SPEED;
					}
					catch (DivideByZero)
					{
						go2->vel = Vector3(1, 0, 0) * MISSILE_SPEED;
					}
				}
				break;
			}
		}
		else if (go->type == GameObject::GO_ENEMY_BULLET)
		{
			go->pos += go->vel * dt * m_speed; // make bullet move

			//if enemy bullet moves out of bounds, despawn
			if (go->pos.x <= 0 || go->pos.x >= m_worldWidth || go->pos.y <= 0 || go->pos.y >= m_worldHeight)
			{
				go->active = false;
			}

			float EdistSq = (go->pos - m_ship->pos).LengthSquared();
			float EcombinedDist = go->scale.x + m_ship->scale.x;
			if (EdistSq <= (EcombinedDist * EcombinedDist))
			{
				go->active = false;
				m_lives -= 1;
				break;
			}
		}
	}

	if (m_lives <= 0)
	{
		m_ship->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0);
		m_ship->vel.SetZero();
		for (int i = 0; i < m_goList.size(); ++i)
		{
			m_goList[i]->active = false;
			m_goList[i]->pos.SetZero();
			m_goList[i]->vel.SetZero();
		}
		m_lives = 3;
		m_score = 0;
		Reset();
	}

}

void SceneAsteroid::Reset()
{
	m_ship->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0);
	m_ship->vel.SetZero();
	m_ship->angularVelocity = 0;
	m_ship->dir.Set(0, 1, 0);
	for (int i = 0; i < m_goList.size(); ++i)
	{
		m_goList[i]->active = false;
		m_goList[i]->pos.SetZero();
		m_goList[i]->vel.SetZero();
	}
	m_lives = 3;
	m_score = 0;
	EnemyCount = 0;
	levelLoaded = false;
	loadlevel = false;
}

void SceneAsteroid::StartGame()
{
	switch(currentLevel)
	{
	case 1:
		if (levelLoaded == false)
		{
			for (int i = 0; i <= 25; i++) //creating 10 asteroid
			{
				GameObject* GO = FetchGO(); //calling FetchGo function that returns a gameobject
				GO->type = GameObject::GO_ASTEROID; //Initializing the type to be GO_ASTEROID
				//randomise health of asteroid
				GO->health = Math::RandIntMinMax(1, 3);
				GO->scale.Set(GO->health, GO->health, GO->health); //scale asteroid size according to amount of health
				GO->pos.Set(Math::RandFloatMinMax(GO->scale.x, m_worldWidth - GO->scale.x), Math::RandFloatMinMax(GO->scale.y, m_worldHeight - GO->scale.x)); //setting position
				GO->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting
				GO->active = true;
			}
			levelLoaded = true;
		}
		if (m_score >= 20)
		{
			currentLevel += 1;
			Reset();
		}
		break;
	case 2: //survive for 30 sec? idk
		if (levelLoaded == false)
		{
			for (int i = 0; i <= 15; i++) //creating 10 asteroid
			{
				GameObject* GO = FetchGO(); //calling FetchGo function that returns a gameobject
				GO->type = GameObject::GO_ASTEROID; //Initializing the type to be GO_ASTEROID
				//randomise health of asteroid
				GO->health = Math::RandIntMinMax(1, 3);
				GO->scale.Set(GO->health, GO->health, GO->health); //scale asteroid size according to amount of health
				GO->pos.Set(Math::RandFloatMinMax(GO->scale.x, m_worldWidth - GO->scale.x), Math::RandFloatMinMax(GO->scale.y, m_worldHeight - GO->scale.x)); //setting position
				GO->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting
				GO->active = true;
			}

			for (int i = 0; i <= 1; i++) //creating 5 enemy
			{
				GameObject* GO = FetchGO(); //calling FetchGo function that returns a gameobject
				GO->type = GameObject::GO_ENEMY;
				GO->dir.Set(0, -1, 0); //enemy faces downwards so yes direction vector downwards
				GO->active = true;
				GO->scale.Set(1, 1, 1); //scaling
				GO->pos.Set(Math::RandFloatMinMax(GO->scale.x, m_worldWidth - GO->scale.x), Math::RandFloatMinMax(GO->scale.y, m_worldHeight - GO->scale.x)); //setting position
				GO->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting
			}

			timer = 1176.f; //20 sec cos haha
			levelLoaded = true;
		}

		if (timer <= 0)
		{
			timer = 0.f;
			Reset();
			currentLevel += 1;
		}

		break;
	case 3:
		if (levelLoaded == false)
		{

			for (int i = 0; i <= 5; i++) //creating 10 asteroid
			{
				GameObject* GO = FetchGO(); //calling FetchGo function that returns a gameobject
				GO->type = GameObject::GO_ASTEROID; //Initializing the type to be GO_ASTEROID
				//randomise health of asteroid
				GO->health = Math::RandIntMinMax(1, 3);
				GO->scale.Set(GO->health, GO->health, GO->health); //scale asteroid size according to amount of health
				GO->pos.Set(Math::RandFloatMinMax(GO->scale.x, m_worldWidth - GO->scale.x), Math::RandFloatMinMax(GO->scale.y, m_worldHeight - GO->scale.x)); //setting position
				GO->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting
				GO->active = true;
			}

			for (int i = 0; i <= 1; i++) //creating 5 enemy
			{
				GameObject* GO = FetchGO(); //calling FetchGo function that returns a gameobject
				GO->type = GameObject::GO_ENEMY;
				GO->dir.Set(0, -1, 0); //enemy faces downwards so yes direction vector downwards
				GO->active = true;
				GO->scale.Set(1, 1, 1); //scaling
				GO->pos.Set(Math::RandFloatMinMax(GO->scale.x, m_worldWidth - GO->scale.x), Math::RandFloatMinMax(GO->scale.y, m_worldHeight - GO->scale.x)); //setting position
				GO->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5)); //setting
				EnemyCount++;
			}

			levelLoaded = true;
		}
		if (EnemyCount <= 0)
		{
			currentLevel += 1;
		}

	default:
		break;
	}
}

void SceneAsteroid::RenderGO(GameObject *go)
{
	switch(go->type)
	{
	case GameObject::GO_SHIP:
		//Exercise 4a: render a sphere with radius 1
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, 1); //z = 1 so ship is on top of rest of objs
		modelStack.Rotate(Math::RadianToDegree(atan2(go->dir.y, go->dir.x)) - 90, 0, 0, 1);		
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SHIP], false);
		modelStack.PopMatrix();
		//Exercise 17a: render a ship texture or 3D ship model
		//Exercise 17b:	re-orientate the ship with velocity
		break;

	case GameObject::GO_ASTEROID:
		//Exercise 4b: render a cube with length 2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, zvalue);
		modelStack.Scale(go->health, go->health, go->health);
		RenderMesh(meshList[GEO_ASTEROID], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, zvalue);
		modelStack.Scale(0.4f, 0.4f, 0.4f);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_ENEMY:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, zvalue);
		modelStack.Scale(2, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ENEMY], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_ENEMY_BULLET:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, zvalue);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
		modelStack.Scale(0.8f, 0.3f, 0.4f);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;

	}
}

void SceneAsteroid::Render()
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

	//render background
	modelStack.PushMatrix();
	modelStack.Translate(m_worldWidth / 2, m_worldHeight / 2, -1);
	modelStack.Scale(Application::GetWindowWidth() / m_worldWidth , Application::GetWindowHeight() / m_worldHeight, 0);
	RenderMesh(meshList[GEO_BG], false);
	modelStack.PopMatrix();

	if (levelLoaded == true && loadlevel == true)
	{
		zvalue = 0;
		//render gameobj
		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->active)
			{
				RenderGO(go);
				zvalue += 0.0001f;
			}
		}
		RenderGO(m_ship);

		//On screen text
		//Exercise 5a: Render m_lives, m_score

		//Exercise 5b: Render position, velocity & mass of ship
		std::ostringstream pos;
		pos.precision(5);
		pos << "x:" << m_ship->pos.x << " y:" << m_ship->pos.y;
		RenderTextOnScreen(meshList[GEO_TEXT], pos.str(), Color(0, 1, 0), 3, 0, 12);

		std::ostringstream vel;
		vel.precision(5);
		vel << "velocity:" << m_ship->vel;
		RenderTextOnScreen(meshList[GEO_TEXT], vel.str(), Color(0, 1, 0), 3, 0, 15);

		std::ostringstream m;
		m << "mass:" << m_ship->mass;
		RenderTextOnScreen(meshList[GEO_TEXT], m.str(), Color(0, 1, 0), 3, 0, 18);

		std::ostringstream ss;
		ss.precision(3);
		ss << "Speed: " << m_speed;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 6);

		ss.str("");
		ss.precision(5);
		ss << "FPS: " << fps;
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);

		std::ostringstream CL;
		CL.str("");
		CL << "current level: " << currentLevel;
		RenderTextOnScreen(meshList[GEO_TEXT], CL.str(), Color(0, 1, 0), 3, 18, 51);

		std::ostringstream ob;
		ob.str("");

		std::ostringstream ls;

		if (currentLevel == 1)
		{
			ob << "Get a score of 20";
			ls << "lives:" << m_lives << " score:" << m_score;
		}
		if (currentLevel == 2)
		{
			ob << "Survive for 20s";
			ls << "lives:" << m_lives << " time left:" << timer * 0.017;
		}
		if (currentLevel == 3)
		{
			ob << "kill all enemies";
			ls << "lives:" << m_lives << " enemies left:" << EnemyCount;
		}

		RenderTextOnScreen(meshList[GEO_TEXT], ob.str(), Color(0, 1, 0), 3, 15, 54);
		RenderTextOnScreen(meshList[GEO_TEXT], ls.str(), Color(0, 1, 0), 3, 0, 9);

		RenderTextOnScreen(meshList[GEO_TEXT], "Asteroid", Color(0, 1, 0), 3, 0, 0);
	}
	else if (currentLevel > 0  && currentLevel <= 3 && levelLoaded == false && loadlevel == false)
	{
		modelStack.PushMatrix();
		modelStack.Translate(m_worldWidth / 2, m_worldHeight / 2 + 10, 0);
		modelStack.Scale(25, 5, 5);
		RenderMesh(meshList[GEO_TEXTBOX], false);
		modelStack.PopMatrix();

		RenderTextOnScreen(meshList[GEO_TEXT], "Restart Level", Color(0, 1, 0), 3, 26, 35);

	}
	else
	{
		modelStack.PushMatrix();
		modelStack.Translate(m_worldWidth / 2, m_worldHeight / 2 + 10, 0);
		modelStack.Scale(25, 5, 5);
		RenderMesh(meshList[GEO_TEXTBOX], false);
		modelStack.PopMatrix();

		RenderTextOnScreen(meshList[GEO_TEXT], "Start Game", Color(0, 1, 0), 3, 26, 35);
	}

}

void SceneAsteroid::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ship)
	{
		delete m_ship;
		m_ship = NULL;
	}
}
