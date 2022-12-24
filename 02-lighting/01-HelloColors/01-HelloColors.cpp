#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "learn/Cube.h"
#include "learn/Input.hpp"
#include "learn/Shaders.h"
#include "learn/Camera.hpp"
#include "learn/Texture.hpp"
#include "learn/Helper.hpp"
using namespace glm;


void OnScrollCallback(glm::vec2 point);
void OnMouseMoveCallback(glm::vec2 point);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


int main()
{
	// glfw: initialize and configure
	Helper::Get().Init();
	GLFWwindow* window = Helper::Get().window;
	
  	Input::Get().Init(window);
  	Input::Get().onScrollCallback = OnScrollCallback;
  	Input::Get().onMouseMoveCallback = OnMouseMoveCallback;

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//OpenGL确保至少有16个包含4分量的顶点属性可用，但是有些硬件或许允许更多的顶点属性，你可以查询GL_MAX_VERTEX_ATTRIBS来获取具体的上限
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	Shader shader("res/shaders/7.1.texture.vs", "res/shaders/7.1.texture.fs");

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind
	glBindVertexArray(0); //unbind

	// texutre
	Texture tex1("res/textures/container.jpg");
	Texture tex2("res/textures/awesomeface.png");

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	shader.use();
	shader.setInt("_Texture1", 0);
	shader.setInt("_Texture2", 1);

	//定义10个立方体的位置
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f),
	};

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		//在每一帧中我们计算出新的deltaTime以备后用
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		//render test
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units
		tex1.Active(0);
		tex2.Active(1);

		//draw
		shader.use();

		glm::mat4 proj = glm::mat4(1.0f);
		proj = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("_Proj", proj);

		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("_View", view);

		glBindVertexArray(vao);

		for (int i = 0; i < 10; ++i) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(55.0f), glm::vec3(0.5f, 1.0f, 0.0));
			shader.setMat4("_Model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//cleanup
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	auto& input = Input::Get();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
		camera.ResetPosition();
	}
}

void OnScrollCallback(glm::vec2 point)
{
	camera.SetScrollBy(point, deltaTime);
}

void OnMouseMoveCallback(glm::vec2 point)
{
	auto& input = Input::Get();
	vec2 offset = input.GetMouseOffset();
	if (input.IsPressLeft()) {
		camera.SetMoveBy(offset, deltaTime);
	}
	else if (input.IsPressRight()) {
		camera.YawAndPitch(offset);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
