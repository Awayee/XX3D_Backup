#pragma once
#include "RHIStructs.h"
#include "Core/macro.h"

namespace RHI{
	class RWindowHandle {
	};

	class RRenderPass {
	public:
		virtual void SetAttachment(uint32 idx, RImageView* imageView) = 0;
		virtual void SetClearValue(uint32 idx, const RSClear& clear) = 0;
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
		RImageLayout GetLayout() const { return m_Layout; }
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
	protected:
		TVector<const RImageView*> m_Attachments;
	public:
		const RImageView* GetAttachment(uint32 i) { ASSERT(i < m_Attachments.size()); return m_Attachments[i]; }
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
		uint64   offset{0};
		uint64   range {0};
	};

	class RTexture {};
	class RShader {};

	class RDescriptorSet {};

	class RDevice {};
	class RFence {};
	class RSemaphore {};
	class RQueue {};

} // namespace RHI