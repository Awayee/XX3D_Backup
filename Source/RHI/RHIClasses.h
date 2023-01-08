#pragma once
#include "RHIStructs.h"

namespace RHI{
	class RWindowHandle {
	};

	class RRenderPass {
	};

	class RPipeline {
	protected:
		RPipelineType m_Type;
	public:
		RPipelineType GetType() { return m_Type; }
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

	class RPipelineLayout {
		
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