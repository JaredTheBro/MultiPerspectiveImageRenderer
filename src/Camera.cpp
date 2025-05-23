#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

Camera::Camera()
{
	camPos = glm::vec3(0, 10, 40);
	camRot = { 0, 0, 0 };

	camView = glm::mat4(1.0);
	camProj = glm::mat4(1.0);

	moveStep = 0.25f;
	rotStep = 2.5f; // degrees
}

Camera::~Camera()
{}

glm::vec3 Camera::getPos()
{
	return camPos;
}

void Camera::setPos(glm::vec3 pos)
{
	camPos = pos;
}

CamOrientation Camera::getRot()
{
	return camRot;
}

void Camera::setRot(CamOrientation rot)
{
	camRot = rot;
}

glm::vec3 Camera::getLookPoint()
{
	return calcLookPoint();
}

void Camera::updateCamMatrices()
{
	camView = glm::lookAt(camPos, calcLookPoint(), calcUp());
}

glm::vec3 Camera::calcLookPoint()
{
	glm::vec3 forward = calcForward();
	// printf("Angle: p %f y %f\n", camRot.pitch, camRot.yaw);
	// printf("Forward: %f %f %f\n\n", forward.x, forward.y, forward.z);
	return camPos + calcForward();
}

glm::vec3 Camera::calcForward()
{
	return glm::normalize(glm::vec3(
		sinf(glm::radians(camRot.yaw)),
		0,
		-cosf(glm::radians(camRot.yaw))
	));
	/*
	return glm::normalize(glm::vec3(
		cosf(glm::radians(camRot.pitch)) * sinf(glm::radians(camRot.yaw)),
		sinf(glm::radians(camRot.pitch)),
		-cosf(glm::radians(camRot.pitch)) * cosf(glm::radians(camRot.yaw))
	));
	*/
}

glm::vec3 Camera::calcRight()
{
	return glm::normalize(glm::cross(calcForward(), glm::vec3(0, 1, 0)));
}

glm::vec3 Camera::calcUp()
{
	return glm::normalize(glm::cross(calcRight(), calcForward()));
}

void Camera::moveForward()
{
	camPos += calcForward() * moveStep;
	updateCamMatrices();
}

void Camera::moveBackward()
{
	camPos += -calcForward() * moveStep;
	updateCamMatrices();
}

void Camera::moveLeft()
{
	camPos += -calcRight() * moveStep;
	updateCamMatrices();
}

void Camera::moveRight()
{
	camPos += calcRight() * moveStep;
	updateCamMatrices();
}

void Camera::moveUp()
{
	camPos += calcUp() * moveStep;
	updateCamMatrices();
}

void Camera::moveDown()
{
	camPos += -calcUp() * moveStep;
	updateCamMatrices();
}

void Camera::lookUp()
{
	camRot.pitch += rotStep;
	if (camRot.pitch >= 360) {
		camRot.pitch -= 360;
		printf("Reset\n");
	}
	updateCamMatrices();
}

void Camera::lookDown()
{
	camRot.pitch -= rotStep;
	if (camRot.pitch < 0) {
		camRot.pitch += 360;
		printf("Reset\n");
	}
	updateCamMatrices();
}

void Camera::lookLeft()
{
	camRot.yaw -= rotStep;
	if (camRot.yaw < 0) {
		camRot.yaw += 360;
	}
	updateCamMatrices();
}

void Camera::lookRight()
{
	camRot.yaw += rotStep;
	if (camRot.yaw >= 360) {
		camRot.yaw -= 360;
	}
	updateCamMatrices();
}
