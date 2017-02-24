#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

using namespace std;

Mesh *getBarMesh(string id) {

	glm::vec3 color(1, 1, 0);

	glm::vec3 corner(0,0,0);
	float length = (float) 0.2;

	std::vector<VertexFormat> vertices;

	if (id == "bar") {
		vertices =
		{
			VertexFormat(corner, color),
			VertexFormat(corner + glm::vec3(0, length, 0), color)
		};
	}
	else {
		vertices =
		{
			VertexFormat(corner, color),
			VertexFormat(corner + glm::vec3(length, 0, 0), color)
		};
	}

	Mesh* square = new Mesh(id);
	std::vector<unsigned short> indices = { 0, 1 };

	square->SetDrawMode(GL_LINE_LOOP);

	square->InitFromData(vertices, indices);
	return square;
}

Mesh *getShipMesh(string id, glm::vec3 color) {
	
	float wing = (float)0.2;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(0,0,0), color),
		VertexFormat(glm::vec3(wing, wing, 0), color),
		VertexFormat(glm::vec3(wing, -wing, 0), color),
		VertexFormat(glm::vec3(-wing, wing, 0), color),
		VertexFormat(glm::vec3(-wing, -wing, 0), color)
	};

	Mesh *ship = new Mesh(id);
	std::vector<unsigned short> indices = { 0, 3, 1, 0, 2, 4 };

	ship->InitFromData(vertices, indices);
	return ship;

}