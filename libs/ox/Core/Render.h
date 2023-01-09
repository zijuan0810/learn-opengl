#pragma once

#include "Actor.h"
#include "Shader.h"

namespace satan
{
	class Render : Actor
	{
	public:
		Shader* m_Shader;
		glm::vec3 m_Color;

	public:
		Render()
		{
			m_Shader = new Shader();
			m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
		}

	public:
		virtual void Update(float dt) { }

		virtual void Draw() 
		{
			m_Shader->Use();
			m_Shader->SetFloat3("u_color", m_Color);
		}
	};
}
