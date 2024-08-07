#include "Functions/Public/TextureImporter.h"
#include "Core/Public/macro.h"
#include "Asset/Public/AssetLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool TextureImporter::Import(const char* fullPath) {
	if (m_SaveFile.empty()) {
		File::FPath relativePath = File::RelativePath(File::FPath(fullPath), Engine::AssetLoader::AssetPath());
		relativePath.replace_extension(".texture");
		m_SaveFile = relativePath.string();
	}

	int width, height, channels;
	constexpr int desiredChannels = STBI_rgb_alpha;
	uint8* pixels = stbi_load(fullPath, &width, &height, &channels, desiredChannels);
	if(!pixels) {
		LOG("loaded image is empty!");
		return false;
	}
	m_Asset->Width = width;
	m_Asset->Height = height;
	m_Asset->Channels = desiredChannels;
	uint64 byteSize = width * height * desiredChannels;
	m_Asset->Pixels.Resize(byteSize);
	memcpy(m_Asset->Pixels.Data(), pixels, byteSize);
	stbi_image_free(pixels);
	return true;
}

bool TextureImporter::Save() {
	return Engine::AssetLoader::SaveProjectAsset(m_Asset, m_SaveFile.c_str());
}
