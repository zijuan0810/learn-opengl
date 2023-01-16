#include "Time.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace ox
{
	float Time::currTime = 0.0f;
	float Time::deltaTime = 0.0f; // 当前帧与上一帧的时间差
	float Time::lastFrame = 0.0f; // 上一帧的时间

	void Time::Init()
	{
		currTime = 0.0f;
		deltaTime = 0.0f;
		lastFrame = 0.0f;
	}

	void Time::Update()
	{
		Time::currTime = static_cast<float>(glfwGetTime());
		deltaTime = currTime - lastFrame;
		lastFrame = currTime;
	}
}
