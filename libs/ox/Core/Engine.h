#pragma once

#define _CRT_SECURE_NO_WARNINGS


#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>
#include <iostream>

#include "Log.h"
#include "Scene.h"
#include "Texture2D.h"
#include "Utils/Gui.h"


namespace satan
{
	class Engine
	{
	public:
		Scene* m_ActiveScene;
		GLFWwindow* m_Window = nullptr;
		float m_DPIFactor = 1.0;
		

	public:
		void Init()
		{
			InitGlfw();
			InitGlad();
			GuiHelper::Init(m_Window, m_DPIFactor);

			Texture2D::Init();
		}

	private:
		bool InitGlfw(int width = 800, int height = 600, const char* title = "Learn OpenGL")
		{
			glfwSetErrorCallback(OnErrorCallback);

			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _WIN32
			// if it's a HighDPI monitor, try to scale everything
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			float xscale, yscale;
			glfwGetMonitorContentScale(monitor, &xscale, &yscale);
			Log::info("Monitor scale: %.2f x %.2f", xscale, yscale);
			if (xscale > 1 || yscale > 1) {
				m_DPIFactor = xscale;
				glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
			}
#endif

			m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
			if (m_Window == nullptr) {
				Log::Error("Failed to create GLFW Window");
				glfwTerminate();
				return false;
			}

			glfwSetWindowSizeLimits(m_Window,
				static_cast<int>(width * m_DPIFactor),
				static_cast<int>(height * m_DPIFactor),
				GLFW_DONT_CARE,
				GLFW_DONT_CARE
			);

			glfwSetFramebufferSizeCallback(m_Window, OnFramebufferSizeCallback);

			glfwMakeContextCurrent(m_Window);
			glfwSwapInterval(1); // VSync

			Log::info("GLFW initialized");

			return true;
		}

		bool InitGlad()
		{
			// load all OpenGL function pointers with glad
			// without it not all the OpenGL functions will be available,
			// such as glGetString(GL_RENDERER), and application might just segfault
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
				Log::Error("Couldn't initialize GLAD");
				return false;
			}
			else {
				Log::info("GLAD initialized");
			}

			Log::info("OpenGL renderer: %s", glGetString(GL_RENDERER));
			Log::info("OpenGL from glad %d.%d", GLVersion.major, GLVersion.minor);

			return true;
		}

	

	

	

	private:
		static void OnFramebufferSizeCallback(GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		}

		static void OnErrorCallback(int error, const char* description)
		{
			std::cerr << "[ERROR] GLFW error: " << error << ", " << description << std::endl;
		}
	};
}
