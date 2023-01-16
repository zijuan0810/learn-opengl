#pragma once

namespace ox
{
	class Time
	{
	public:
		static float currTime;
		static float deltaTime; // 当前帧与上一帧的时间差
		static float lastFrame; // 上一帧的时间

	public:
		static void Init();
		static void Update();
	};
}
