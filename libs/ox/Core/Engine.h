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
#include "Utils/Time.h"


namespace ox
{
	class Engine
	{
	public:
		Scene* m_Scene;
		GLFWwindow* m_Window = nullptr;
		glm::vec4 m_ClearColor;
		float m_DPIFactor = 1.0;
		
	public:
		static Engine* Ins()
		{
			static Engine engine;
			return &engine;
		}

	public:
		void Init(Scene* scene)
		{
			m_LastMouse = glm::vec2(400.0f, 300.0f);
			m_ClearColor = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);

			InitGlfw();
			InitGlad();
			InitInput();
			Time::Init();
			Gui::Ins()->Init(m_Window, m_DPIFactor);

			Texture2D::Init();

			m_Scene = scene == nullptr ? new Scene() : scene;
		}

		void Run()
		{
			Time::Update();
			OnKeyCallback();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();

			m_Scene->OnGUI();
			m_Scene->Update(Time::deltaTime);

			
			BeginRender();
			{
				m_Scene->Draw();
			}
			EndedRender();
		}

		bool WillClose()
		{
			return glfwWindowShouldClose(m_Window);
		}

	private:
		void BeginRender()
		{
			glfwGetFramebufferSize(m_Window, &m_DisplayWidth, &m_DisplayHeight);
			glViewport(0, 0, m_DisplayWidth, m_DisplayHeight);
			glClearColor(m_ClearColor.x * m_ClearColor.w,
				m_ClearColor.y * m_ClearColor.w,
				m_ClearColor.z * m_ClearColor.w, m_ClearColor.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
		}

		void EndedRender()
		{
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(m_Window);
			glfwPollEvents();
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

		void InitInput()
		{
			glfwSetDropCallback(m_Window, drop_callback);
			glfwSetScrollCallback(m_Window, scroll_callback);
			glfwSetCursorPosCallback(m_Window, mouse_callback);
		}
	
	private:
		void OnKeyCallback()
		{
			if (ImGui::IsAnyItemHovered())
				return;

			if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(m_Window, true);
			else
				m_Scene->OnKeyCallback(m_Window, glfwGetKey(m_Window, GLFW_KEY_W));

			/*if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
				camera.ProcessKeyboard(satan::FORWARD, deltaTime);
			if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
				camera.ProcessKeyboard(satan::BACKWARD, deltaTime);
			if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
				camera.ProcessKeyboard(satan::LEFT, deltaTime);
			if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
				camera.ProcessKeyboard(satan::RIGHT, deltaTime);*/
		}

		void OnMouse(GLFWwindow* window, double xpos, double ypos)
		{
			auto mouse = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));

			if (m_FirstMouse)
			{
				m_LastMouse = mouse;
				m_FirstMouse = false;
			}

			// 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
			m_MouseOffset.x = mouse.x - m_LastMouse.x;
			m_MouseOffset.y = m_LastMouse.y - mouse.y;
			m_MouseOffset *= m_Sensitivity;

			m_Scene->OnMouseCallback(m_Window, m_MouseOffset, mouse);

			/*if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
				camera.ProcessMouseMovement(xoffset, yoffset);
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				rotation.x -= yoffset * 2.0f;
				rotation.y += xoffset * 2.0f;
			}*/

			m_LastMouse.x = static_cast<float>(xpos);
			m_LastMouse.y = static_cast<float>(ypos);
		}

		void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
		{
			auto offset = glm::vec2(static_cast<float>(xoffset), static_cast<float>(yoffset));

			ImGuiIO& io = ImGui::GetIO();
			io.AddMouseWheelEvent(offset.x, offset.y);
			if (io.WantCaptureMouse)
				return;

			m_Scene->OnScrollCallback(m_Window, offset);

			//camera.ProcessMouseScroll(static_cast<float>(yoffset));
		}

		void OnDrop(GLFWwindow* window, int count, const char** paths)
		{
			m_Scene->OnDropCallback(m_Window, count, paths);

			//for (int i = 0; i < count; i++)
			//{
			//	Log::info(paths[i]);
			//	auto extension = std::filesystem::path(paths[i]).extension().string();
			//	model->Load(paths[i]);
			//	/*if (extension == ".png" || extension == ".jpg") {
			//		if (i == 0)
			//			textureId1 = satan::GetTexture(paths[i], true);
			//		else if (i == 1)
			//			textureId2 = satan::GetTexture(paths[i], true);
			//	}*/
			//}
		}

	private:
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddMousePosEvent((float)xpos, (float)ypos);
			if (io.WantCaptureMouse)
				return;

			Engine::Ins()->OnMouse(window, xpos, ypos);
		}

		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddMouseWheelEvent((float)xoffset, (float)yoffset);
			if (io.WantCaptureMouse)
				return;

			Engine::Ins()->OnScroll(window, xoffset, yoffset);

			//camera.ProcessMouseScroll(static_cast<float>(yoffset));
		}

		static void drop_callback(GLFWwindow* window, int count, const char** paths)
		{
			Engine::Ins()->OnDrop(window, count, paths);
		}
	

	public:
		glm::vec2 m_LastMouse;
		glm::vec2 m_MouseOffset;
		bool m_FirstMouse = true;
		float m_Sensitivity = 0.05f;
		int m_DisplayWidth;
		int m_DisplayHeight;
	

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
