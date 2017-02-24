#include "BuildandSurvive.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

#define speed 0.1

using namespace std;

BuildandSurvive::BuildandSurvive()
{
}

BuildandSurvive::~BuildandSurvive()
{
	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i] != NULL) {
			delete enemies[i];
			enemies[i] = NULL;
		}
	}
}

void BuildandSurvive::Init()
{
	fov = RADIANS(60);
	dim = 5;
	interval = 5;
	step = 5;

	renderCameraTarget = false;
	trex.angle = 0;
	camera = new Laborator::Camera();
	camera->Set(glm::vec3(0, 2, -3.5f), glm::vec3(0,1,0), glm::vec3(0, 1, 0));

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("trex");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Characters", "trex.obj");
		meshes["trex"] = mesh;
	}

	{
		Mesh* mesh = new Mesh("enemy");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Characters", "wendy_Scene.obj");
		meshes["enemy"] = mesh;
	}

	{
		Mesh* mesh = new Mesh("tower");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Props", "tower.obj");
		meshes["tower"] = mesh;
	}

	enemies = std::vector<Enemy*>(100);

	for (int i = 0; i < enemies.size(); i++) {
		enemies[i] = NULL;
	}

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}

void BuildandSurvive::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void BuildandSurvive::Update(float deltaTimeSeconds)
{

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() - glm::vec3(0,1,0));
	modelMatrix = glm::rotate(modelMatrix, trex.angle, glm::vec3(0,1,0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3, 0.3, 0.3));
	RenderMesh(meshes["trex"], shaders["VertexNormal"], modelMatrix);

	
	//tower 1
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(10,0,5));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01, 0.01));
	RenderMesh(meshes["tower"], shaders["VertexNormal"], modelMatrix);
	
	//tower 2
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-10, 0, 5));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01, 0.01));
	RenderMesh(meshes["tower"], shaders["VertexNormal"], modelMatrix);

	//tower 3
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, -10));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01, 0.01));
	RenderMesh(meshes["tower"], shaders["VertexNormal"], modelMatrix);

	if ((int)Engine::GetElapsedTime() >= interval) {
		interval += step;
		if (step > 1) step -= 0.25;
		for (int i = 0; i < enemies.size(); i++) {
			if (enemies[i] == NULL) {
				enemies[i] = new Enemy();
				break;
			}
		}
	}

	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i] != NULL) {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, enemies[i]->position);
			modelMatrix = glm::rotate(modelMatrix, enemies[i]->angle, glm::vec3(0, 1, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(5, 5, 5));
			RenderMesh(meshes["enemy"], shaders["VertexNormal"], modelMatrix);

			if (enemies[i]->position.x > -15 && enemies[i]->position.z == 15) {
				enemies[i]->position.x -= speed;
			}
			else if (enemies[i]->position.z < -15) {
				enemies[i]->position.x -= speed;
				enemies[i]->angle = -3.14 / 2;
				if (enemies[i]->position.x < -15) {
					delete enemies[i];
					enemies[i] = NULL;
				}
			}
			else {
				enemies[i]->angle = 3 * 3.14 / 4;
				enemies[i]->position.x += speed;
				enemies[i]->position.z -= speed;
			}
		}
	}
}

void BuildandSurvive::FrameEnd()
{
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

void BuildandSurvive::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void BuildandSurvive::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only if MOUSE_RIGHT button is pressed
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f * deltaTime;

		if (window->KeyHold(GLFW_KEY_W)) {
			// TODO : translate the camera forward
			camera->MoveForward(cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// TODO : translate the camera to the left
			camera->TranslateRight(-cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// TODO : translate the camera backwards
			camera->MoveForward(-cameraSpeed);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// TODO : translate the camera to the right
			camera->TranslateRight(cameraSpeed);
		}
	}
}

void BuildandSurvive::OnKeyPress(int key, int mods)
{
	
}

void BuildandSurvive::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void BuildandSurvive::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event

	float sensivity = 0.001f;
	
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		if (window->GetSpecialKeyState() == 0) {

			//renderCameraTarget = false;
			camera->RotateThirdPerson_OY(-deltaX * sensivity);
			trex.angle -= deltaX * sensivity;
		}
	}
}

void BuildandSurvive::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void BuildandSurvive::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void BuildandSurvive::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void BuildandSurvive::OnWindowResize(int width, int height)
{
}
