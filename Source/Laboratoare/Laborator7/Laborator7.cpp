#include "Laborator7.h"
#include "Grid.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

float wave_speed = 5, amplitude = 0, wavel = 5;
glm::vec3 center;

Laborator7::Laborator7()
{
}

Laborator7::~Laborator7()
{
}

void Laborator7::Init()
{
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

	//drop position
	dropPosition = glm::vec3(3,5,3);
	
	{
		Mesh* mesh = Grid::CreateGrid("grid", glm::vec3(0,0,0), glm::vec3(0, 0, 0), false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader *shader = new Shader("ShaderLab7");
		shader->AddShader("Source/Laboratoare/Laborator7/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator7/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//Light & material properties
	{
		lightPosition = glm::vec3(5, 5, 5);
		materialShininess = 30;
		materialKd = 0.5;
		materialKs = 0.5;
	}
}

void Laborator7::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);	
}

void Laborator7::Update(float deltaTimeSeconds)
{
	//grid
	{
	glm::mat4 modelMatrix = glm::mat4(1);
	RenderSimpleMesh(meshes["grid"], shaders["ShaderLab7"], modelMatrix, glm::vec3(0, 0.5, 1));
	}
	
	// Render the point light in the scene
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, lightPosition);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
		RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
	}

	//robinet
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, dropPosition);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab7"], modelMatrix, glm::vec3(0.5, 0.5, 0.5));
	}

	//picaturi
	pos = -1;
	for (int i = 0; i < drops.size(); i++) {
		drops[i].moveDrop();
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, drops[i].position);
		modelMatrix = glm::scale(modelMatrix, drops[i].scale);
		RenderSimpleMesh(meshes["sphere"], shaders["VertexColor"], modelMatrix, glm::vec3(0, 0.5, 1));
		//lives controleaza de cate ori face bounce picatura
		if (drops[i].lives == 0) {
			pos = i;
		}
		if (drops[i].position.y == 0) {
			amplitude = 0.3 ;
			center = drops[i].position;
		}
	}
	if (pos != -1) {
		drops.erase(drops.begin() + pos);
	}

	if (amplitude > 0)
		amplitude -= 0.001;
}

void Laborator7::FrameEnd()
{
	//DrawCoordinatSystem();
}

void Laborator7::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, const glm::vec3 &color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Set shader uniforms for light & material properties
	// TODO: Set light position uniform
	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3fv(light_position, 1, glm::value_ptr(lightPosition));

	// TODO: Set eye position (camera position) uniform
	glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3fv(eye_position, 1, glm::value_ptr(eyePosition));

	// TODO: Set material property uniforms (shininess, kd, ks, object color) 
	
	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);
	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3fv(object_color, 1, glm::value_ptr(color));
	
	//send wave uniforms
	string id = mesh->GetMeshID();
	int wave = (!id.compare("grid")) ? 1 : 0;

	int wave_location = glGetUniformLocation(shader->program, "wave");
	glUniform1i(wave_location, wave);

	float time = Engine::GetElapsedTime();
	int time_location = glGetUniformLocation(shader->program, "time");
	glUniform1f(time_location, time);

	int wave_center_location = glGetUniformLocation(shader->program, "center");
	glUniform3fv(wave_center_location, 1, glm::value_ptr(center));

	int amplitude_location = glGetUniformLocation(shader->program, "amplitude");
	glUniform1f(amplitude_location, amplitude);

	int wavelength_location = glGetUniformLocation(shader->program, "wavelength");
	glUniform1f(wavelength_location, wavel);

	int speed_location = glGetUniformLocation(shader->program, "speed");
	glUniform1f(speed_location, wave_speed);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	
	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator7::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 2;

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = GetSceneCamera()->transform->GetLocalOXVector();
		glm::vec3 forward = GetSceneCamera()->transform->GetLocalOZVector();
		forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

		// Control light position using on W, A, S, D, E, Q
		if (window->KeyHold(GLFW_KEY_W)) lightPosition -= forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_A)) lightPosition -= right * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_S)) lightPosition += forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_D)) lightPosition += right * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_E)) lightPosition += up * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_Q)) lightPosition -= up * deltaTime * speed;
	
		if (window->KeyHold(GLFW_KEY_UP)) dropPosition -= forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_LEFT)) dropPosition -= right * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_DOWN)) dropPosition += forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_RIGHT)) dropPosition += right * deltaTime * speed;

		if (window->KeyHold(GLFW_KEY_MINUS)) wave_speed -= 0.05;
		if (window->KeyHold(GLFW_KEY_EQUAL)) wave_speed += 0.05;
	}
}

void Laborator7::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE) {
		drops.push_back(Drop(dropPosition));
	}
}

void Laborator7::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator7::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator7::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator7::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator7::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator7::OnWindowResize(int width, int height)
{
}
