#pragma once

#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Object.h"
#include "Camera.h"
#include "../Utils/Log.h"


namespace satan
{
	class Scene : Object
	{
	public:
		std::vector<Camera*> m_Cameras;

	public:
		Scene()
		{
		}

		~Scene()
		{
		}

	public:
	};
}