 #include "GeometryWars.h"

#include <vector>
#include <iostream>
#include <time.h>

#include <Core/Engine.h>
#include "Transform.h"
#include "Meshes.h"

using namespace std;

GeometryWars::GeometryWars()
{
}

GeometryWars::~GeometryWars()
{
	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i] != NULL) delete enemies[i];
	}
	for (int i = 0; i < shots.size(); i++) {
		if (shots[i] != NULL) delete shots[i];
	}
	delete player;
}

void GeometryWars::Init()
{
	auto camera = GetSceneCamera();
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	logicSpace.x = 0;		// logic x
	logicSpace.y = 0;		// logic y
	logicSpace.width = (float)9.6;	// logic width
	logicSpace.height = (float)5.4;	// logic height

	player = new Object(glm::vec3(logicSpace.width / 2, logicSpace.height / 2, 0), 3);
	interval = 2;
	red = 0;

	meshes["bar"] = getBarMesh("bar");
	meshes["shot"] = getBarMesh("shot");
	meshes["player"] = getShipMesh("player",glm::vec3(1,0,0));

	enemies = std::vector<Object *>(30);
	shots = std::vector<Bar *>(30);

	for (int i = 0; i < 30; i++) {
		enemies[i] = NULL;
		shots[i] = NULL;
	}
	glLineWidth(3);
	srand((unsigned int)time(NULL));
}

// uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 GeometryWars::VisualizationTransf2DUnif(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}

void GeometryWars::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}

void GeometryWars::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void GeometryWars::Update(float deltaTimeSeconds)
{

	glm::ivec2 resolution = window->GetResolution(); 

	// The viewport is now the full size of the window

	viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
	SetViewportArea(viewSpace, glm::vec3(0), true);

	// Compute uniform 2D visualization matrix
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
	
	if (player->lives != 0) {

		//create new enemy every 2 seconds
		if ((int)Engine::GetElapsedTime() >= interval) {
			interval += 2;
			Object *enemy = new Object(glm::vec2(0, 0), rand() % 2 + 1, (float)0.01 + (float)rand() / (float)(RAND_MAX / 0.02));
			for (int i = 0; i < enemies.size(); i++) {
				if (enemies[i] == NULL) {
					enemies[i] = enemy;
					break;
				}
			}
			//place enemy on circle of radius 3
			enemy->center.x = player->center.x + 3 * cos(enemy->angle);
			enemy->center.y = player->center.y + 3 * sin(enemy->angle);
		}

		//update existing enemies
		for (int i = 0; i < enemies.size(); i++) {
			if (enemies[i] != NULL) {
				
				//move enemies
				enemies[i]->center.x -= enemies[i]->speed*cos(enemies[i]->angle);
				enemies[i]->center.y -= enemies[i]->speed*sin(enemies[i]->angle);

				//compute new angle between enemies and player
				float difX, difY;
				difX = enemies[i]->center.x - player->center.x;
				difY = enemies[i]->center.y - player->center.y;
				enemies[i]->angle = atan2(difY, difX);
				
				//check collision between player and enemies
				if (enemies[i]->checkCollision(player)) {
					delete enemies[i];
					enemies[i] = NULL;
					player->lives--;
				}
				//check collision between shots and enemies
				for (int j = 0; j < shots.size(); j++) {
					if (shots[j] != NULL && enemies[i] != NULL && enemies[i]->checkCollision(shots[j])) {
						delete shots[j];
						shots[j] = NULL;
						enemies[i]->lives--;
						enemies[i]->scale = 0.5;
						enemies[i]->speed *= 2;
						if (enemies[i]->lives == 0) {
							delete enemies[i];
							enemies[i] = NULL;
						}
					}
				}
			}
		}

		//move shots
		for (int i = 0; i < shots.size(); i++) {
			if (shots[i] != NULL) {
				shots[i]->moveBar();

				//delete shot if it is out of bounds
				if (shots[i]->tail.x < 0 || shots[i]->tail.x > logicSpace.width || shots[i]->tail.y < 0 || shots[i]->tail.y > logicSpace.height) {
					delete shots[i];
					shots[i] = NULL;
				}

			}
		}
	}
	else {

		//end game animation
		glClearColor(red, 0, 0, 1);
		if (red <= 0.8) red += (float)0.01;

		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	DrawScene(visMatrix);
}

void GeometryWars::FrameEnd()
{

}

void GeometryWars::DrawScene(glm::mat3 visMatrix)
{	
	//draw enemies
	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i] != NULL) {
			glm::vec3 color(0, 0, 1);
			if (enemies[i]->lives == 2) color = glm::vec3(1, 1, 0);
			modelMatrix = visMatrix * Transform::Translate(enemies[i]->center.x, enemies[i]->center.y) * Transform::Rotate(enemies[i]->angle) * Transform::Scale(enemies[i]->scale, enemies[i]->scale);
			RenderMesh2D(meshes["player"], modelMatrix, color);
			
		}
	} 
	
	//draw health bars
	float healthX = (float)logicSpace.width - (float)0.3;
	for (int i = 0; i < player->lives; i++) {
		modelMatrix = visMatrix * Transform::Translate(healthX, (float)logicSpace.height - (float)0.4);
		RenderMesh2D(meshes["bar"], shaders["VertexColor"], modelMatrix);
		healthX = healthX - (float)0.1;
	} 

	// draw shots
	for (int i = 0; i < shots.size(); i++) {
		if (shots[i] != NULL) {
			modelMatrix = visMatrix *  Transform::Translate(shots[i]->tail.x, shots[i]->tail.y);
			modelMatrix *= Transform::Rotate(shots[i]->angle);
			RenderMesh2D (meshes["shot"], shaders["VertexColor"], modelMatrix);
		}
	}


	//draw player
	modelMatrix = visMatrix * Transform::Translate(player->center.x, player->center.y);
	modelMatrix *= Transform::Rotate(rotationAngle);
	RenderMesh2D(meshes["player"], shaders["VertexColor"], modelMatrix);
}

void GeometryWars::OnInputUpdate(float deltaTime, int mods)
{
	if (window->KeyHold(GLFW_KEY_W) && player->center.y < logicSpace.height) player->moveUp();
	if (window->KeyHold(GLFW_KEY_S) && player->center.y > 0) player->moveDown();
	if (window->KeyHold(GLFW_KEY_A) && player->center.x > 0) player->moveLeft();
	if (window->KeyHold(GLFW_KEY_D) && player->center.x < logicSpace.width) player->moveRight();
}

void GeometryWars::OnKeyPress(int key, int mods)
{

}

void GeometryWars::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void GeometryWars::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (player->lives != 0) {
		//rotate ship
		float mx, my;
		mx = (float)mouseX * (logicSpace.width / window->GetResolution().x);
		my = logicSpace.height - (float)mouseY * (logicSpace.height / window->GetResolution().y);
		float difX = mx - player->center.x;
		float difY = my - player->center.y;
		rotationAngle = atan2(difY, difX);
	}
}

void GeometryWars::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (player->lives != 0) {
		//fire shots
		for (int i = 0; i < shots.size(); i++) {
			if (shots[i] == NULL) {
				shots[i] = new Bar(glm::vec2(player->center.x, player->center.y), glm::vec2(cos(rotationAngle) / 10, sin(rotationAngle) / 10), rotationAngle);
				break;
			}
		}
	}
}

void GeometryWars::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void GeometryWars::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}