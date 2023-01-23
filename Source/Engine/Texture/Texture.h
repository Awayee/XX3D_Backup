#pragma once
#include "RHI/RHI.h"

namespace Engine {
	class Texture2D {
	private:
		RHI::RImage* m_Image;
		RHI::RImageView* m_ImageView;
		RHI::RMemory* m_ImageMemory;
	public:
		Texture2D() = delete;
		Texture2D(RHI::RFormat format, uint32 width, uint32 height, RHI::RImageUsageFlags usage, RHI::RImageAspectFlags aspect, void* pData);
		~Texture2D();
		RHI::RImage* GetImage() { return m_Image; }
		RHI::RImageView* GetImageView() { return m_ImageView; }
	};
}