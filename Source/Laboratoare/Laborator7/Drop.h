#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

class Drop
{

public:
	glm::vec3 position, scale;
	float speed, weight;
	int lives;

	Drop(glm::vec3 position) {
		this->position = position;
		this->scale = glm::vec3(0.2);
		this->speed = 0;
		this->weight = 0.01;
		this->lives = 5;
	}

	//miscarea picaturii pe verticala; simuleaza actiunea gravitatiei
	void moveDrop() {
		speed -= weight;
		position.y += speed;
		if (position.y <= 0) {
			scale /= 2;
			position.y = 0;
			speed = -speed/2;
			weight /= 1.5;
			lives--;
		}
	}
};