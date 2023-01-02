#pragma once

#include <string>


namespace satan
{
	class Mesh
	{
	public:
		Mesh()
			: Vertices(nullptr)
			, Indices(nullptr)
			, UVs(nullptr)
			, nTriangleCount(0)
			, nVertexCount(0)
			, HasUV(false)
			, VAO(-1)
		{
		}

		~Mesh()
		{
			if (Vertices != nullptr) { delete Vertices; Vertices = nullptr; }
			if (Indices != nullptr) { delete Indices; Indices = nullptr; }
			if (UVs != nullptr) { delete UVs; UVs = nullptr; }
		}

	public:
		void InitVAO()
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nVertexCount * 3, Vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nTriangleCount * 3, Indices, GL_STATIC_DRAW);

			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			//// color attribute
			//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			//glEnableVertexAttribArray(1);
			//// texture coord attribute
			//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			//glEnableVertexAttribArray(2);
		}

		void Draw()
		{
			if (VAO <= 0)
				InitVAO();

			glBindVertexArray(VAO);
			//使用顶点索引进行绘制，最后的0表示数据偏移量。
			glDrawElements(GL_TRIANGLES, nTriangleCount * 3, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

	public:
		unsigned int VBO;
		unsigned int VAO;
		unsigned int EBO;

	public:
		std::string Name;

		int nTriangleCount;
		int nVertexCount;
		float* Vertices;
		unsigned int* Indices;

		bool HasUV;
		float* UVs;
	};
}
