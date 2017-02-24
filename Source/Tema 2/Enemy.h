#pragma once
#include <include/glm.h>
#include <include/math.h>

class Enemy {
public:
	glm::vec3 position;
	float angle;
	float speed;

	Enemy() {
		angle = -3.14 / 2;
		position = glm::vec3(15,0.7,15);
		speed = 0.1;
	}

};