#include "MeshUtil.h"
#include "Core/String/String.h"
#include "Core/macro.h"

#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Engine {

	uint32_t GetPrimitiveCount(const tinygltf::Model& model, const tinygltf::Node& node) {
		uint32_t count = 0;
		if (node.mesh > -1) {
			count = model.meshes[node.mesh].primitives.size();
		}
		if (!node.children.empty()) {
			for (uint32_t i = 0; i < node.children.size(); i++) {
				count += GetPrimitiveCount(model, model.nodes[node.children[i]]);
			}
		}
		return count;
	}

	void LoadGLTFNode(const tinygltf::Model& model, const tinygltf::Node& node, TVector<SPrimitiveData>& primitives, uint32_t& index) {
		if (node.mesh > -1) {
			const tinygltf::Mesh& mesh = model.meshes[node.mesh];
			for (uint32_t i = 0; i < mesh.primitives.size(); i++) {
				const tinygltf::Primitive& primitive = mesh.primitives[i];

				//vertices
				auto kvPos = primitive.attributes.find("POSITION");
				if (kvPos == primitive.attributes.end()) {
					continue;
				}
				const tinygltf::Accessor& posAccessor = model.accessors[kvPos->second];
				const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
				const float* bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
				uint32_t vertexCount = static_cast<uint32_t>(posAccessor.count);
				int posByteStride = posAccessor.ByteStride(posView) / sizeof(float);

				// normal
				auto kvNormal = primitive.attributes.find("NORMAL");
				const float* bufferNormals = nullptr;
				int normByteStride;
				if (kvNormal != primitive.attributes.end()) {
					const tinygltf::Accessor& normAccessor = model.accessors[kvNormal->second];
					const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
					normByteStride = normAccessor.ByteStride(normView) / sizeof(float);
				}

				// texcoord
				auto kvTexcoord0 = primitive.attributes.find("TEXCOORD_0");
				const float* bufferTexCoordSet0 = nullptr;
				int uv0ByteStride;
				if (kvTexcoord0 != primitive.attributes.end()) {
					const tinygltf::Accessor& uvAccessor = model.accessors[kvTexcoord0->second];
					const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoordSet0 = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
					uv0ByteStride = uvAccessor.ByteStride(uvView) / sizeof(float);
				}

				TVector<Vertex>& vertices = primitives[index].vertices;
				vertices.resize(vertexCount);

				for (uint32_t v = 0; v < vertexCount; v++) {
					vertices[v].position.x = bufferPos[v * posByteStride];
					vertices[v].position.y = bufferPos[v * posByteStride + 1];
					vertices[v].position.z = bufferPos[v * posByteStride + 2];
					vertices[v].normal.x = bufferNormals[v * normByteStride];
					vertices[v].normal.y = bufferNormals[v * normByteStride + 1];
					vertices[v].normal.z = bufferNormals[v * normByteStride + 2];
					vertices[v].uv.x = bufferTexCoordSet0[v * uv0ByteStride];
					vertices[v].uv.y = bufferTexCoordSet0[v * uv0ByteStride + 1];
				}

				//indices
				if (primitive.indices > -1) {
					TVector<uint32_t>& indices = primitives[index].indices;
					const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
					const tinygltf::BufferView& indexView = model.bufferViews[indexAccessor.bufferView];
					const tinygltf::Buffer& bufferIndex = model.buffers[indexView.buffer];
					indices.resize(indexAccessor.count);
					const void* dataPtr = &(bufferIndex.data[indexAccessor.byteOffset + indexView.byteOffset]);

					if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
						const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
						for (size_t j = 0; j < indexAccessor.count; j++) {
							indices[j] = buf[j];
						}
					}
					else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
						const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
						for (size_t j = 0; j < indexAccessor.count; j++) {
							indices[j] = buf[j];
						}
					}
					else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE) {
						const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);
						for (size_t j = 0; j < indexAccessor.count; j++) {
							indices[j] = buf[j];
						}
					}
				}

				// TODO textures
				//if (primitive.material > -1) {
				//	TVector<std::string>& textureNames = primitives[index].textures;
				//	const tinygltf::Material& mat = model.materials[primitive.material];
				//	const tinygltf::Texture& tex = model.textures[mat.pbrMetallicRoughness.baseColorTexture.index];
				//	const tinygltf::Image& img = model.images[tex.source];
				//	textureNames.resize(1);
				//	size_t typeIdx = img.mimeType.find('/');
				//	const char* imgType = &img.mimeType[typeIdx + 1];
				//	std::string imageFile = img.name + '.' + imgType;
				//	textureNames[0] = "textures\\";
				//	textureNames[0].append(imageFile);
				//}
				++index;
				auto& info = primitives[index - 1];
				auto& verts = info.vertices;
			}
		}

		if (node.children.size() > 0) {
			for (uint32_t i = 0; i < node.children.size(); i++) {
				LoadGLTFNode(model, model.nodes[node.children[i]], primitives, index);
			}
		}
	}

	// TODO run in another thread
	bool LoadMeshFromGLTF(const char* file, Mesh* mesh)
	{
		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF gltfContext;
		FString error;
		FString warning;
		if(!gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, file)) {
			LOG("Failed to load GLTF mesh: %s", file);
			return false;
		}
		const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

		uint32_t primitiveCount = 0;
		for(auto& node: scene.nodes) {
			primitiveCount += GetPrimitiveCount(gltfModel, gltfModel.nodes[node]);
		}
		TVector<SPrimitiveData> primitives(primitiveCount);
		primitiveCount = 0;
		for (uint32_t i = 0; i < scene.nodes.size(); i++) {
			LoadGLTFNode(gltfModel, gltfModel.nodes[scene.nodes[i]], primitives, primitiveCount);
		}
		mesh->LoadPrimitives(primitives);
		return true;
	}

	uint32_t GetPrimitiveCount(const aiScene* aScene, aiNode* aNode) {
		uint32_t count = aNode->mNumMeshes;
		for (uint32_t i = 0; i < aNode->mNumChildren; i++) {
			count += GetPrimitiveCount(aScene, aNode->mChildren[i]);
		}
		return count;
	}

	void LoadFbxNode(const aiScene* aScene, aiNode* aNode, TVector<SPrimitiveData>& meshInfos) {
		for (uint32_t i = 0; i < aNode->mNumMeshes; i++) {
			aiMesh* aMesh = aScene->mMeshes[aNode->mMeshes[i]];
			TVector<Vertex>& vertices = meshInfos[i].vertices;
			TVector<uint32_t>& indices = meshInfos[i].indices;
			//TVector<std::string>& textures = meshInfos[i].textures;

			// vertices
			vertices.resize(aMesh->mNumVertices);
			for (uint32_t i = 0; i < aMesh->mNumVertices; i++) {
				vertices[i].position.x = aMesh->mVertices[i].x;
				vertices[i].position.y = aMesh->mVertices[i].y;
				vertices[i].position.z = aMesh->mVertices[i].z;

				vertices[i].normal.x = aMesh->mNormals[i].x;
				vertices[i].normal.y = aMesh->mNormals[i].y;
				vertices[i].normal.z = aMesh->mNormals[i].z;
				if (aMesh->mTextureCoords[0]) {
					vertices[i].uv.x = aMesh->mTextureCoords[0][i].x;
					vertices[i].uv.y = aMesh->mTextureCoords[0][i].y;
				}
			}

			// indices
			uint32_t count = 0;
			for (uint32_t i = 0; i < aMesh->mNumFaces; i++) {
				aiFace aFace = aMesh->mFaces[i];
				for (uint32_t j = 0; j < aFace.mNumIndices; j++) {
					++count;
				}
			}
			indices.resize(count);
			count = 0;
			for (uint32_t i = 0; i < aMesh->mNumFaces; i++) {
				aiFace aFace = aMesh->mFaces[i];
				for (uint32_t j = 0; j < aFace.mNumIndices; j++) {
					indices[count++] = aFace.mIndices[j];
				}
			}

			// textures TODO
			//if (aMesh->mMaterialIndex >= 0) {
			//	// ֻ��ȡ��Ҫ������
			//	aiMaterial* aMat = aScene->mMaterials[aMesh->mMaterialIndex];
			//	uint32_t idx = 0;
			//	aiTextureType types[] = { aiTextureType_DIFFUSE, aiTextureType_NORMALS, aiTextureType_SPECULAR };
			//	for (uint32_t j = 0; j < 3; j++) {
			//		for (uint32_t i = 0; i < aMat->GetTextureCount(types[j]); i++) { ++idx; }
			//	}
			//	if (idx > 0) {
			//		textures.resize(idx);
			//		idx = 0;
			//		for (uint32_t j = 0; j < 3; j++) {
			//			for (uint32_t i = 0; i < aMat->GetTextureCount(types[j]); i++) {
			//				aiString str;
			//				aMat->GetTexture(types[j], i, &str);
			//				textures[idx++] = str.C_Str();
			//			}
			//		}
			//	}
			//}
		}

		for (uint32_t i = 0; i < aNode->mNumChildren; i++) {
			LoadFbxNode(aScene, aNode->mChildren[i], meshInfos);
		}
	}

	bool LoadMeshFromFBX(const char* file, Mesh* mesh) {

		Assimp::Importer importer;
		const aiScene* aScene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!aScene || aScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aScene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return false;
		}
		// �Ȼ�ȡ������
		uint32_t primitiveCount = GetPrimitiveCount(aScene, aScene->mRootNode);
		TVector<SPrimitiveData> primitives(primitiveCount);
		LoadFbxNode(aScene, aScene->mRootNode, primitives);
		return true;
	}
}