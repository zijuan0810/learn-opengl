#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Shader.h"
#include "Texture2D.h"

#define MAX_BONE_INFLUENCE 4


namespace ox
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 UV;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;

		//bone indexes which will influence this vertex
		int BoneIDs[MAX_BONE_INFLUENCE];
		//weights from each bone
		float Weights[MAX_BONE_INFLUENCE];
	};

	

	class Mesh
	{
	public:
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		std::vector<Texture2D> Texture2Ds;

		unsigned int VAO, VBO, EBO;

	public:
		Mesh(const std::vector<Vertex>& vertices,
			const std::vector<unsigned int>& indices,
			const std::vector<Texture2D>& textures);

		void Draw(Shader* shader);

	private:
		void setupMesh();
	};
}
