#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#define TEXTURE_INDEX(x) GL_TEXTURE##x


class Texture
{
public:
	unsigned int ID;

public:
	Texture(const char* filename)
		: _data(nullptr)
		, _filename(filename)
	{
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set the texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		this->loadImage();
	}

	~Texture()
	{
		if (_data != nullptr) {
			stbi_image_free(_data);
		}
	}

	void Active(int index)
	{
		static GLenum indices[16] = {
			GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, 
			GL_TEXTURE8, GL_TEXTURE9, GL_TEXTURE10, GL_TEXTURE11, GL_TEXTURE12, GL_TEXTURE13, GL_TEXTURE14, GL_TEXTURE15,
		};
		glActiveTexture(indices[index]);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

private:
	void loadImage()
	{
		stbi_set_flip_vertically_on_load(true);
		_data = stbi_load(_filename.c_str(), &_width, &_height, &_nrChannels, 0);
		if (_data) {
			if (_nrChannels == 3) { //jpg
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _data);
			}
			else if (_nrChannels == 4) { //png
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data);
			}

			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			printf("Failed to load texture: %s\n", _filename.c_str());
		}
	}

private:
	int _width;
	int _height;
	int _nrChannels;

	unsigned char* _data;

	std::string _filename;
};

#endif