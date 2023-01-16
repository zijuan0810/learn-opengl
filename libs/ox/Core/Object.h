#pragma once

#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Log.h"


namespace ox
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
		Object();
		~Object();

	public:
		bool AddChild(Object* child);
		bool RemoveChild(Object* child);

		void Translate(glm::vec3 translation);

		glm::quat GetQuat();
		glm::mat4 GetModelMatrix();
		glm::mat4 GetLocalToWorldMatrix();

	private:
		static int _ID;
	};
}
