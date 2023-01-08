#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Shader.h"

#define MAX_BONE_INFLUENCE 4


namespace satan
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

	struct Texture2D
	{
		unsigned int ID;
		std::string Type;
		std::string filename;
	};

	class Mesh
	{
	public:
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		std::vector<Texture2D> Texture2Ds;

	public:
		Mesh(const std::vector<Vertex>& vertices,
			const std::vector<unsigned int>& indices,
			const std::vector<Texture2D>& textures)
		{
			Vertices = vertices;
			Indices = indices;
			Texture2Ds = textures;

			setupMesh();
		}

		void Draw(Shader* shader)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
			for (unsigned int i = 0; i < Texture2Ds.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);

				std::string number;
				std::string name = Texture2Ds[i].Type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++); // transfer unsigned int to string
				else if (name == "texture_height")
					number = std::to_string(heightNr++);
				shader->SetInt(("material." + name + number).c_str(), i);
				glBindTexture(GL_TEXTURE_2D, Texture2Ds[i].ID);
			}

			//绘制网格
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, (GLsizei)Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// always good practice to set everything back to defaults once configured.
			glActiveTexture(GL_TEXTURE0);
		}

	private:
		void setupMesh()
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

			//顶点位置
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			//顶点法线
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			//纹理坐标
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));
			//tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			//bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
			//ids
			glEnableVertexAttribArray(5);
			glVertexAttribIPointer(5, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIDs));
			//weights
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));

			glBindVertexArray(0);
		}

	private:
		unsigned int VAO, VBO, EBO;
	};
}
