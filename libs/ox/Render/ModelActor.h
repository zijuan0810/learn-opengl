#pragma once

#include <vector>
#include <filesystem>
#include <unordered_map>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "../Core/Actor.h"
#include "../Core/Mesh.h"


namespace ox
{
	class ModelActor : public Actor
	{
	public:
		ModelActor(bool gama = false);

	public:
		void Load(const std::string& filepath);

	public:
		virtual void Draw(const glm::mat4& view, const glm::mat4& proj);

	private:
		void cleanUp();
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture2D> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

	private:
		bool m_IsGamma;
		std::string m_Folder;
		std::vector<Mesh> m_Meshes;
		std::unordered_map<std::string, Texture2D> m_TextureMap;
	};
}
