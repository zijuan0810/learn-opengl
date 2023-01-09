#pragma once

#include "Object.h"
#include "Transform.h"

namespace satan
{
	class Actor : Object
	{
	public:
		Transform Transform;

	public:
		virtual void Update(float dt) { }
		virtual void Draw() { }
	};
}

