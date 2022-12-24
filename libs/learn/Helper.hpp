#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void onFramebufferSizeCallback(GLFWwindow* window, int width, int height);

class Helper
{
public:
	// settings
	static const unsigned int SCR_WIDTH = 800;
	static const unsigned int SCR_HEIGHT = 600;

	GLFWwindow* window;
	
public:
	static Helper& Get()
	{
		static Helper ins;
		return ins;
	}

public:
	Helper()
		: window(nullptr)
		, _isRetinaScreen(false)
	{
	}

	void Init()
	{
		this->InitGLFW();
	}

	GLFWwindow* InitGLFW()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// glfw window creation
		// --------------------
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
		if (window == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return nullptr;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, onFramebufferSizeCallback);

		// check retina
		int monitorCount;
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		for (int i = 0; i < monitorCount; i++) {
			int screen_x, screen_y;
			const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
			std::cout << "Screen size is X = " << mode->width << ", Y = " << mode->height << std::endl;

			int width_mm, height_mm;
			glfwGetMonitorPhysicalSize(monitors[i], &width_mm, &height_mm);
			printf("%d, %d\n", width_mm, height_mm);
		}

		_isRetinaScreen = true;

		return window;
	}

	bool IsRetinaScreen()
	{
		return _isRetinaScreen;
	}

private:
	bool _isRetinaScreen;
};

void onFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
