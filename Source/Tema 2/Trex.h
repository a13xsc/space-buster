#pragma once
#include <include/glm.h>
#include <include/math.h>

class Trex {

public:
	glm::vec3 center;
	float angle;

	Trex() {
		center = glm::vec3(0, 0, 0);
		angle = 0;
	}

};