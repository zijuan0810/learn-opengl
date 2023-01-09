#pragma once

#include "ox/Core/Scene.h"
#include "ox/Render/ModelActor.h"

class MyScene : public ox::Scene
{
public:
	virtual bool OnInit() 
	{
		m_ModelActor = new ox::ModelActor();
		m_ModelActor->Load("../../res/models/cube.fbx");
		AddChild(m_ModelActor);
	}

	virtual void OnGUI()
	{
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Tools");                          // Create a window called "Hello, world!" and append into it.

			//ImGui::DragFloat3("Move", glm::value_ptr(cameraPosition), 0.01f, -1000.0f, 1000.0f);
			//ImGui::DragFloat3("Rotate", glm::value_ptr(rotation), 1.0f, -360.0f, 360.0f);
			//ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f, -10.0f, 10.0f);

			//ImGui::ColorEdit3("Color", (float*)&clear_color); // Edit 3 floats representing a color
			//ImGui::ColorEdit3("Color", glm::value_ptr(color)); // Edit 3 floats representing a color

			ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
	}

private:
	ox::ModelActor* m_ModelActor;
	bool show_demo_window = false;
	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
};
