#pragma once

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace satan
{
	class Shader
	{
	public:
		unsigned int ID;

		Shader(const char* vertexPath, const char* fragmentPath)
		{
			// 1. retrieve the vertex/fragment source code from filepath
			std::string vertexCode;
			std::string fragmentCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);

				std::stringstream vShaderStream, fShaderStream;
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();

				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();

				vShaderFile.close();
				fShaderFile.close();
			}
			catch (std::ifstream::failure& e) {
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
			}

			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();

			// 2. compile shaders
			unsigned int vertex, fragment;
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, nullptr);
			glCompileShader(vertex);
			this->checkCompileErrors(vertex, "VERTEX");

			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, nullptr);
			glCompileShader(fragment);
			this->checkCompileErrors(fragment, "FRAGMENT");

			// shader program
			this->ID = glCreateProgram();
			glAttachShader(this->ID, vertex);
			glAttachShader(this->ID, fragment);
			glLinkProgram(this->ID);
			this->checkCompileErrors(ID, "PROGRAM");

			// delete the shader
			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}

		~Shader()
		{
			glDeleteProgram(ID);
		}

		void Use()
		{
			glUseProgram(this->ID);
		}

		// utility uniform functions
		void setBool(const std::string& name, bool value)
		{
			glUniform1i(getUniform(name), (int)value);
		}

		void SetInt(const std::string& name, int value)
		{
			glUniform1i(getUniform(name), value);
		}

		void setFloat(const std::string& name, float value)
		{
			glUniform1f(getUniform(name), value);
		}

		void setFloat3(const std::string& name, glm::vec3 value)
		{
			glUniform3fv(getUniform(name), 1, glm::value_ptr(value));
		}

		void setMat4(const std::string& name, const glm::mat4& value)
		{
			glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, glm::value_ptr(value));
		}

	private:
		// utility function for checking shader
		void checkCompileErrors(unsigned int shader, std::string type)
		{
			int success;
			char infoLog[1024];
			if (type != "PROGRAM") {
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success) {
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog <<
						"\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
			else {
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success) {
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog <<
						"\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
		}

		unsigned int getUniform(const std::string& name)
		{
			if (_mapUniforms.find(name) == _mapUniforms.end()) {
				_mapUniforms[name] = glGetUniformLocation(ID, name.c_str());
			}
			return _mapUniforms[name];
		}

	private:
		std::map<std::string, unsigned int> _mapUniforms;
	};
}
