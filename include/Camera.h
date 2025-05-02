#pragma once

#include <glm/glm.hpp>

struct CamOrientation
{
	float pitch; // look up/down
	float yaw; // look left/right
	float roll; // tilt left/right
};

class Camera
{

public:

	Camera();
	~Camera();

	// Camera control
	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void lookLeft();
	void lookRight();
	void lookUp();
	void lookDown();

	// Getters
	glm::vec3 getPos();
	CamOrientation getRot();
	glm::vec3 getLookPoint();

protected:

	// Camera position and orientation
	glm::vec3 camPos;	// Camera position
	struct CamOrientation camRot; // Camera orientation

	// Camera matrices
	glm::mat4 camView;
	glm::mat4 camProj;

	float moveStep;
	float rotStep;

	// Update camera matrices when position/orientation changed
	void updateCamMatrices();
	
	// Calculate the point the camera is looking at using its position and orientation
	glm::vec3 calcLookPoint();
	// Calculate the camera's up vector
	glm::vec3 calcUp();
	// Calculate camera's forward vector
	glm::vec3 calcForward();
	// Calculate camera's right vector
	glm::vec3 calcRight();

};
