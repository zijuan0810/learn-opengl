#include "Scene.h"
#include "Engine.h"

namespace ox
{
	Scene::Scene(Camera* mainCamera)
		: m_MainCamera(mainCamera)
	{
		if (mainCamera == nullptr)
			m_MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 60.0f));
		m_Cameras.push_back(m_MainCamera);

		OnInit();
	}

	Scene::~Scene()
	{
	}

	void Scene::AddChild(Actor* child)
	{
		if (Actor::AddChild(child))
			m_Actors.push_back(child);
	}

	void Scene::RemoveChild(Actor* child)
	{
		if (Actor::RemoveChild(child))
		{
			auto it = std::find_if(m_Actors.begin(), m_Actors.end(), [child](Object* obj) {
				return obj->m_InsID == child->m_InsID;
				});
			if (it != m_Actors.end())
				m_Actors.erase(it);
		}
	}

	void Scene::Update(float dt)
	{
		for (auto it = m_Actors.begin(); it != m_Actors.end(); ++it)
			(*it)->Update(dt);
	}

	void Scene::Draw()
	{
		Engine* engine = Engine::Ins();
		float aspect = static_cast<float>(engine->m_DisplayWidth) / static_cast<float>(engine->m_DisplayHeight);
		for (std::size_t i = 0; i < m_Cameras.size(); ++i)
		{
			auto camera = m_Cameras[i];
			glm::mat4 view = camera->GetViewMatrix();
			glm::mat4 proj = glm::perspective(glm::radians(camera->Zoom), aspect, 0.1f, 100.0f);
			for (auto it = m_Actors.begin(); it != m_Actors.end(); ++it)
				(*it)->Draw(view, proj);
		}
	}

	void Scene::OnGUI()
	{
		for (auto it = m_Actors.begin(); it != m_Actors.end(); ++it)
			(*it)->OnGUI();
	}

	bool Scene::OnKeyCallback(GLFWwindow* window, int key)
	{
		for (auto it = m_Actors.begin(); it != m_Actors.end(); ++it)
		{
			if ((*it)->OnKeyCallback(window, key))
				return true;
		}

		return false;
	}

	bool Scene::OnMouseCallback(GLFWwindow* window, glm::vec2 offset, glm::vec2 mouse)
	{
		for (auto it = m_Actors.begin(); it != m_Actors.end(); ++it)
		{
			if ((*it)->OnMouseCallback(window, offset, mouse))
				return true;
		}

		return false;
	}

	bool Scene::OnScrollCallback(GLFWwindow* window, glm::vec2 offset)
	{
		for (auto it = m_Actors.begin(); it != m_Actors.end(); ++it)
		{
			if ((*it)->OnScrollCallback(window, offset))
				return true;
		}

		return false;
	}

	bool Scene::OnDropCallback(GLFWwindow* window, int count, const char** paths)
	{
		for (auto it = m_Actors.begin(); it != m_Actors.end(); ++it)
		{
			if ((*it)->OnDropCallback(window, count, paths))
				return true;
		}

		return false;
	}
}
