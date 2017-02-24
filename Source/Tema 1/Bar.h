#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

using namespace std;

class Bar {

	glm::vec2 axisSpeed;

public:

	glm::vec2 tail;
	float angle;

	Bar(glm::vec2 tail, glm::vec2 axisSpeed, float angle) {
		this->angle = angle;
		this->tail = tail;
		this->axisSpeed = axisSpeed;
	}

	void moveBar() {
		tail.x += axisSpeed.x;
		tail.y += axisSpeed.y;
	}

};