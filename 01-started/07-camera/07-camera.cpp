#include "satan/Satan.h"

#pragma warning (disable: 4244)


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0;
float fov = 45.0f;
float lastX = 400;
float lastY = 300;

unsigned int textureId1;
unsigned int textureId2;

void process_input(GLFWwindow* window);
void drop_callback(GLFWwindow* window, int count, const char** paths);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


int main(int argc, char* argv[])
{
	satan::init();

	glfwSetDropCallback(satan::window, drop_callback);
	glfwSetCursorPosCallback(satan::window, mouse_callback);
	glfwSetScrollCallback(satan::window, scroll_callback);

	glEnable(GL_DEPTH_TEST);

	satan::Shader shader("shader.vs.glsl", "shader.fs.glsl");
	unsigned int cubeId = satan::GetCube();
	//unsigned int squareId = satan::get_square();
	textureId1 = satan::GetTexture("../../res/textures/container.jpg", true);
	textureId2 = satan::GetTexture("../../res/textures/awesomeface.png", true);

	// Our state
	bool show_demo_window = false;
	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);

	//trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	glm::vec3 offset = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec3 eye = glm::vec3(0.0f, 0.0f, -3.0f);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	// world space positions of our cubes
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
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	//auto modelImporter = new satan::ModelImporter();
	//modelImporter->LoadFBX("../../res/models/Terraformer_Basic.fbx");

	while (!glfwWindowShouldClose(satan::window))
	{
		float curTime = glfwGetTime();
		deltaTime = curTime - lastFrame;
		lastFrame = curTime;

		process_input(satan::window);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Tools");                          // Create a window called "Hello, world!" and append into it.

			ImGui::DragFloat3("Eye", glm::value_ptr(eye), 0.01f, -20.0f, 20.0f);

			ImGui::DragFloat3("Move", glm::value_ptr(offset), 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat3("Rotate", glm::value_ptr(rotation), 1.0f, -360.0f, 360.0f);
			ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f, -10.0f, 10.0f);

			//ImGui::ColorEdit3("Color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Color", glm::value_ptr(color)); // Edit 3 floats representing a color

			if (ImGui::Button("Load FBX"))
			{
			}

			ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(satan::window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		shader.Use();
		shader.setFloat("glfw_time", glfwGetTime());
		shader.SetInt("texture1", 0);
		shader.SetInt("texture2", 1);

		glm::quat r = glm::identity<glm::quat>();
		r *= glm::angleAxis(glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0f)); //x
		r *= glm::angleAxis(glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0f)); //y
		r *= glm::angleAxis(glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0f)); //z

		//glm::mat4 modelMat4 = glm::identity<glm::mat4>();
		//modelMat4 = glm::translate(modelMat4, offset);
		//modelMat4 = glm::mat4_cast(r) * modelMat4;
		//modelMat4 = glm::scale(modelMat4, scale);

		float radius = 10.0f;
		float cameraX = sin(glfwGetTime()) * radius;
		float cameraZ = cos(glfwGetTime()) * radius;
		//glm::mat4 viewMat4 = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//glm::mat4 viewMat4 = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//glm::mat4 viewMat4 = glm::lookAt(glm::vec3(cameraX, 0.0, cameraZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 viewMat4 = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glm::mat4 projMat4 = glm::perspective(glm::radians(fov), display_w / (float)display_h, 0.1f, 100.0f);

		//shader.setMat4("u_model", modelMat4);
		shader.setMat4("u_view", viewMat4);
		shader.setMat4("u_proj", projMat4);
		shader.setFloat3("u_color", color);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureId2);

		glBindVertexArray(cubeId);
		for (int i = 0; i < 10; i++)
		{
			float angle = 20.0f * i;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.setMat4("u_model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		//glBindVertexArray(squareId);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		//glBindVertexArray(cubeId);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(satan::window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void process_input(GLFWwindow* window)
{
	if (ImGui::IsAnyItemHovered())
		return;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W))
		cameraPos += cameraFront * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_S))
		cameraPos -= cameraFront * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_A))
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D))
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent((float)xpos, (float)ypos);
	if (io.WantCaptureMouse)
		return;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent((float)xoffset, (float)yoffset);
	if (io.WantCaptureMouse)
		return;

	if (fov >= 1.0f && fov <= 45.0f) fov -= yoffset;
	if (fov <= 1.0f) fov = 1.0f;
	if (fov >= 45.0f) fov = 45.0f;
}

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	for (int i = 0; i < count; i++)
	{
		satan::Log::info(paths[i]);
		auto extension = std::filesystem::path(paths[i]).extension().string();
		if (extension == ".png" || extension == ".jpg") {
			if (i == 0)
				textureId1 = satan::GetTexture(paths[i], true);
			else if (i == 1)
				textureId2 = satan::GetTexture(paths[i], true);
		}
	}
}
