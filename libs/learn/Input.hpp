#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <functional>


void _glfwOnScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void _glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void _glfwOnMouseCallback(GLFWwindow* window, int button, int action, int mods);


class Input
{
public:
	static Input& Get()
	{
		static Input ins;
		return ins;
	}

public:
	Input()
		: _handCursor(nullptr)
		, _isPressLeft(false)
		, _isPressRight(false)
		, _isFirstMouse(true)
		, onScrollCallback(nullptr)
		, onMouseMoveCallback(nullptr)
		, onMouseClickCallback(nullptr)
	{
	}

	void Init(GLFWwindow* window)
	{
		_window = window;
		_handCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

		glfwSetScrollCallback(window, _glfwOnScrollCallback);
		glfwSetCursorPosCallback(window, _glfwCursorPosCallback);
		glfwSetMouseButtonCallback(window, _glfwOnMouseCallback);
	}

	bool IsPressLeft() { return _isPressLeft; }
	bool IsPressRight() { return _isPressRight; }

	glm::vec2 GetMousePoint() { return _lastPoint; }
	glm::vec2 GetMouseOffset() { return _offsetPoint; }

public:
	void __OnScrollCallback(float xoffset, float yoffset)
	{
		if (this->onScrollCallback) {
			this->onScrollCallback(glm::vec2(xoffset, yoffset));
		}
	}

	void __OnMouseCallback(int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			_isPressLeft = (action == GLFW_PRESS);
			_isPressRight = false;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			_isPressLeft = false;
			_isPressRight = (action == GLFW_PRESS);
			glfwSetCursor(_window, action == GLFW_PRESS ? _handCursor : nullptr);
		}

		if (this->onMouseClickCallback) {
			this->onMouseClickCallback(button, action);
		}
	}

	void __OnCursorPosCallback(float xpos, float ypos)
	{
		if (_isFirstMouse) {
			_lastPoint.x = xpos;
			_lastPoint.y = ypos;
			_isFirstMouse = false;
		}

		_offsetPoint.x = xpos - _lastPoint.x;
		_offsetPoint.y = ypos - _lastPoint.y;

		_lastPoint.x = xpos;
		_lastPoint.y = ypos;

		if (this->onMouseMoveCallback) {
			this->onMouseMoveCallback(_lastPoint);
		}
	}

public:
	std::function<void(glm::vec2)> onScrollCallback;
	std::function<void(int, int)> onMouseClickCallback;
	std::function<void(glm::vec2)> onMouseMoveCallback;

private:
	GLFWwindow* _window;
	GLFWcursor* _handCursor;

	bool _isPressLeft;
	bool _isPressRight;
	bool _isFirstMouse;

	glm::vec2 _lastPoint;
	glm::vec2 _offsetPoint;
};

void _glfwOnScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Input::Get().__OnScrollCallback(xoffset, yoffset);
}

void _glfwOnMouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	Input::Get().__OnMouseCallback(button, action, mods);
}

void _glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	Input::Get().__OnCursorPosCallback(xpos, ypos);
}
