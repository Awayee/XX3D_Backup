#include "Texture.h"

namespace Engine {

	Texture2D::Texture2D(RHI::RFormat format, uint32 width, uint32 height, RHI::RImageUsageFlags usage, RHI::RImageAspectFlags aspect, void* pData)
	{
		GET_RHI(rhi);
		m_Image = rhi->CreateImage2D(format, width, height, 1, RHI::SAMPLE_COUNT_1_BIT, RHI::IMAGE_TILING_OPTIMAL, usage);
		m_ImageMemory = rhi->CreateImageMemory(m_Image, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pData);
		m_ImageView = rhi->CreateImageView(m_Image, RHI::IMAGE_VIEW_TYPE_2D, aspect, 0, 1, 0, 1);
	}

	Texture2D::~Texture2D()
	{
		GET_RHI(rhi);
		rhi->DestroyImage(m_Image);
		rhi->FreeMemory(m_ImageMemory);
		rhi->DestroyImageView(m_ImageView);
	}
}