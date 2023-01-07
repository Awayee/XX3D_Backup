#pragma once
#include "RHIEnum.h"
#include <cstdint>

namespace RHI{
	struct RSInitInfo {
		bool enableDebug;
		bool enableGeometryShader;
		int windowWidth;
		int windowHeight;
		void* windowHandle;
		const char* applicationName;
		uint8_t maxFramesInFlight;
	};

	struct RSAttachment {
		RFormat format;
		RImageLayout initialLayout;
		RImageLayout finalLayout;
		RAttachmentType type;
	};

	struct RSOffset2D {
		int32_t x;
		int32_t y;
	};

	struct RSExtent2D {
		uint32_t width;
		uint32_t height;
	};

	struct RSExtent3D {
		uint32_t width;
		uint32_t height;
		uint32_t depth;
	};

	struct RSOffset3D {
		int32_t x, y, z;
	};

	struct RSRect2D {
		RSOffset2D offset;
		RSExtent2D extent;
	};

	struct RSClearDepthStencilValue {
		float depth;
		uint32_t stencil;
	};

	struct RSImageBlit {
		RImageAspectFlags srcAspect;
		uint32_t		  srcMipLevel;
		uint32_t		  srcBaseLayer;
		uint32_t		  srcLayerCount;
		RSOffset3D		  srcOffsets[2];
		RImageAspectFlags dstAspect;
		uint32_t		  dstMipLevel;
		uint32_t		  dstBaseLayer;
		uint32_t		  dstLayerCount;
		RSOffset3D		  dstOffsets[2];
	};

	struct RSSamplerInfo {
		RFilter magFilter;
		RFilter minFilter;
		RSamplerMipmapMode mipmapMode;
		RSamplerAddressMode addressModeU;
		RSamplerAddressMode addressModeV;
		RSamplerAddressMode addressModeW;
		float minLodBias{ 0.0f };
		bool anisotropyEnable{ false };
		float maxAnisotropy;
		bool compareEnable{ false };
		RCompareOp compreOp;
		float minLod{ 0.0f };
		float maxLod{ 1000.0f }; // none
		RBorderColor borderColor{BORDER_COLOR_FLOAT_OPAQUE_BLACK};
		bool unnormalizedCoordinates{ false };
	};

	struct RSDescriptorSetLayoutBinding {
		uint32_t              binding;
		RDescriptorType      descriptorType;
		uint32_t              descriptorCount;
		RShaderStageFlags    stageFlags;
	};

	struct RSPushConstantRange {
		RShaderStageFlags stageFlags;
		uint32_t offset;
		uint32_t size;
	};

	struct RSGraphicsPipelineCreateInfo {
		uint32_t x;
	};

	union RSClearColorValue {
		float float32[4];
		int32_t int32[4];
		uint32_t uint32[4];
	};

	union RSClearValue {
		RSClearColorValue color;
		RSClearDepthStencilValue depthStencil;
	};

	class RWindowHandle {
	};

	class RRenderPass {
	};

	class RPipeline {
	};

	class RImage {
	protected:
		RImageType m_Type;
		RFormat m_Format;
		RSExtent3D m_Extent;
	public:
		virtual ~RImage() {}
		RImageType GetType() { return m_Type; }
		RFormat GetFormat() { return m_Format; }
		const RSExtent3D GetExtent() { return m_Extent; }
	};

	class RImageView {
	public:
		virtual ~RImageView() {};
	};

	class RCommandBuffer {
	public:
		virtual ~RCommandBuffer() {}
	};

	class RMemory {
	public:
		virtual ~RMemory() {};
	};

	class RFramebuffer {
		
	};
	class RSampler {
		
	};

	class RDescriptorSetLayout {
		
	};

	class RBuffer {};
	class RTexture {};
	class RShader {};

	class RDescriptorSet {};

	class RDevice {};
	class RFence {};
	class RSemaphore {};
	class RQueue {};

} // namespace RHI