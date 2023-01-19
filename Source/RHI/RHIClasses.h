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
		RImageLayout m_Layout;
	public:
		virtual ~RImage() {}
		RImageType GetType() { return m_Type; }
		RFormat GetFormat() { return m_Format; }
		const RSExtent3D& GetExtent() { return m_Extent; }
		RImageLayout GetLayout() { return m_Layout; }
	};

	class RImageView {
	protected:
		RImageLayout m_Layout;
	public:
		virtual ~RImageView() {};
		RImageLayout GetLayout() { return m_Layout; }
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

	struct BufferRange {
		RBuffer* buffer{nullptr};
		size_t   offset{0};
		size_t   range {0};
	};

	class RTexture {};
	class RShader {};

	class RDescriptorSet {};

	class RDevice {};
	class RFence {};
	class RSemaphore {};
	class RQueue {};

} // namespace RHI