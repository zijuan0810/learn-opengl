#pragma once

#include <vector>
#include <filesystem>
#include <unordered_map>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Log.h"
#include "Mesh.h"
#include "Shader.h"

#pragma comment(lib, "assimp-vc143-mtd.lib")


namespace satan
{
	class Model
	{
	public:
		Model(bool gama = false)
			: m_IsGamma(gama)
		{
		}

		void Draw(Shader& shader)
		{
			for (int i = 0; i < m_Meshes.size(); ++i)
				m_Meshes[i].Draw(shader);
		}

		unsigned int TextureFromFile(const char* path)
		{
			std::string filename(path);
			filename = m_Folder + '/' + filename;

			unsigned int textureID;
			glGenTextures(1, &textureID);

			int width, height, nrComponents;
			unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
			if (data != nullptr)
			{
				GLenum format;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 3)
					format = GL_RGB;
				else if (nrComponents == 4)
					format = GL_RGBA;

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}
			else
			{
				Log::Error("Texture failed to load at path: %s", path);
				stbi_image_free(data);
			}

			return textureID;
		}

		void Load(const std::string& filepath)
		{
			cleanUp();

			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals
				| aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
			if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
			{
				Log::Error("ASSIMP:: %s", importer.GetErrorString());
				return;
			}

			auto path = std::filesystem::path(filepath);
			m_Folder = path.parent_path().string();

			processNode(scene->mRootNode, scene);
		}

	private:
		void cleanUp()
		{
			m_Meshes.clear();
			m_TextureMap.clear();
		}

		void processNode(aiNode* node, const aiScene* scene)
		{
			//处理节点所有的网格（如果有的话）
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				m_Meshes.push_back(processMesh(mesh, scene));
			}

			//接下来对它的子节点重复这一过程
			for (unsigned int i = 0; i < node->mNumChildren; i++)
				processNode(node->mChildren[i], scene);
		}

		Mesh processMesh(aiMesh* mesh, const aiScene* scene)
		{
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<Texture2D> textures;

			//walk through each of the mesh's vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				//position
				vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				//normals
				if (mesh->HasNormals())
					vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				//UV
				if (mesh->mTextureCoords[0] != nullptr)
				{
					// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
					// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
					vertex.UV = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
					//tangent
					vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
					//bitangent
					vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
				}
				else
					vertex.UV = glm::vec2(0.0f, 0.0f);

				vertices.push_back(vertex);
			}

			//now wak through each of the mesh's faces (a face is a mesh its triangle) 
			//and retrieve the corresponding vertex indices.
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			//process materials
			// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:
			// diffuse: texture_diffuseN
			// specular: texture_specularN
			// normal: texture_normalN
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// 1. diffuse maps
			std::vector<Texture2D> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			std::vector<Texture2D> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<Texture2D> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<Texture2D> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			return Mesh(vertices, indices, textures);
		}

		std::vector<Texture2D> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
		{
			std::vector<Texture2D> textures;
			unsigned int textureCount = material->GetTextureCount(type);
			for (unsigned int i = 0; i < textureCount; i++)
			{
				aiString path;
				material->GetTexture(type, i, &path);

				auto it = m_TextureMap.find(path.C_Str());
				if (it == m_TextureMap.end())
				{
					Texture2D texture2D;
					texture2D.ID = TextureFromFile(path.C_Str());
					texture2D.Type = typeName;
					texture2D.filename = path.C_Str();
					textures.push_back(texture2D);

					m_TextureMap.insert(std::pair<std::string, Texture2D>(texture2D.filename, texture2D));
				}
				else
					textures.push_back(it->second);
			}

			return textures;
		}

	private:
		bool m_IsGamma;
		std::string m_Folder;
		std::vector<Mesh> m_Meshes;
		std::unordered_map<std::string, Texture2D> m_TextureMap;
	};
}

