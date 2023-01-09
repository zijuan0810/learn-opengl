#pragma once

#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "../Utils/Log.h"


namespace satan
{
	class Object
	{
	public:
		glm::vec3 m_Position;
		glm::vec3 m_Scale;
		glm::vec3 m_Rotation;

		Object* m_Parent;
		std::vector<Object*> m_Children;
		int m_InsID;

	public:
		Object()
			: m_Position(glm::vec3(0.0f))
			, m_Scale(glm::vec3(1.0f))
			, m_Rotation(glm::vec3(0.0f))
		{
		}

		~Object()
		{
			for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
				delete* it;
			m_Children.clear();
		}

	public:
		void AddChild(Object* child)
		{
			auto it = std::find_if(m_Children.begin(), m_Children.end(), [child](Object* obj) {
				return obj->m_InsID == child->m_InsID;
			});
			if (it != m_Children.end())
			{
				Log::Error("There has child %d", (*it)->m_InsID);
				return;
			}

			child->m_InsID = ++_ID;
			child->m_Parent = this;
			m_Children.push_back(child);
		}

		void Translate(glm::vec3 translation)
		{
		}

		glm::quat GetQuat()
		{
			glm::quat quat = glm::identity<glm::quat>();
			quat *= glm::angleAxis(glm::radians(m_Rotation.x), glm::vec3(1.0, 0.0, 0.0f)); //x
			quat *= glm::angleAxis(glm::radians(m_Rotation.y), glm::vec3(0.0, 1.0, 0.0f)); //y
			quat *= glm::angleAxis(glm::radians(m_Rotation.z), glm::vec3(0.0, 0.0, 1.0f)); //z
			return quat;
		}

		glm::mat4 GetLocalToWorldMatrix()
		{
			glm::mat4 matrix = glm::mat4(1.0f);
			matrix = glm::translate(matrix, m_Position);
			matrix = glm::mat4_cast(GetQuat()) * matrix;
			matrix = glm::scale(matrix, m_Scale);

			Object* parent = m_Parent;
			while (parent != nullptr)
				matrix *= parent->GetLocalToWorldMatrix();

			return matrix;
		}

	private:
		static int _ID;
	};
}
