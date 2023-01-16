#pragma once

#include <string>
#include <filesystem>
#include <glad/glad.h>
#include <glfw/glfw3.h>


#include "Log.h"


namespace ox
{
	struct Texture2D
	{
	public:
		unsigned int ID;
		std::string Type;
		std::string filename;

	public:
		static void Init();
		static unsigned int LoadTexture(const std::string& filepath, bool flipY = true);
	};
}
