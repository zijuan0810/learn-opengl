#include "Object.h"

namespace ox
{
	int Object::_ID = 0;

	Object::Object()
		: m_Position(glm::vec3(0.0f))
		, m_Scale(glm::vec3(1.0f))
		, m_Rotation(glm::vec3(0.0f))
	{
	}

	Object::~Object()
	{
		for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
			delete* it;
		m_Children.clear();
	}

	bool Object::AddChild(Object* child)
	{
		auto it = std::find_if(m_Children.begin(), m_Children.end(), [child](Object* obj) {
			return obj->m_InsID == child->m_InsID;
			});
		if (it != m_Children.end())
		{
			Log::Error("There has child %d", (*it)->m_InsID);
			return false;
		}

		child->m_InsID = ++_ID;
		child->m_Parent = this;
		m_Children.push_back(child);

		return false;
	}

	bool Object::RemoveChild(Object* child)
	{
		auto it = std::find_if(m_Children.begin(), m_Children.end(), [child](Object* obj) {
			return obj->m_InsID == child->m_InsID;
			});
		if (it != m_Children.end())
		{
			m_Children.erase(it);
			return true;
		}

		return false;
	}

	void Object::Translate(glm::vec3 translation)
	{
	}

	glm::quat Object::GetQuat()
	{
		glm::quat quat = glm::identity<glm::quat>();
		quat *= glm::angleAxis(glm::radians(m_Rotation.x), glm::vec3(1.0, 0.0, 0.0f)); //x
		quat *= glm::angleAxis(glm::radians(m_Rotation.y), glm::vec3(0.0, 1.0, 0.0f)); //y
		quat *= glm::angleAxis(glm::radians(m_Rotation.z), glm::vec3(0.0, 0.0, 1.0f)); //z
		return quat;
	}

	glm::mat4 Object::GetModelMatrix()
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_Position);
		model = glm::mat4_cast(GetQuat()) * model;
		model = glm::scale(model, m_Scale);
		return model;
	}

	glm::mat4 Object::GetLocalToWorldMatrix()
	{
		glm::mat4 matrix = GetModelMatrix();

		Object* parent = m_Parent;
		while (parent != nullptr)
		{
			matrix *= parent->GetLocalToWorldMatrix();
			parent = parent->m_Parent;
		}

		return matrix;
	}
}