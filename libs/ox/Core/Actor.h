#pragma once

#include "Object.h"
#include "Shader.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace ox
{
	class Actor : public Object
	{
	public:
		Shader* m_Shader;
		glm::vec3 m_Color;

	public:
		Actor()
		{
			m_Shader = new Shader();
			m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
		}

	public:
		virtual void OnGUI() { }
		virtual void Update(float dt) { }

		virtual void Draw(const glm::mat4& view, const glm::mat4& proj) 
		{
			m_Shader->Use();
			m_Shader->SetFloat3("u_color", m_Color);
		}


	public:
		virtual bool OnKeyCallback(GLFWwindow* window, int key)
		{
			return false;
		}

		virtual bool OnMouseCallback(GLFWwindow* window, const glm::vec2& offset, const glm::vec2& mouse)
		{
			return false;
		}

		virtual bool OnScrollCallback(GLFWwindow* window, const glm::vec2& offset)
		{
			return false;
		}

		virtual bool OnDropCallback(GLFWwindow* window, int count, const char** paths)
		{
			return false;
		}
	};
}

