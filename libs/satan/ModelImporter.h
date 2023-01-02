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
			// 初始化FBX SDKManager，并创建一个Scene，用来容纳从FBX中解析的所有对象
			if (!InitManager())
			{
				log::error("Failed to create FBX SDK manager.");
				return false;
			}

			// 创建一个Importer，用来解析FBX文件
			int lFileFormat = -1;
			m_Importer = FbxImporter::Create(m_Manager, "");
			if (!m_Manager->GetIOPluginRegistry()->DetectReaderFileFormat(filename, lFileFormat)) {
				// 未能识别文件格式
				log::error("Unrecognizable file format.");
				return -1;
			}

			// 初始化Importer，设置文件路径
			if (m_Importer->Initialize(filename, lFileFormat) == false) {
				log::error("Call to FbxImporter::Initialize() failed.Error reported: {}",
					m_Importer->GetStatus().GetErrorString());
				return -1;
			}

			// 将FBX文件解析导入到Scene中
			if (!m_Importer->Import(m_Scene)) {
				log::error("Call to FbxImporter::Import() failed.Error reported: {}",
					m_Importer->GetStatus().GetErrorString());
			}

			// 检查Scene完整性
			FbxStatus status;
			FbxArray<FbxString*> details;
			FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(m_Scene), &status, &details);
			bool lNotify = (!sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData) && details.GetCount() > 0) ||
				(m_Importer->GetStatus().GetCode() != FbxStatus::eSuccess);
			//输出错误信息
			if (lNotify) {
				return LogSceneCheckError(m_Importer, details);
			}

			// 转换坐标系为右手坐标系。
			FbxAxisSystem SceneAxisSystem = m_Scene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
			if (SceneAxisSystem != OurAxisSystem) {
				OurAxisSystem.ConvertScene(m_Scene);
			}

			// 转换单元长度
			FbxSystemUnit SceneSystemUnit = m_Scene->GetGlobalSettings().GetSystemUnit();
			if (SceneSystemUnit.GetScaleFactor() != 1.0) {
				// 例子中用的是厘米，所以这里也要转换
				FbxSystemUnit::cm.ConvertScene(m_Scene);
			}

			// 转换曲面到三角形
			FbxGeometryConverter fbxGeometryConverter(m_Manager);
			try {
				fbxGeometryConverter.Triangulate(m_Scene, true);
			}
			catch (std::runtime_error) {
				log::error("Scene integrity verification failed.\n");
				return -1;
			}

			// 递归解析节点
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
		/// 递归解析节点
		/// </summary>
		void ParseNode(FbxNode* fbxNode)
		{
			// 获取节点属性
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

			// 递归解析子节点
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

			// 获取Mesh多边形个数，对游戏来说就是三角形面数。
			mesh->nTriangleCount = fbxMesh->GetPolygonCount();
			// 是否有UV数据？
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

			// 最终顶点个数到底是多少？
			// 如果只有一套UV，即UV映射方式是按实际顶点个数(FbxGeometryElement::eByControlPoint)，那么就是实际顶点个数。
			// 如果有多套UV，那么一个顶点在不同的多边形里，会对应不同的UV坐标，即UV映射方式是按多边形(eByPolygonVertex)，
			// 那么顶点个数是多边形数*3.
			mesh->nVertexCount = bAllByControlPoint ? fbxMesh->GetControlPointsCount() : mesh->nTriangleCount * 3;
			// 创建数组存放所有顶点坐标。
			mesh->Vertices = new float[mesh->nVertexCount * 3];
			// 创建数组存放索引数据，数组长度=面数*3.
			mesh->Indices = new unsigned int[mesh->nTriangleCount * 3];
			// 获取多套UV名字
			FbxStringList lUVNames;
			fbxMesh->GetUVSetNames(lUVNames);
			const char* chUVName = NULL;
			if (mesh->HasUV && lUVNames.GetCount()) {
				mesh->UVs = new float[mesh->nVertexCount * 2]; // 创建数组存放UV数据
				chUVName = lUVNames[0]; // 暂时只使用第一套UV。
			}
			// 实际顶点数据。
			const FbxVector4* lControlPoints = fbxMesh->GetControlPoints();

			// 遍历所有三角面，遍历每个面的三个顶点，解析顶点坐标、UV坐标数据。
			int nVertexCount = 0;
			for (int i = 0; i < mesh->nTriangleCount; ++i) {
				for (int j = 0; j < 3; ++j) { // 三角面，3个顶点
					// 传入面索引，以及当前面的第几个顶点，获取顶点索引。
					const int lControlPointIndex = fbxMesh->GetPolygonVertex(i, j);
					if (lControlPointIndex >= 0) {
						// 因为设定一个顶点有多套UV，所以每个三角面与其他面相邻的共享的顶点，
						// 尽管实际上是同一个点(ControlPoint),因为有不同的UV，所以还是算不同的顶点。
						mesh->Indices[nVertexCount] = static_cast<unsigned int>(nVertexCount);
						// 获取当前顶点索引对应的实际顶点。
						FbxVector4 lCurrentVertex = lControlPoints[lControlPointIndex];
						// 将顶点坐标从FbxVector4转为float数组
						mesh->Vertices[nVertexCount * 3] = static_cast<float>(lCurrentVertex[0]);
						mesh->Vertices[nVertexCount * 3 + 1] = static_cast<float>(lCurrentVertex[1]);
						mesh->Vertices[nVertexCount * 3 + 2] = static_cast<float>(lCurrentVertex[2]);

						if (mesh->HasUV) {
							// 获取当前顶点在指定UV层的UV坐标，前面说过，一个顶点可能有多套UV。
							bool bUnmappedUV;
							FbxVector2 lCurrentUV;
							fbxMesh->GetPolygonVertexUV(i, j, chUVName, lCurrentUV, bUnmappedUV);
							// 将UV坐标从FbxVector2转为float数组
							mesh->UVs[nVertexCount * 2] = static_cast<float>(lCurrentUV[0]);
							mesh->UVs[nVertexCount * 2 + 1] = static_cast<float>(lCurrentUV[1]);
						}
					}
					++nVertexCount;
				}
			}

			mesh->nVertexCount = nVertexCount;
			m_Meshs.push_back(mesh);


			// 创建引擎Mesh文件，从FBX中解析数据填充到里面。
			/*
			Engine::MeshFile mesh_file;
			// 构造引擎Mesh结构，设置文件头
			strcpy(mesh_file.head_.type_, "Mesh");
			strcpy(mesh_file.head_.name_, lNode->GetName());
			mesh_file.head_.vertex_num_ = lVertexCount;
			mesh_file.head_.vertex_index_num_ = lVertexCount;
			mesh_file.vertex_ = new Engine::Vertex[mesh_file.head_.vertex_num_];
			mesh_file.index_ = new unsigned short[mesh_file.head_.vertex_index_num_];
			// 填充顶点坐标、color、UV坐标。
			for (int i = 0; i < lVertexCount; ++i) {
				mesh_file.vertex_[i].position_ = glm::vec3(lVertices[i * 3], lVertices[i * 3 + 1], lVertices[i * 3 + 2]);
				mesh_file.vertex_[i].color_ = glm::vec4(1.0f);
				mesh_file.vertex_[i].uv_ = glm::vec2(lUVs[i * 2], lUVs[i * 2 + 1]);
			}
			// 填充索引
			mesh_file.index_ = lIndices;
			*/

			//// 写入文件
			//std::filesystem::path path(src_file_path);
			//std::string src_file_name = path.filename().stem().string();
			//std::string dst_file_name = fmt::format("{}_{}.mesh", src_file_name, mesh_file.head_.name_);
			//path.replace_filename(dst_file_name);
			//mesh_file.Write(path.string());
		}
	};
}
