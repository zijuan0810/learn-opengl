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


namespace ox
{
	class Shader
	{
	public:
		unsigned int ID;
		std::string m_VertPath;
		std::string m_FragPath;

	public:
		Shader(const char* vertexPath = "", const char* fragmentPath = "");
		~Shader();

	public:
		void Load();
		void Use();
		void UnUse();

		// utility uniform functions
		void SetBool(const std::string& name, bool value);
		void SetInt(const std::string& name, int value);
		void SetFloat(const std::string& name, float value);
		void SetFloat3(const std::string& name, glm::vec3 value);
		void SetMatrix4(const std::string& name, const glm::mat4& value);
		

	private:
		void checkCompileErrors(unsigned int shader, std::string type);
		unsigned int getUniform(const std::string& name);

	private:
		std::map<std::string, unsigned int> _mapUniforms;
	};
}
