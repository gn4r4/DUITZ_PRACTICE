#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/shader.h"
#include "graphics/texture.h"

#include "graphics/light/light.h"

#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/camera.h"
#include "io/screen.h"

void processInput(double dt);

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

Screen screen;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool flashLightOn = true;

glm::mat4 transform = glm::mat4(1.0f);

int main() {
	std::cout << "Hello world!" << std::endl;

	int success;
	char infoLog[512];

	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// openGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

# ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAP, GL_TRUE);
# endif

	if (!screen.init()) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	screen.setParameters();

	// shaders
	std::cout << "Shaders" << std::endl;
	Shader shader("assets/object.vs", "assets/object.fs");
	Shader lampShader("assets/object.vs", "assets/lamp.fs");
	
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	Cube cubes[10];
	for (unsigned int i = 0; i < 10; i++) {
		cubes[i] = Cube(Material::gold, cubePositions[i], glm::vec3(1.0f));
		cubes[i].init();
	}

	glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
	};

	Lamp lamps[4];
	for (unsigned int i = 0; i < 4; i++) {
		lamps[i] = Lamp(glm::vec3(1.0f),
			glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f),
			1.0f, 0.07f, 0.032f,
			pointLightPositions[i], glm::vec3(0.25f));
		lamps[i].init();
	}

	//DirLight dirLight = { glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.1f), glm::vec3(0.4f), glm::vec3(0.7f) };
	SpotLight spotLight = {
		camera.cameraPos, camera.cameraFront,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
		1.0f, 0.07, 0.032f,
		glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f)
	};

	while (!screen.shouldClose()) {
	
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
				
		// process input
		processInput(deltaTime);

		// render
		screen.update();

		shader.activate();
		//shader.set3Float("light.position", lamp.pos);
		shader.set3Float("viewPos", camera.cameraPos);

		/*dirLight.direction = glm::vec3(
			glm::rotate(glm::mat4(1.0f), glm::radians(0.05f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(dirLight.direction, 1.0f));
		dirLight.render(shader);*/

		for (int i = 0; i < 4; ++i) {
			lamps[i].pointLight.render(shader, i);
		}
		shader.setInt("noPointLights", 4);

		if (flashLightOn) {
			spotLight.position = camera.cameraPos;
			spotLight.direction = camera.cameraFront;
			spotLight.render(shader, 0);
			shader.setInt("noSpotLights", 1);
		}
		else {
			shader.setInt("noSpotLights", 0);
		}

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = camera.getViewMatrix();
		projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		for (Cube& cube : cubes) {
			cube.render(shader);
		}
		

		lampShader.activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);

		//dirLight.render(shader);

		for (Lamp& lamp : lamps) {
			lamp.render(lampShader);
			//lamp.pointLight.render(shader);
		}

		// send new frame to window
		screen.newFrame();
	}

	// cleanup
	for (Cube& cube : cubes) {
		cube.cleanup();
	}
	for (Lamp& lamp : lamps) {
		lamp.cleanup();
	}

	glfwTerminate();

	return 0;
}


void processInput(double dt) {

	// close window
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		screen.setShouldClose(true);
	}

	// turn on flashlight
	if (Keyboard::keyWentDown(GLFW_KEY_F)) {
		flashLightOn = !flashLightOn;
	}

	// move camera
	if (Keyboard::key(GLFW_KEY_W)) {
		camera.updateCameraPosition(CameraDirection::FORWARD, dt);
	}
	if (Keyboard::key(GLFW_KEY_S)) {
		camera.updateCameraPosition(CameraDirection::BACKWARD, dt);
	}
	if (Keyboard::key(GLFW_KEY_D)) {
		camera.updateCameraPosition(CameraDirection::RIGHT, dt);
	}
	if (Keyboard::key(GLFW_KEY_A)) {
		camera.updateCameraPosition(CameraDirection::LEFT, dt);
	}
	if (Keyboard::key(GLFW_KEY_SPACE)) {
		camera.updateCameraPosition(CameraDirection::UP, dt);
	}
	if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
		camera.updateCameraPosition(CameraDirection::DOWN, dt);
	}

	double dx = Mouse::getDX() * 0.1f;
	double dy = Mouse::getDY() * 0.1f;

	if (dx != 0 || dy != 0) {
		camera.updateCameraDirection(dx, dy);
	}

	double scrollDy = Mouse::getScrollDY();

	if (scrollDy != 0) {
		camera.updateCameraZoom(scrollDy);
	}

}