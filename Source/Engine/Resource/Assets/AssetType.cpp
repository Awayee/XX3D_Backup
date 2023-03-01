#include "AssetType.h"
#include "MeshUtil/MeshUtil.h"
#include "Core/Json/Json.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define IMAGE_CHANNELS 4
void AImageAsset::Load(const char* file){
	stbi_load(file, &Width, &Height, &Channels, IMAGE_CHANNELS);
}
void AImageAsset::Save(const char* file){
}

AImageAsset::~AImageAsset(){
	stbi_image_free(Pixels);
}

void AExternalMeshAsset::Load(const char* file) {
	LoadMeshFromGLTF(file, Primitives);
}

void ASceneAsset::Load(const char* file) {
	Json::Document doc;
	Json::ReadFile(file, doc);
	if(doc.HasMember("Camera")) {
		const rapidjson::Value& cameraParam = doc["Camera"].GetObject();
		Json::LoadVector3(cameraParam["Eye"], CameraParam.Eye);
		Json::LoadVector3(cameraParam["At"], CameraParam.At);
		Json::LoadVector3(cameraParam["Up"], CameraParam.Up);
		CameraParam.Near = cameraParam["Near"].GetFloat();
		CameraParam.Far = cameraParam["Far"].GetFloat();
		CameraParam.Fov = cameraParam["Fov"].GetFloat();
	}
	if(doc.HasMember("Objects")) {
		const Json::Value& objects = doc["Objects"];
		Objects.resize(objects.Size());
		for(uint32 i=0; i< objects.Size(); ++i) {
			const Json::Value& meshVal = objects[i].GetObject();
			Objects[i].Name = meshVal["Name"].GetString();
			Objects[i].File = meshVal["File"].GetString();
			Json::LoadVector3(meshVal["Position"], Objects[i].Position);
			Json::LoadVector3(meshVal["Scale"], Objects[i].Scale);
			Json::LoadVector3(meshVal["Rotation"], Objects[i].Euler);
		}
	}
}

void ASceneAsset::Save(const char* file) {
	Json::Document doc;
	doc.SetObject();
	// camera
	Json::Value cameraVal;
	auto& a = doc.GetAllocator();
	Json::AddVector3(cameraVal, "Eye", CameraParam.Eye, a);
	Json::AddVector3(cameraVal, "At", CameraParam.At, a);
	Json::AddVector3(cameraVal, "Up", CameraParam.Up, a);
	cameraVal.AddMember("Near", CameraParam.Near, a);
	cameraVal.AddMember("Far", CameraParam.Far, a);
	cameraVal.AddMember("Fov", CameraParam.Fov, a);
	doc.AddMember("Camera", cameraVal, a);
	// meshes
	Json::Value objectsVal;
	objectsVal.SetArray();
	for(auto& mesh: Objects) {
		Json::Value meshVal;
		Json::AddString(meshVal, "Name", mesh.Name, a);
		Json::AddString(meshVal, "File", mesh.File, a);
		Json::AddVector3(meshVal, "Position", mesh.Position, a);
		Json::AddVector3(meshVal, "Scale", mesh.Scale, a);
		Json::AddVector3(meshVal, "Rotation", mesh.Euler, a);
		objectsVal.PushBack(meshVal, a);
	}
	doc.AddMember("Objects", objectsVal, a);
	Json::WriteFile(file, doc);
}
