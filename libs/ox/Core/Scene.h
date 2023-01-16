#pragma once

#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Object.h"
#include "Camera.h"
#include "Log.h"
#include "Actor.h"
#include "Object.h"


namespace ox
{
	class Scene : public Actor
	{
	public:
		std::vector<Actor*> m_Actors;
		std::vector<Camera*> m_Cameras;
		Camera* m_MainCamera;

	public:
		Scene(Camera* mainCamera = nullptr);
		~Scene();

	public:
		virtual bool OnInit() { return true; }

		void AddChild(Actor* child);
		void RemoveChild(Actor* child);

	public:
		virtual void Update(float dt);
		virtual void Draw();
		virtual void OnGUI();

	public:
		bool OnKeyCallback(GLFWwindow* window, int key);
		bool OnMouseCallback(GLFWwindow* window, glm::vec2 offset, glm::vec2 mouse);
		bool OnScrollCallback(GLFWwindow* window, glm::vec2 offset);
		bool OnDropCallback(GLFWwindow* window, int count, const char** paths);
	};
}