#pragma once

#include <glm/glm.hpp>

class Camera
{

public:

	Camera();
	~Camera();


protected:

	// Camera position and orientation
	glm::mat4 cam;

};
