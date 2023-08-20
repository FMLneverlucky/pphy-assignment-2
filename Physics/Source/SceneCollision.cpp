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

	//starting state
	currentGameState = AIMING;

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	
	Math::InitRNG();

	//this is starting line where player release ball
	startingLine_pos = 10.f; //this is a y value

	//Exercise 1: initialize m_objectCount
	m_objectCount = 0;
	bLightEnabled = true;

	m_ghost = new GameObject(GameObject::GO_BALL);
	//set ghost ball to be active and on starting line
	m_ghost->pos.Set(m_worldWidth / 2, startingLine_pos, 1);
	m_ghost->scale.Set(2, 2, 2);
	m_ghost->mass = 8;
	m_ghost->color.Set(Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));
	m_ghost->active = true;

	// Week 13 Exercise 4
	float angle = Math::HALF_PI;
	float wallLength = 40;
	float radius = wallLength * 0.5f / tan(angle * 0.5f) + 20;
	
	//Create octogonal shape -  Week 13 Exercise 4
	/*
	for (int i = 0; i < 4; ++i)
	{
		GameObject* go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->scale.Set(2.0f, wallLength + 0.9f, 1.f);
		go->pos = Vector3(radius * cosf(i * angle) + m_worldWidth / 2,
							radius * sinf(i * angle) + m_worldHeight / 2 + startingLine_pos,	//so wall isnt ovelapping with starting line
							0.0f);
		go->normal = Vector3(cosf(i * angle), sinf(i * angle), 0.f);

	}
	*/

	// Week 13 Exercise 4
	//---------------------------- OBJECT PLACEMENT HERE---------------------------------------
	MakeThinWall(2.0f, 40.0f, Vector3(1, 0, 0), Vector3(m_worldWidth / 5, m_worldHeight / 2 + startingLine_pos, 0)); //this is red rectangle in the middle
	minX = m_worldWidth / 5 + 2.f; //ball bounce off wall on the inside, so take wall position + width
	MakeThinWall(2.0f, 40.0f, Vector3(-1, 0, 0), Vector3(m_worldWidth / 5 * 4, m_worldHeight / 2 + startingLine_pos, 0)); //this is red rectangle in the middle
	maxX = m_worldWidth / 5 * 4 - 2.f; //this is right wall and ball bounce off wall inside, so wall position - width

	DividePlayArea();
	/*
	//this is the 2 big balls beside each end of red rectangle
	//Pillar 1 
	GameObject* pillar = FetchGO();
	pillar->type = GameObject::GO_PILLAR;
	pillar->color.Set(1, 0, 1);
	pillar->scale.Set(10, 10, 1);
	pillar->pos = Vector3(m_worldWidth / 2 + m_worldWidth / 4, m_worldHeight / 2, 0);

	//Pillar 2
	pillar = FetchGO();
	pillar->type = GameObject::GO_PILLAR;
	pillar->color.Set(1, 0, 1);
	pillar->scale.Set(10, 10, 1);
	pillar->pos = Vector3(m_worldWidth / 2 - m_worldWidth / 4, m_worldHeight / 2, 0);

	*/
	//addRowOfBricks(5);
}

GameObject* SceneCollision::FetchGO()
{
	//Exercise 2a: implement FetchGO()
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
			continue;
		go->active = true;
		
		++m_objectCount;
		return go;
	}
	//
	//Exercise 2b: increase object count every time an object is set to active
	int prevSize = m_goList.size();
	for (int i = 0; i < 10; ++i)
	{
		m_goList.push_back(new GameObject(GameObject::GO_BALL));
	}
	m_goList.at(prevSize)->active = true;

	++m_objectCount;

	return m_goList[prevSize]; //
	//return NULL;
}

void SceneCollision::ReturnGO(GameObject *go)
{
	//Exercise 3: implement ReturnGO()
	if (go->active)
	{
		go->active = false;
		--m_objectCount;
	}
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

	//Mouse Section - added No. 6
	double x, y, windowWidth, windowHeight;
	Application::GetCursorPos(&x, &y);
	windowWidth = Application::GetWindowWidth();
	windowHeight = Application::GetWindowHeight();

	Vector3 mousePos(x * (m_worldWidth / windowWidth), (windowHeight - y) * (m_worldHeight / windowHeight), 0);

	//vector to store mouse position when left mouse button pressed (replacement for ghost ball used for velocity calculation)
	Vector3 mouseClickPos;


	static bool bLButtonState = false;

	if (reachWindowWidthBoundary(m_ghost, windowWidth) == false && !bLButtonState)	//adding check mouse state just in case
		m_ghost->pos.x = mousePos.x; //since dont want ball to move into play area while aiming, ghost ball can only move x pos to simulatemoving along y axis


	if (hitWallBoundary(m_ghost, minX, maxX) == false && !bLButtonState)	//ghost ball can pnly move if within wall boundary and mouse not clicked... in theory
		m_ghost->pos.x = mousePos.x;

	/*
	if (bLButtonState == false)
	{

		//ghost ball will move about along with mouse when left mouse button not clicked
		m_ghost->pos.x = mousePos.x;
	}
	*/

	//sleepy but yes states mmmmmm
	switch (currentGameState)
	{
	case AIMING:
		{

			//TODO: limit angle player can shoot ball so ball cannot bounce outside walls
			if(!bLButtonState && Application::IsMousePressed(0))
			{
				bLButtonState = true;
				std::cout << "LBUTTON DOWN" << std::endl;

				mouseClickPos = mousePos;
				std::cout << "ghost ball pos: " << m_ghost->pos.x << std::endl;

				std::cout << "object list count: " << m_goList.size() << std::endl;
			}
			else if(bLButtonState && !Application::IsMousePressed(0))
			{
				bLButtonState = false;
				std::cout << "LBUTTON UP" << std::endl;

				//Exercise 6: spawn small GO_BALL
				GameObject* go = FetchGO();

				go->type = GameObject::GO_BALL;
				//actual shooting ball starting position
				go->pos = m_ghost->pos;
				//
				go->vel = mouseClickPos - mousePos;
				std::cout << go->vel << std::endl;

				go->scale = m_ghost->scale;
				go->mass = m_ghost->mass;

				go->color = m_ghost->color;
				m_ghost->active = false;

				std::cout << go->pos << std::endl;
				currentGameState = GAME_STATE::SHOOTING;

				std::cout << "new object list count: " << m_goList.size() << std::endl;
			/*
			//currently not using big ball
			static bool bRButtonState = false;
			if(!bRButtonState && Application::IsMousePressed(1))
			{
				bRButtonState = true;
				std::cout << "RBUTTON DOWN" << std::endl;

				//No.9
				m_ghost->active = true;
				m_ghost->pos = mousePos;

				m_ghost->scale.Set(3, 3, 3);
				m_ghost->mass = 27;
				m_ghost->color.Set(Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));
				//
			}
			else if(bRButtonState && !Application::IsMousePressed(1))
			{
				bRButtonState = false;
				std::cout << "RBUTTON UP" << std::endl;

				//Exercise 10: spawn large GO_BALL
				GameObject* go = FetchGO();

				go->type = GameObject::GO_BALL;
				go->pos = m_ghost->pos;

				go->vel = m_ghost->pos - mousePos;
				//go->vel.y = 0;

				go->scale = m_ghost->scale;
				go->mass = m_ghost->mass;
				go->color = m_ghost->color;
				m_ghost->active = false;
			}
			*/

			/*
			//related to big ball code
			// No. 10
			if (bRButtonState)
			{
				float size = Math::Clamp((mousePos - m_ghost->pos).Length(), 2.0f, 10.0f);
				m_ghost->scale.Set(size, size, size);
				m_ghost->mass = size * size * size;
			}
			*/
			}

			break;
		}
	case SHOOTING:
		{
			for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)	//object list
			{
				GameObject* go = (GameObject*)*it;
				if (go->active && go->type == GameObject::GO_BALL) //since skipturn function is able to disable all gameobjects, check if object is a ball
				{
					if (destroyShootingBall(go) || skipTurn(go)) //this might bug because function will trigger whenever ball return to starting line
						currentGameState = WAITING;	//assuming everyball is first ball that return to starting line in shooting state, go to waiting state
				}
			}

			break;
		}
	case WAITING:
		{
			int num_ball = 0;
			for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject* go = (GameObject*)*it;
				if (go->type == GameObject::GO_BALL && go->active) //checking there are no balls in play area
					num_ball++;
			}
			if (num_ball == 0) //gamestate returns to aiming and respawns ghost ball at starting line
			{
				m_ghost->pos.Set(m_worldWidth / 2, startingLine_pos, 1);
				m_ghost->scale.Set(2, 2, 2);
				m_ghost->mass = 8;
				m_ghost->color.Set(Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));
				m_ghost->active = true;

				//updateBrickPos();	//if add bricks is becfore update, new row of bricks will also update its position -> not as intended
				//addRowOfBricks(5);
				//TODO: fix brick endless update problem
				currentGameState = AIMING;
			}
			break;
		}
	case GAME_OVER:
		{
			break;
		}
	default:
		break;
	}

	//Math::Clamp(m_ghost->pos.x, 0.f, m_worldWidth); //an attempt to clamp here x pos of ghost -> it didnt work so made functions to force position to min value
	//Math::Clamp(m_ghost->pos.x, minX, maxX); //an attempt to clamp here x pos of ghost

	//Physics Simulation Section

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			//Exercise 7a: implement movement for game objects
			if (go->type == GameObject::GO_BALL) //trying to prevent brick from getting position update
				go->pos += go->vel * dt * m_speed;
			
			//Exercise 7b: handle out of bound game objects
			/*if (go->pos.x < 0 - go->scale.x || go->pos.x > m_worldWidth + go->scale.x || go->pos.y < 0 - go->scale.y || go->pos.y > m_worldHeight + go->pos.y)
			{
				ReturnGO(go);
				continue;
			}*/
			
			// Week 13+ Improve internal collision between objects
			//if (go->pos.x < 0 || go->pos.x > m_worldWidth)
			if (((go->pos.x - go-> scale.x < 0) && go->vel.x < 0) 
				|| ((go->pos.x + go->scale.x > m_worldWidth) && go->vel.x > 0))
			{
				go->vel.x = -go->vel.x;
			}

			if (go->pos.x < 0 - go->scale.x || go->pos.x > m_worldWidth + go->scale.x)
			{
				ReturnGO(go);
				continue;
			}

			if (((go->pos.y - go->scale.y < 0) && go->vel.y < 0)
				|| ((go->pos.y + go->scale.y > m_worldHeight) && go->vel.y > 0))
			{
				go->vel.y = -go->vel.y;
			}

			if (go->pos.y < 0 - go->scale.y || go->pos.y > m_worldHeight + go->scale.y)
			{
				ReturnGO(go);
				continue;
			}

			GameObject* go2 = nullptr;
			for (std::vector<GameObject*>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
			{
				GameObject* go2 = static_cast<GameObject*> (*it2);
				if (go2->active)
				{
					if (CheckCollision(go, go2))
						CollisionResponse(go, go2);
				}

				GameObject* actor(go);
				GameObject* actee(go2);

				if (go->type != GameObject::GO_BALL)
				{
					actor = go2;
					actee = go;
				}

				if (go2->active && CheckCollision(actor, actee))
				{
					CollisionResponse(actor, actee);
				}
			}
			//Exercise 10: handle collision using momentum swap instead

			//Exercise 12: improve inner loop to prevent double collision

			//Exercise 13: improve collision detection algorithm 
		}

		
	}

}

// Week 13 Exercise 2 - Make Thin Wall 
void SceneCollision::MakeThinWall(float width, float height, const Vector3& normal, const Vector3& pos)
{
	GameObject* wall = FetchGO();	//gameobject created is ball within fetchgo function, but doesnt matter since object type is defined below
	wall->type = GameObject::GO_WALL;
	wall->scale.Set(width, height, 1.0f);
	wall->pos = pos;
	wall->normal = normal;
	wall->color.Set(1, 0, 0);
	wall->vel.SetZero();

	Vector3 tangent(-normal.y, normal.x);
	//Pillar 1
	GameObject* pillar = FetchGO();
	pillar->type = GameObject::GO_PILLAR;
	pillar->color.Set(0, 1, 1);
	pillar->scale.Set(width * 0.5f, width * 0.5f, 1);
	pillar->pos = pos + height * 0.5f * tangent;

	//Pillar 2
	pillar = FetchGO();
	pillar->type = GameObject::GO_PILLAR;
	pillar->color.Set(0, 1, 1);
	pillar->scale.Set(width * 0.5f, width * 0.5f, 1);
	pillar->pos = pos - height * 0.5f * tangent;
}

bool SceneCollision::destroyShootingBall(GameObject* go)//destroy all balls that return to starting line after player turn
{
	if (go->pos.y <= startingLine_pos)
	{
		ReturnGO(go);	//just found out this function sets active state false and subtracts object count so yea
		return true;	//return ball been destroyed
	}
	return false;	//no ball destroyed
}

bool SceneCollision::reachWindowWidthBoundary(GameObject* go, float windowWidth)
{
	if (go->pos.x < 0)
	{
		go->pos.x = 0;
		return true;
	}

	if (go->pos.x > windowWidth)
	{
		go->pos.x = windowWidth;
		return true;
	}
	return false;
}

bool SceneCollision::hitWallBoundary(GameObject* go, float min_x, float max_x)
{
	if (go->pos.x < min_x)
	{
		go->pos.x = min_x;
		return true;
	}

	if (go->pos.x > max_x)
	{
		go->pos.x = max_x;
		return true;
	}

	return false;
}

bool SceneCollision::skipTurn(GameObject* go)
{
	if (Application::IsKeyPressed('E'))
	{
		ReturnGO(go);
		return true;
	}
	return false;
}

/*
this function is to define play area (within walls from top of window to starting line)
& divide the area into columns and rows
since m_golist handles all object behaviour, this function is solely used to store ghost positions 
-> store position, spawn actual bricks in m_golist
| 1 ! 2 ! 3 ! 4 ! 5 !|
----------------------
|	!	!	!	!	!| 1
|---!---!---!---!---!|
|	!	!	!	!	!| 2
|---!---!---!---!---!|
|	!	!	!	!	!| 3
----------------------

*/
void SceneCollision::DividePlayArea()
{
	float playareaWidth = maxX - minX;

	//placeholder object used to calculate amount of bricks that can fit within play area
	//push this object into grid vector NOT M_GOLIST VECTOR
	GameObject* ghostbrick = new GameObject(GameObject::GO_P);	//since is placeholder, dont use fetchgo
	ghostbrick->type = GameObject::GO_P;
	ghostbrick->scale.Set(5, 5, 1);
	ghostbrick->health = 1;

	int numCol = playareaWidth / (2 * ghostbrick->scale.x);
	int numRow = ((m_worldHeight - ghostbrick->scale.y) - startingLine_pos) / (2 * ghostbrick->scale.y);	//playarea height is from top of screen to starting line

	std::cout << "num of column in playarea: " << numCol << " ,num of row in playarea: " << numRow << std::endl;

	for (int currentRow = 0; currentRow < numRow; currentRow++)	//post increment because y value calculation relies on current row value -> like row 1
	{
		std::cout << "row count: " << currentRow << std::endl;
		//all bricks in a row will have same y value -> set y value everytime loop through row
		//formula for setting y value -> 1st row = worldheight - scale -> n row = worldheight - 2(row num)*scale[number of bricks] - scale [initial scale that sets bricks within window]
		float y_pos = m_worldHeight - (2 * currentRow * ghostbrick->scale.y) - ghostbrick->scale.y;

		for (int currentCol = 0; currentCol < numCol; currentCol++)
		{
			//calculating x position of bricks in a row
			//1st col = min x + scale [starting from inner wall]
			//2nd col = min x + 2 * scale [skip 1 brick] + scale [initial scale to set brick within wall]
			//formula = min x + 2 * scale * currentcol + scale
			float x_pos = minX + (2 * ghostbrick->scale.x * currentCol) + ghostbrick->scale.x;

			ghostbrick->pos = Vector3(x_pos, y_pos, 0);
			//for testing purposes
			ghostbrick->color.Set(Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));
			ghostbrick->active = true;

			//pushback brick info into grid vector
			grid.push_back(ghostbrick);
			std::cout << ghostbrick->pos << std::endl;
		}
	}
	std::cout << "total num of brick position stored in grid: " << grid.size() << std::endl;
}

void SceneCollision::addRowOfBricks(int hp)
{
	int RandomNum = Math::RandIntMinMax(1, 5);

	GameObject* brick = FetchGO();
	brick->type = GameObject::GO_P;
	brick->color.Set(Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));
	brick->scale.Set(5, 5, 1);
	brick->health = hp;

	brick->pos = Vector3(minX + brick->scale.x, m_worldHeight - brick->scale.y, 0);

	std::cout << "pos of first brick in new row: " << brick->pos << std::endl;

	for (int numBricks = 1; numBricks != RandomNum; ++numBricks) //starts at 1 because loop is making 1 more brick than it should and my brain empty now, cant think of better way to fix
	{
			GameObject* brick2 = FetchGO();
			brick2->type = GameObject::GO_P;
			brick2->color.Set(Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1), Math::RandFloatMinMax(0, 1));
			brick2->scale.Set(5, 5, 1);
			brick2->health = hp;
			brick2->pos = Vector3(brick->pos.x + ((2 * numBricks) * brick2->scale.x), m_worldHeight - brick2->scale.y, 0); //x2 scale because center of object is well... at the cneter
			std::cout << "pos of following brick in row: " << brick2->pos << std::endl;

	}
	std::cout << "num of bricks added: " << RandomNum << std::endl;
	/*
	GameObject* brick2 = FetchGO();
	brick2->type = GameObject::GO_P;
	brick2->color.Set(1, 0.1f, 0);
	brick2->scale.Set(5, 5, 1);
center
	brick2->health = hp;
	*/
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		std::cout << go->type << std::endl;
	}
	std::cout << "total objects present: " << m_goList.size();

}

void SceneCollision::updateBrickPos()
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active && (go->type == GameObject::GO_P))	//looking for bricks within object vector
		{
			if (currentGameState == WAITING)
			{
				go->pos.y = go->pos.y + 2 * go->scale.y;
				std::cout << "new pos: " << go->pos << std::endl;
				//checking how long brick updates position -> brick still updates position even after game state change
				//TODO: handle brick stop updating when game state change here
			}
		}
	}
}

//Exercise 8a: handle collision between GO_BALL and GO_BALL using velocity swap
//Exercise 8b: store values in auditing variables
bool SceneCollision::CheckCollision(GameObject* go1, GameObject* go2)
{
	// Week 13
	//Prevent non ball vs non ball code
	if (go1->type != GameObject::GO_BALL)
	{
		return false;
	}

	switch (go2->type)
	{
		case GameObject::GO_PILLAR:
		case GameObject::GO_P:
		case GameObject::GO_BALL:
		{
			Vector3 relativeVel = go1->vel - go2->vel;
			Vector3 disDiff = go2->pos - go1->pos;

			if (relativeVel.Dot(disDiff) <= 0)
				return false;

			return disDiff.LengthSquared() <= (go1->scale.x + go2->scale.x) * (go1->scale.x + go2->scale.x);
		}
		case GameObject::GO_WALL:
		{
			//fix this part
			//increase length of detection so it detects full length of wall instead of just the center
			Vector3 diff = go1->pos - go2->pos;
			Vector3 axisX = go2->normal;
			Vector3 axisY = Vector3(-go2->normal.y, go2->normal.x, 0);

			float projectedDist = diff.Dot(axisX);

			if (projectedDist > 0)
				axisX = -axisX;

			return go1->vel.Dot(axisX) >= 0 && //Check 1: Travelling towards the wall ?
				go2->scale.x * 0.5 + go1->scale.x > -diff.Dot(axisX) && //Check 2: Radius + Thickness vs Distance
				go2->scale.y > Math::FAbs(diff.Dot(axisY)); //Check 3: Length check
		}
	}
/* // Week 12
	//Vector3 disDiffVec = go2->pos - go1->pos;
	//return disDiffVec.LengthSquared() <= (go1->scale.x + go2->scale.x) * (go1->scale.x + go2->scale.x);

	Vector3 relativeVel = go1->vel - go2->vel;
	Vector3 disDiff = go2->pos - go1->pos;

	if (relativeVel.Dot(disDiff) <= 0)
		return false;

	return disDiff.LengthSquared() <= (go1->scale.x + go2->scale.x) * (go1->scale.x + go2->scale.x);
	*/
}

void SceneCollision::CollisionResponse(GameObject* go1, GameObject* go2)
{
	//initial values
	u1 = go1->vel;
	u2 = go2->vel;
	m1 = go1->mass;
	m2 = go2->mass;

	//1D Elastic Collision
	//go1->vel = (m1 - m2) / (m2 + m1) * u1 + (2 * m2) / (m2 + m1) * u2;
	//go2->vel = (2 * m1) / (m2 + m1) * u1 + (m2 - m1) / (m2 + m1) * u2;
	
	//Swap velocity
	//Vector3 temp = go1->vel;
	//go1->vel = go2->vel;
	//go2->vel = temp;

	//2D Elastic Collision
	//Step 1: Find n, un and ut // Week 13 commented
/*	Vector3 n = go1->pos - go2->pos;
	Vector3 un = n.Normalize();
	Vector3 ut = Vector3(-un.y, un.x, 0);

	//Step 3 find v1n, v2n, v1t, v2t
	float v1n = un.Dot(u1);
	float v1t = ut.Dot(u1);
	float v2n = un.Dot(u2);
	float v2t = ut.Dot(u2);

	//Step 5 find new normal velocity
	float v1nP = (v1n * (m1 - m2) + (2 * m2 * v2n)) / (m1 + m2);
	float v2nP = (v2n * (m2 - m1) + (2 * m1 * v1n)) / (m1 + m2);

	//Step 6
	Vector3 v1nVec = v1nP * un;
	Vector3 v1tVec = v1t * ut;
	Vector3 v2nVec = v2nP * un;
	Vector3 v2tVec = v2t * ut;

	//Step 7
	go1->vel = v1nVec + v1tVec;
	go2->vel = v2nVec + v2tVec; */

	switch (go2->type)
	{
		case GameObject::GO_BALL:
		{
			//2D Version 2
			Vector3 n = go1->pos - go2->pos;
			Vector3 vec = (u1 - u2).Dot(n) / (n).LengthSquared() * n;
			go1->vel = u1 - (2 * m2 / (m1 + m2)) * vec;
			go2->vel = u2 - (2 * m1 / (m2 + m1)) * -vec;
			break;
		}
		case GameObject::GO_WALL:
		{
			go1->vel = u1 - (2.0 * u1.Dot(go2->normal)) * go2->normal;
			break;
		}
		case GameObject::GO_PILLAR:
		{
			Vector3 n = (go2->pos - go1->pos).Normalize();
			go1->vel = u1 - (2.0 * u1.Dot(n)) * n;
			break;
		}
		case GameObject::GO_P:
		{
   			if (go2->health > 1)
			{
				//this is pillar collision response
				Vector3 n = (go2->pos - go1->pos).Normalize();
				go1->vel = u1 - (2.0 * u1.Dot(n)) * n;

				//brick reduce 1 health when ball bounce off brick
				go2->health -= 1;
			}
			else
			{
				go1->vel = u1 - (2.0 * u1.Dot(go2->normal)) * go2->normal;
				go2->active = false;
			}
			break;
		}
	}
}

void SceneCollision::RenderGO(GameObject *go)
{
	switch (go->type)
	{

	case GameObject::GO_PILLAR:
		break;
	case GameObject::GO_P: // new // A block to be destroyed.
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(go->color.x, go->color.y, go->color.z);
		RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(go->health), Color(1, 1, 1),
			8, (go->pos.x / m_worldWidth) * go->scale.x, go->pos.y / m_worldHeight);
		RenderMesh(meshList[GEO_CUBE], true);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_BALL:
		//Exercise 4: render a sphere using scale and pos
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.y);
		meshList[GEO_BALL]->material.kAmbient.Set(go->color.x, go->color.y, go->color.z);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();

		//Exercise 11: think of a way to give balls different colors
		break;

	// Week 13 Exercise 3
	case GameObject::GO_WALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2f(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		meshList[GEO_CUBE]->material.kAmbient.Set(go->color.x, go->color.y, go->color.z);
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
	
	// MGhost 
	if (m_ghost->active)
		RenderGO(m_ghost);

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}

	for (std::vector<GameObject*>::iterator it = grid.begin(); it != grid.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}

	//On screen text
	std::ostringstream ss;

	//Exercise 5: Render m_objectCount
	RenderTextOnScreen(meshList[GEO_TEXT], "Object Count:" +std::to_string(m_objectCount), Color(0, 1, 0
), 3, 0, 9);

	//Exercise 8c: Render initial and final momentum
	
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
