#include "satan/Satan.h"


float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

glm::vec3 cameraPosition(0.0f, 0.0f, 60.0f);

satan::Model* model;
satan::Camera camera(cameraPosition);
glm::vec3 rotation = glm::vec3(0.0);
bool firstMouse = true;
float lastX = 400;
float lastY = 300;

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

	//satan::Shader shader("shader.vert", "shader.frag");
	satan::Shader shaderNoTex("shader-no-tex.vert", "shader-no-tex.frag");
	model = new satan::Model(&shaderNoTex);
	//model->Load("../../res/objects/backpack/backpack.obj");
	model->Load("../../res/models/cube.fbx");

	// Our state
	bool show_demo_window = false;
	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);

	//glm::vec3 move = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

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

			ImGui::DragFloat3("Move", glm::value_ptr(cameraPosition), 0.01f, -1000.0f, 1000.0f);
			ImGui::DragFloat3("Rotate", glm::value_ptr(rotation), 1.0f, -360.0f, 360.0f);
			ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f, -10.0f, 10.0f);

			//ImGui::ColorEdit3("Color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Color", glm::value_ptr(color)); // Edit 3 floats representing a color

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


		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)satan::SCR_WIDTH / (float)satan::SCR_HEIGHT, 0.1f, 100.0f);
		glm::quat rorateQuat = glm::identity<glm::quat>();
		rorateQuat *= glm::angleAxis(glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0f)); //x
		rorateQuat *= glm::angleAxis(glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0f)); //y
		rorateQuat *= glm::angleAxis(glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0f)); //z
		glm::mat4 modelMat4 = glm::mat4(1.0f);
		//modelMat4 = glm::translate(modelMat4, move);
		modelMat4 = glm::mat4_cast(rorateQuat) * modelMat4;
		modelMat4 = glm::scale(modelMat4, scale);
		model->m_Color = color;
		model->Draw(modelMat4, camera.GetViewMatrix(), projection);

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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(satan::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(satan::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(satan::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(satan::RIGHT, deltaTime);
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

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera.ProcessMouseMovement(xoffset, yoffset);
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		rotation.x -= yoffset * 2.0f;
		rotation.y += xoffset * 2.0f;
	}

	lastX = xpos;
	lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent((float)xoffset, (float)yoffset);
	if (io.WantCaptureMouse)
		return;

	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	for (int i = 0; i < count; i++)
	{
		satan::Log::info(paths[i]);
		auto extension = std::filesystem::path(paths[i]).extension().string();
		model->Load(paths[i]);
		/*if (extension == ".png" || extension == ".jpg") {
			if (i == 0)
				textureId1 = satan::GetTexture(paths[i], true);
			else if (i == 1)
				textureId2 = satan::GetTexture(paths[i], true);
		}*/
	}
}
