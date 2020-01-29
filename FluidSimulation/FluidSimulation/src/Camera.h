#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

enum Camera_Zoom {
	ZOOM_IN,
	ZOOM_OUT
};

//Default values
const float YAW = -90.0f;
const float PITCH = -30.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera();
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	void ProcessKeyboard(Camera_Movement direction);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch);
	void ProcessMouseScroll(float yoffset);
	void resizeCameraViewport(int width, int height);
	void init(glm::vec3 position, glm::vec3 up, glm::vec3 front);
	void processZoom(Camera_Zoom z, float dist);

private:
	void updateCameraVectors();

	glm::mat4 projection;
	float current_height;
	float current_width;
};