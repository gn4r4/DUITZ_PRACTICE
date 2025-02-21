#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraDirection {
	NONE = 0, 
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera {
public: 
	glm::vec3 cameraPos; // позиція камери

	glm::vec3 cameraFront; // рух вперед/назад
	glm::vec3 cameraUp; // рух вліво/вправо
	glm::vec3 cameraRight; // рух вгору/вниз

	glm::vec3 worldUp;

	float yaw; // x-axis
	float pitch; // y-axis
	float speed;
	float zoom;

	Camera(glm::vec3 position);

	void updateCameraDirection(double dx, double dy);
	void updateCameraPosition(CameraDirection direction, float deltaTime);
	void updateCameraZoom(double dy);

	float getZoom();

	glm::mat4 getViewMatrix();

private:
	void updateCameraVectors();

};


#endif
