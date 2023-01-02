#include "satan/satan.h"
#include "satan/shader.h"

#include <glm/glm.hpp> // vec2, vec3, mat4, radians
#include <glm/ext.hpp> // perspective, translate, rotate

void process_input(GLFWwindow* window);

int main(int argc, char* argv[])
{
	satan::init();

	satan::shader shader("shader.vs.glsl", "shader.fs.glsl");
	unsigned int squareId = satan::get_square();
	unsigned int textureId1 = satan::GetTexture("../../res/textures/container.jpg", true);
	unsigned int textureId2 = satan::GetTexture("../../res/textures/grass.png", true);

	// Our state
	bool show_demo_window = false;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);

	//trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	glm::vec3 offset = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0);
	glm::vec3 scale = glm::vec3(1.0f);


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

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			ImGui::DragFloat3("Trans", glm::value_ptr(offset), 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat3("Rotate", glm::value_ptr(rotation), 1.0f, -360.0f, 360.0f);
			ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f, -10.0f, 10.0f);

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
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
		glClear(GL_COLOR_BUFFER_BIT);


		shader.use();
		shader.setFloat("glfw_time", glfwGetTime());
		shader.setInt("texture1", 0);
		shader.setInt("texture2", 1);

		glm::mat4 trans = glm::identity<glm::mat4>();
		trans = glm::translate(trans, offset);
		//trans = glm::rotate(trans, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0)); //x
		//trans = glm::rotate(trans, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0)); //y
		//trans = glm::rotate(trans, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0)); //z

		glm::quat r = glm::identity<glm::quat>();
		r *= glm::angleAxis(glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0f)); //x
		r *= glm::angleAxis(glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0f)); //y
		r *= glm::angleAxis(glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0f)); //z
		trans = glm::mat4_cast(r) * trans;

		trans = glm::scale(trans, scale);
		shader.setMat4("u_Transform", trans);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureId2);

		glBindVertexArray(squareId);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

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

