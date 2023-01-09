#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>


namespace satan
{
	class Time
	{
	public:
		static float currTime;
		static float deltaTime; // 当前帧与上一帧的时间差
		static float lastFrame; // 上一帧的时间

	public:
		static void Init()
		{
			currTime = 0.0f;
			deltaTime = 0.0f;
			lastFrame = 0.0f;
		}

		static void Update()
		{
			currTime = glfwGetTime();
			deltaTime = currTime - lastFrame;
			lastFrame = currTime;
		}
	};
}
