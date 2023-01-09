#pragma once

#include <string>
#include <filesystem>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
		static void Init()
		{
			// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
			stbi_set_flip_vertically_on_load(true);
		}

		static unsigned int LoadTexture(const std::string& filepath, bool flipY = true)
		{
			unsigned int textureID;
			glGenTextures(1, &textureID);

			//加载并生成纹理
			stbi_set_flip_vertically_on_load(flipY);

			int width, height, nrComponents;
			unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrComponents, 0);
			if (data != nullptr)
			{
				GLenum format;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 3)
					format = GL_RGB;
				else if (nrComponents == 4)
					format = GL_RGBA;

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}
			else
			{
				Log::Error("Texture failed to load at path: %s", filepath.c_str());
				stbi_image_free(data);
			}

			return textureID;
		}
	};
}
