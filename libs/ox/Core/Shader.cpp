#include "Shader.h"


namespace ox
{
	Shader::Shader(const char* vertexPath, const char* fragmentPath)
		: m_VertPath(vertexPath)
		, m_FragPath(fragmentPath)
	{
	}

	Shader::~Shader()
	{
		glDeleteProgram(ID);
	}

	void Shader::Load()
	{
		// 1. retrieve the vertex/fragment source code from filepath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			vShaderFile.open(m_VertPath);
			fShaderFile.open(m_FragPath);

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

	void Shader::Use()
	{
		glUseProgram(this->ID);
	}

	void Shader::UnUse()
	{
		glUseProgram(0);
	}

	// utility uniform functions
	void Shader::SetBool(const std::string& name, bool value)
	{
		glUniform1i(getUniform(name), (int)value);
	}

	void Shader::SetInt(const std::string& name, int value)
	{
		glUniform1i(getUniform(name), value);
	}

	void Shader::SetFloat(const std::string& name, float value)
	{
		glUniform1f(getUniform(name), value);
	}

	void Shader::SetFloat3(const std::string& name, glm::vec3 value)
	{
		glUniform3fv(getUniform(name), 1, glm::value_ptr(value));
	}

	void Shader::SetMatrix4(const std::string& name, const glm::mat4& value)
	{
		glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::checkCompileErrors(unsigned int shader, std::string type)
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

	unsigned int Shader::getUniform(const std::string& name)
	{
		if (_mapUniforms.find(name) == _mapUniforms.end()) {
			_mapUniforms[name] = glGetUniformLocation(ID, name.c_str());
		}
		return _mapUniforms[name];
	}
}