#include "camera.h"

#define PI 3.14159f

Camera::Camera()
{
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

void Camera::init(glm::vec3 position, glm::vec3 up, glm::vec3 front)
{
	Position = position;
	WorldUp = up;
	Front = front;
	Yaw = YAW;
	Pitch = PITCH;

	MovementSpeed = SPEED;
	MouseSensitivity = SENSITIVITY;
	Zoom = ZOOM;

	current_height = 640;
	current_width = 480;

	updateCameraVectors();
}

void Camera::resizeCameraViewport(int width, int height)
{
	projection = glm::perspective(60.f / 180.f * PI, float(width) / float(height), 0.01f, 100.0f);
	current_height = height;
	current_width = width;
}

glm::mat4 Camera::getProjectionMatrix()
{
	return projection;
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction)
{
	float velocity = MovementSpeed*0.05f;

	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;

	//Position.y = 0.5f; //Comment if you want to move in the vertical direction
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
//As zoom is fixed as 45 as default we want to constrain the zoom between 1 and 45
void Camera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;

}

void Camera::processZoom(Camera_Zoom z, float dist)
{
	if (z == ZOOM_IN and Zoom <= 45.0f)
		Zoom += abs(dist);

	else if (z == ZOOM_OUT and Zoom >= 10.0f)
		Zoom -= abs(dist);

	projection = glm::perspective(glm::radians(Zoom), current_width / current_height, 0.01f, 100.0f);
}

void Camera::updateCameraVectors() {

	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}