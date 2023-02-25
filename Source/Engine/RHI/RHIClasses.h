#pragma once
#include "RHIStructs.h"
#include "Core/macro.h"
#include "Core/BaseType/typedefine.h"

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
		USize3D m_Extent;
		RImageLayout m_Layout;
	public:
		virtual ~RImage() {}
		RImageType GetType() const { return m_Type; }
		RFormat GetFormat() const { return m_Format; }
		const USize3D& GetExtent() const { return m_Extent; }
		RImageLayout GetLayout() const { return m_Layout; }
	};

	class RImageView {
	protected:
		RImageLayout m_Layout;
	public:
		virtual ~RImageView() {};
		RImageLayout GetLayout() const { return m_Layout; }
	};

	class RMemory {
	public:
		virtual ~RMemory() {};
	};

	class RFramebuffer {
	protected:
		uint32 m_Width;
		uint32 m_Height;
	public:
		uint32 GetWidth() { return m_Width; }
		uint32 GetHeight() { return m_Height; }
	};

	class RSampler {
		
	};

	class RDescriptorSetLayout {
		
	};

	class RPipelineLayout {
		
	};

	class RBuffer {
	protected:
		uint64 m_Size;
	public:
		uint64 GetSize() { return m_Size; }
	};

	struct BufferRange {
		RBuffer* buffer{nullptr};
		uint64   offset{0};
		uint64   range {0};
	};

	class RTexture {};
	class RShader {};

	class RDescriptorSet {
	public:
		virtual void Update(uint32 binding, RDescriptorType type, const RDescriptorInfo& info, uint32 arrayElement, uint32 count) = 0;
		virtual void UpdateUniformBuffer(uint32 binding, RBuffer* buffer) = 0;
		virtual void UpdateImageSampler(uint32 binding, RSampler* sampler, RImageView* image) = 0;
		virtual void UpdateInputAttachment(uint32 binding, RImageView* image) = 0;
	};

	class RCommandBuffer {
	public:
		virtual ~RCommandBuffer() {}
		virtual void Begin(RCommandBufferUsageFlags flags) = 0;
		virtual void End() = 0;
		virtual void BeginRenderPass(RRenderPass* pass, RFramebuffer* framebuffer, const URect2D& area) = 0;
		virtual void NextSubpass() = 0;
		virtual void EndRenderPass() = 0;
		virtual void CopyBufferToImage(RBuffer* buffer, RImage* image, RImageAspectFlags aspect, uint32 mipLevel, uint32 baseLayer, uint32 layerCount) = 0;
		virtual void BlitImage(RCommandBuffer* cmd, RImage* srcImage, RImage* dstImage, const RSImageBlit& region) = 0;
		virtual void BindPipeline(RPipeline* pipeline) = 0;
		virtual void BindDescriptorSet(RPipelineLayout* layout, RDescriptorSet* descriptorSet, uint32 setIdx, RPipelineType pipelineType) = 0;
		virtual void BindVertexBuffer(RBuffer* buffer, uint32 first, uint64 offset) = 0;
		virtual void BindIndexBuffer(RBuffer* buffer, uint64 offset) = 0;
		virtual void Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstIndex, uint32 firstInstance) = 0;
		virtual void DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance) = 0;
		virtual void DrawVertices(RBuffer* buffer, uint32 vertexCount, uint32 instanceCount) = 0;
		virtual void DrawVerticesIndexed(RBuffer* vertexBuffer, RBuffer* indexBuffer, uint32 indexCount, uint32 instanceCount) = 0;
		virtual void Dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) = 0;
		virtual void ClearAttachment(RImageAspectFlags aspect, const float* color, const URect2D& rect) = 0;
		virtual void CopyBuffer(RBuffer* srcBuffer, RBuffer* dstBuffer, uint64 srcOffset, uint64 dstOffset, uint64 size) = 0;
		virtual void TransitionImageLayout(RImage* image, RImageLayout oldLayout, RImageLayout newLayout, uint32 baseMipLevel, uint32 levelCount, uint32 layer, uint32 layerCount, RImageAspectFlags aspect) = 0;
		virtual void GenerateMipmap(RImage* image, uint32 levelCount, RImageAspectFlags aspect, uint32 baseLayer, uint32 layerCount) = 0;
		virtual void BeginDebugLabel(const char* msg, const float* color) = 0;
		virtual void EndDebugLabel() = 0;
	};

	class RDevice {};
	class RFence {};
	class RSemaphore {};
	class RQueue {};

} // namespace RHI