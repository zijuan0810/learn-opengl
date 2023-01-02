#pragma once

#include "log.h"
#include "Mesh.h"
#include <vector>

//#pragma comment(lib, "libfbxsdk.lib")
#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")


namespace satan
{
	class ModelImporter
	{
	public:
		ModelImporter()
		{
		}

		~ModelImporter()
		{
			if (m_Manager != nullptr) m_Manager->Destroy();

			for (auto it = m_Meshs.begin(); it != m_Meshs.end(); ++it)
				delete* it;

			FBXSDK_printf("Program Success!\n");
		}

	private:
		FbxManager* m_Manager;
		FbxScene* m_Scene;
		FbxImporter* m_Importer;

	public:
		std::vector<Mesh*> m_Meshs;

	public:
		bool LoadFBX(const char* filename)
		{
			// ��ʼ��FBX SDKManager��������һ��Scene���������ɴ�FBX�н��������ж���
			if (!InitManager())
			{
				log::error("Failed to create FBX SDK manager.");
				return false;
			}

			// ����һ��Importer����������FBX�ļ�
			int lFileFormat = -1;
			m_Importer = FbxImporter::Create(m_Manager, "");
			if (!m_Manager->GetIOPluginRegistry()->DetectReaderFileFormat(filename, lFileFormat)) {
				// δ��ʶ���ļ���ʽ
				log::error("Unrecognizable file format.");
				return -1;
			}

			// ��ʼ��Importer�������ļ�·��
			if (m_Importer->Initialize(filename, lFileFormat) == false) {
				log::error("Call to FbxImporter::Initialize() failed.Error reported: {}",
					m_Importer->GetStatus().GetErrorString());
				return -1;
			}

			// ��FBX�ļ��������뵽Scene��
			if (!m_Importer->Import(m_Scene)) {
				log::error("Call to FbxImporter::Import() failed.Error reported: {}",
					m_Importer->GetStatus().GetErrorString());
			}

			// ���Scene������
			FbxStatus status;
			FbxArray<FbxString*> details;
			FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(m_Scene), &status, &details);
			bool lNotify = (!sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData) && details.GetCount() > 0) ||
				(m_Importer->GetStatus().GetCode() != FbxStatus::eSuccess);
			//���������Ϣ
			if (lNotify) {
				return LogSceneCheckError(m_Importer, details);
			}

			// ת������ϵΪ��������ϵ��
			FbxAxisSystem SceneAxisSystem = m_Scene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
			if (SceneAxisSystem != OurAxisSystem) {
				OurAxisSystem.ConvertScene(m_Scene);
			}

			// ת����Ԫ����
			FbxSystemUnit SceneSystemUnit = m_Scene->GetGlobalSettings().GetSystemUnit();
			if (SceneSystemUnit.GetScaleFactor() != 1.0) {
				// �������õ������ף���������ҲҪת��
				FbxSystemUnit::cm.ConvertScene(m_Scene);
			}

			// ת�����浽������
			FbxGeometryConverter fbxGeometryConverter(m_Manager);
			try {
				fbxGeometryConverter.Triangulate(m_Scene, true);
			}
			catch (std::runtime_error) {
				log::error("Scene integrity verification failed.\n");
				return -1;
			}

			// �ݹ�����ڵ�
			ParseNode(m_Scene->GetRootNode());
		}

	private:
		bool InitManager()
		{
			//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
			m_Manager = FbxManager::Create();
			if (m_Manager == nullptr)
			{
				FBXSDK_printf("Error: Unable to create FBX Manager!\n");
				return false;
			}
			else
				FBXSDK_printf("Autodesk FBX SDK version %s\n", m_Manager->GetVersion());

			//Create an IOSettings object. This object holds all import/export settings.
			FbxIOSettings* ios = FbxIOSettings::Create(m_Manager, IOSROOT);
			m_Manager->SetIOSettings(ios);

			//Load plugins from the executable directory (optional)
			FbxString lPath = FbxGetApplicationDirectory();
			m_Manager->LoadPluginsDirectory(lPath.Buffer());

			//Create an FBX scene. This object holds most objects imported/exported from/to files.
			m_Scene = FbxScene::Create(m_Manager, "My Scene");
			if (m_Scene == nullptr)
			{
				FBXSDK_printf("Error: Unable to create FBX scene!\n");
				return false;
			}

			return true;
		}

		int LogSceneCheckError(FbxImporter* mImporter, FbxArray<FbxString*>& details)
		{
			("\n");
			("********************************************************************************\n");
			if (details.GetCount()) {
				log::error("Scene integrity verification failed with the following errors:\n");

				for (int i = 0; i < details.GetCount(); i++)
					log::error("   %s\n", details[i]->Buffer());

				FbxArrayDelete<FbxString*>(details);
			}

			if (mImporter->GetStatus().GetCode() != FbxStatus::eSuccess) {
				log::error("\n");
				log::error("WARNING:\n");
				log::error("   The importer was able to read the file but with errors.\n");
				log::error("   Loaded scene may be incomplete.\n\n");
				log::error("   Last error message:'%s'\n", mImporter->GetStatus().GetErrorString());
			}

			("********************************************************************************\n");
			("\n");
			return -1;
		}

		/// <summary>
		/// �ݹ�����ڵ�
		/// </summary>
		void ParseNode(FbxNode* fbxNode)
		{
			// ��ȡ�ڵ�����
			FbxNodeAttribute* nodeAttribute = fbxNode->GetNodeAttribute();
			if (nodeAttribute) {
				auto attributeType = nodeAttribute->GetAttributeType();
				switch (attributeType)
				{
				case fbxsdk::FbxNodeAttribute::eMesh:
					ParseMesh(fbxNode);
					break;
				case fbxsdk::FbxNodeAttribute::eSkeleton:
					break;
				case fbxsdk::FbxNodeAttribute::eCamera:
					break;
				case fbxsdk::FbxNodeAttribute::eLight:
					break;
				default:
					break;
				}
			}

			// �ݹ�����ӽڵ�
			const int nChildCount = fbxNode->GetChildCount();
			for (int i = 0; i < nChildCount; ++i) {
				ParseNode(fbxNode->GetChild(i));
			}
		}

		void ParseMesh(FbxNode* fbxNode)
		{
			const FbxMesh* fbxMesh = fbxNode->GetMesh();
			if (fbxMesh == nullptr)
				return;

			log::info("Mesh name: %s", fbxNode->GetName());

			Mesh* mesh = new Mesh();
			mesh->Name = fbxNode->GetName();

			// ��ȡMesh����θ���������Ϸ��˵����������������
			mesh->nTriangleCount = fbxMesh->GetPolygonCount();
			// �Ƿ���UV���ݣ�
			mesh->HasUV = fbxMesh->GetElementUVCount() > 0;
			bool bAllByControlPoint = true;
			FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
			if (mesh->HasUV) {
				lUVMappingMode = fbxMesh->GetElementUV(0)->GetMappingMode();
				if (lUVMappingMode == FbxGeometryElement::eNone) {
					mesh->HasUV = false;
				}
				if (mesh->HasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint) {
					bAllByControlPoint = false;
				}
			}

			// ���ն�����������Ƕ��٣�
			// ���ֻ��һ��UV����UVӳ�䷽ʽ�ǰ�ʵ�ʶ������(FbxGeometryElement::eByControlPoint)����ô����ʵ�ʶ��������
			// ����ж���UV����ôһ�������ڲ�ͬ�Ķ��������Ӧ��ͬ��UV���꣬��UVӳ�䷽ʽ�ǰ������(eByPolygonVertex)��
			// ��ô��������Ƕ������*3.
			mesh->nVertexCount = bAllByControlPoint ? fbxMesh->GetControlPointsCount() : mesh->nTriangleCount * 3;
			// �������������ж������ꡣ
			mesh->Vertices = new float[mesh->nVertexCount * 3];
			// �����������������ݣ����鳤��=����*3.
			mesh->Indices = new unsigned int[mesh->nTriangleCount * 3];
			// ��ȡ����UV����
			FbxStringList lUVNames;
			fbxMesh->GetUVSetNames(lUVNames);
			const char* chUVName = NULL;
			if (mesh->HasUV && lUVNames.GetCount()) {
				mesh->UVs = new float[mesh->nVertexCount * 2]; // ����������UV����
				chUVName = lUVNames[0]; // ��ʱֻʹ�õ�һ��UV��
			}
			// ʵ�ʶ������ݡ�
			const FbxVector4* lControlPoints = fbxMesh->GetControlPoints();

			// �������������棬����ÿ������������㣬�����������ꡢUV�������ݡ�
			int nVertexCount = 0;
			for (int i = 0; i < mesh->nTriangleCount; ++i) {
				for (int j = 0; j < 3; ++j) { // �����棬3������
					// �������������Լ���ǰ��ĵڼ������㣬��ȡ����������
					const int lControlPointIndex = fbxMesh->GetPolygonVertex(i, j);
					if (lControlPointIndex >= 0) {
						// ��Ϊ�趨һ�������ж���UV������ÿ�������������������ڵĹ���Ķ��㣬
						// ����ʵ������ͬһ����(ControlPoint),��Ϊ�в�ͬ��UV�����Ի����㲻ͬ�Ķ��㡣
						mesh->Indices[nVertexCount] = static_cast<unsigned int>(nVertexCount);
						// ��ȡ��ǰ����������Ӧ��ʵ�ʶ��㡣
						FbxVector4 lCurrentVertex = lControlPoints[lControlPointIndex];
						// �����������FbxVector4תΪfloat����
						mesh->Vertices[nVertexCount * 3] = static_cast<float>(lCurrentVertex[0]);
						mesh->Vertices[nVertexCount * 3 + 1] = static_cast<float>(lCurrentVertex[1]);
						mesh->Vertices[nVertexCount * 3 + 2] = static_cast<float>(lCurrentVertex[2]);

						if (mesh->HasUV) {
							// ��ȡ��ǰ������ָ��UV���UV���꣬ǰ��˵����һ����������ж���UV��
							bool bUnmappedUV;
							FbxVector2 lCurrentUV;
							fbxMesh->GetPolygonVertexUV(i, j, chUVName, lCurrentUV, bUnmappedUV);
							// ��UV�����FbxVector2תΪfloat����
							mesh->UVs[nVertexCount * 2] = static_cast<float>(lCurrentUV[0]);
							mesh->UVs[nVertexCount * 2 + 1] = static_cast<float>(lCurrentUV[1]);
						}
					}
					++nVertexCount;
				}
			}

			mesh->nVertexCount = nVertexCount;
			m_Meshs.push_back(mesh);


			// ��������Mesh�ļ�����FBX�н���������䵽���档
			/*
			Engine::MeshFile mesh_file;
			// ��������Mesh�ṹ�������ļ�ͷ
			strcpy(mesh_file.head_.type_, "Mesh");
			strcpy(mesh_file.head_.name_, lNode->GetName());
			mesh_file.head_.vertex_num_ = lVertexCount;
			mesh_file.head_.vertex_index_num_ = lVertexCount;
			mesh_file.vertex_ = new Engine::Vertex[mesh_file.head_.vertex_num_];
			mesh_file.index_ = new unsigned short[mesh_file.head_.vertex_index_num_];
			// ��䶥�����ꡢcolor��UV���ꡣ
			for (int i = 0; i < lVertexCount; ++i) {
				mesh_file.vertex_[i].position_ = glm::vec3(lVertices[i * 3], lVertices[i * 3 + 1], lVertices[i * 3 + 2]);
				mesh_file.vertex_[i].color_ = glm::vec4(1.0f);
				mesh_file.vertex_[i].uv_ = glm::vec2(lUVs[i * 2], lUVs[i * 2 + 1]);
			}
			// �������
			mesh_file.index_ = lIndices;
			*/

			//// д���ļ�
			//std::filesystem::path path(src_file_path);
			//std::string src_file_name = path.filename().stem().string();
			//std::string dst_file_name = fmt::format("{}_{}.mesh", src_file_name, mesh_file.head_.name_);
			//path.replace_filename(dst_file_name);
			//mesh_file.Write(path.string());
		}
	};
}
