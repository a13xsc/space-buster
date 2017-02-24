#include "Grid.h"

#include <Core/Engine.h>
#include<iostream>
Mesh* Grid::CreateGrid(std::string name, glm::vec3 leftBottomCorner, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices;

	//se creeaza vertexurile din plasa poligonala
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			vertices.push_back(VertexFormat(glm::vec3(i,0,j), color));
		}
	}

	Mesh* grid = new Mesh(name);
	std::vector<unsigned short> indices;

	//matrice ajutatoare pentru generarea triunghiurilor folosite in colorarea plasei
	int count = 0;
	int mat[100][100];
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			mat[i][j] = count;
			count++;
		}
	}

	//creearea triunghiurilor pentru colorarea gridului
	for (int i = 0; i < 99; i++) {
		for (int j = 0; j < 99; j++) {
			indices.push_back(mat[i][j]);
			indices.push_back(mat[i][j + 1]);
			indices.push_back(mat[i + 1][j + 1]);

			indices.push_back(mat[i][j]);
			indices.push_back(mat[i + 1][j + 1]);
			indices.push_back(mat[i + 1][j]);
		}
	}

	grid->InitFromData(vertices, indices);
	return grid;
}