#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

#include "Bar.h"

using namespace std;

class Object {

public:

	glm::vec2 center;
	glm::vec3 color;
	float angle;
	int lives;
	float speed, scale;

	Object(glm::vec2 center, int lives, float speed = 0.05) {
		this->speed = speed;
		this->lives = lives;
		this->center = center;
		this->scale = 1;
		angle = (float)rand() * (float)6.28 / (float)RAND_MAX;
	}

	void moveUp() {
		center.y += speed;
	}
	void moveDown() {
		center.y -= speed;
	}
	void moveRight() {
		center.x += speed;
	}
	void moveLeft() {
		center.x -= speed;
	}

	bool checkCollision(Object *ship) {
		bool collisionX = this->center.x + 0.1 >= ship->center.x && ship->center.x + 0.1 >= this->center.x;
		bool collisionY = this->center.y + 0.1 >= ship->center.y && ship->center.y + 0.1 >= this->center.y;
		return collisionX && collisionY;
	}

	bool checkCollision(Bar *shot) {
		bool collisionX = this->center.x + 0.3 >= shot->tail.x && shot->tail.x + 0.3 >= this->center.x;
		bool collisionY = this->center.y + 0.3 >= shot->tail.y && shot->tail.y + 0.3 >= this->center.y;
		return collisionX && collisionY;
	}
};

