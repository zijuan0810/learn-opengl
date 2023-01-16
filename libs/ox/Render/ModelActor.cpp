#include "ModelActor.h"

#pragma comment(lib, "assimp-vc143-mtd.lib")

namespace ox
{
	ModelActor::ModelActor(bool gama)
		: m_IsGamma(gama)
	{
		m_Shader->m_FragPath = "model.vert";
		m_Shader->m_VertPath = "model.frag";
		m_Shader->Load();
	}

	void ModelActor::Draw(const glm::mat4& view, const glm::mat4& proj)
	{
		Actor::Draw(view, proj);

		glm::mat4 model = GetLocalToWorldMatrix();
		m_Shader->SetMatrix4("u_model", model);
		m_Shader->SetMatrix4("u_view", view);
		m_Shader->SetMatrix4("u_proj", proj);

		for (int i = 0; i < m_Meshes.size(); ++i)
			m_Meshes[i].Draw(m_Shader);
	}

	void ModelActor::Load(const std::string& filepath)
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

	void ModelActor::cleanUp()
	{
		m_Meshes.clear();
		m_TextureMap.clear();
	}

	void ModelActor::processNode(aiNode* node, const aiScene* scene)
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

	Mesh ModelActor::processMesh(aiMesh* mesh, const aiScene* scene)
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

	std::vector<Texture2D> ModelActor::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
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
				texture2D.ID = Texture2D::LoadTexture(m_Folder + '/' + path.C_Str());
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
}
