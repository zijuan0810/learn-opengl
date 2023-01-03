#include "satan/satan.h"
#include "satan/shader.h"

#include <glm/glm.hpp> // vec2, vec3, mat4, radians
#include <glm/ext.hpp> // perspective, translate, rotate

void process_input(GLFWwindow* window);

int main(int argc, char* argv[])
{
	satan::init();

	glEnable(GL_DEPTH_TEST);

	satan::Shader shader("shader.vs.glsl", "shader.fs.glsl");
	unsigned int cubeId = satan::GetCube();
	//unsigned int squareId = satan::get_square();
	unsigned int textureId1 = satan::GetTexture("../../res/textures/container.jpg", true);
	unsigned int textureId2 = satan::GetTexture("../../res/textures/awesomeface.png", true);

	// Our state
	bool show_demo_window = false;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);

	//trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	glm::vec3 offset = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec3 view = glm::vec3(0.0f, 0.0f, -3.0f);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);


	while (!glfwWindowShouldClose(satan::window))
	{
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

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::DragFloat3("View", glm::value_ptr(view), 0.01f, -20.0f, 20.0f);

			ImGui::DragFloat3("Move", glm::value_ptr(offset), 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat3("Rotate", glm::value_ptr(rotation), 1.0f, -360.0f, 360.0f);
			ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f, -10.0f, 10.0f);

			//ImGui::ColorEdit3("Color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Color", glm::value_ptr(color)); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
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

		glm::mat4 modelMat4 = glm::identity<glm::mat4>();
		modelMat4 = glm::translate(modelMat4, offset);
		modelMat4 = glm::mat4_cast(r) * modelMat4;
		modelMat4 = glm::scale(modelMat4, scale);
		glm::mat4 viewMat4 = glm::translate(glm::identity<glm::mat4>(), view);
		glm::mat4 projMat4 = glm::perspective(glm::radians(45.0f), display_w / (float)display_h, 0.1f, 100.0f);

		shader.setMat4("u_model", modelMat4);
		shader.setMat4("u_view", viewMat4);
		shader.setMat4("u_proj", projMat4);
		shader.setFloat3("u_color", color);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureId2);

		//glBindVertexArray(squareId);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(cubeId);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(satan::window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}



void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

