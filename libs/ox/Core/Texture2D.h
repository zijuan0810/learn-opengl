#pragma once

#include <string>
#include <filesystem>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Log.h"


namespace satan
{
	struct Texture2D
	{
	public:
		unsigned int ID;
		std::string Type;
		std::string filename;

	public:
		static void Init()
		{
			// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
			stbi_set_flip_vertically_on_load(true);
		}

		static unsigned int GetTexture(const char* filename, bool flipY)
		{
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			//为当前绑定的纹理对象设置环绕、过滤方式
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//加载并生成纹理
			stbi_set_flip_vertically_on_load(flipY);
			int width, height, nrChannels;
			unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
			if (data != nullptr) {
				auto extension = std::filesystem::path(filename).extension();
				if (extension.string() == ".png") {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				}
				else
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else {
				Log::Error("Failed to load texture: %s", filename);
			}
			stbi_image_free(data);
			return texture;
		}
	};
}
