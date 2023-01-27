#pragma once
#include "RHI/RHI.h"
#include "Core/Memory/SmartPointer.h"
#include "Core/Singleton/Singleton.h"
namespace Engine {
	enum EDescsType {
		DESCS_SCENE,
		DESCS_MODEL,
		DESCS_MATERIAL,

		DESCS_COUNT
	};

	class DescsMgrIns {
	private:
		TVector<RHI::RDescriptorSetLayout*> m_Layouts;
	public:
		DescsMgrIns();
		~DescsMgrIns();
		RHI::RDescriptorSetLayout* GetLayout(EDescsType type) { return m_Layouts[type]; }
	};

	class DescsMgr: public Singleton<DescsMgrIns> {
	public:
		static RHI::RDescriptorSetLayout* GetLayout(EDescsType type) {
			return Instance()->GetLayout(type);
		}
	};

	struct Attachment {
		RHI::RImage* Image{nullptr};
		RHI::RImageView* View{nullptr};
		RHI::RMemory* Memory{nullptr};
		void Create(RHI::RFormat format, uint32 width, uint32 height, bool bForDepth, bool bForShader);
		void Release();
	};

	struct BufferCommon {
		RHI::RBuffer* Buffer;
		RHI::RMemory* Memory;
		uint64 Size{ 0 };
		RHI::RBufferUsageFlags Usage{RHI::BUFFER_USAGE_FLAG_BITS_MAX_ENUM};
		void Create(uint64 size, RHI::RBufferUsageFlags usage, RHI::RMemoryPropertyFlags memoryFlags, void* pData);
		void Release();
		void Map(void** pData) { RHI_INSTANCE->MapMemory(Memory, pData); };
		void Unmap() { RHI_INSTANCE->UnmapMemory(Memory); };
		~BufferCommon() { Release(); }
	};

	class RenderPassCommon {
	protected:
		RHI::RRenderPass* m_RHIPass{nullptr};
		RHI::RFramebuffer* m_Framebuffer{nullptr};
		TVector<Attachment> m_Attachments;
		TVector<TVector<Attachment*>> m_ColorAttachments;
		TVector<Attachment*> m_DepthAttachments;
	public:
		virtual ~RenderPassCommon();
		RHI::RRenderPass* GetRHIPass() const { return m_RHIPass; }
		RHI::RFramebuffer* GetFramebuffer() const { return m_Framebuffer; }
		uint32 GetAttachmentCount() const { return m_Attachments.size(); }
		RHI::RImageView* GetAttachment(uint32 attachmentIdx) const;
		uint32 GetColorAttachmentCount(uint32 subpass) const { ASSERT(subpass < m_ColorAttachments.size()); return m_ColorAttachments[subpass].size(); }
		RHI::RImageView* GetColorAttachment(uint32 subpass, uint32 idx) const { return m_ColorAttachments[subpass][idx]->View; }
		RHI::RImageView* GetDepthAttachment(uint32 subpass) const { return m_DepthAttachments[subpass]->View; }
		virtual void Begin(RHI::RCommandBuffer* cmd);
	};

	class PresentPass final: public RenderPassCommon {
	private:
		TVector<RHI::RFramebuffer*> m_SwapchainFramebuffers;
	public:
		enum {
			ATTACHMENT_DEPTH,
			ATTACHMENT_NORMAL,
			ATTACHMENT_ALBEDO,
			ATTACHMENT_COUNT
		};

		enum {
			SUBPASS_GBUFFER,
			SUBPASS_LIGHTING,
			SUBPASS_COUNT
		};
		PresentPass();
		~PresentPass();
		void SetImageIndex(uint32 i);
	};

	class GraphicsPipelineCommon {
	protected:
		RHI::RPipelineLayout* m_Layout;
		RHI::RPipeline* m_Pipeline;
	public:
		virtual ~GraphicsPipelineCommon();
		RHI::RPipelineLayout* GetLayout()const { return m_Layout; }
		void Bind(RHI::RCommandBuffer* cmd);
	};

	class GBufferPipeline: public GraphicsPipelineCommon {
	public:
		GBufferPipeline(const RenderPassCommon* pass, uint32 subpass);
	};

	class LightingPipeline: public GraphicsPipelineCommon {
	public:
		LightingPipeline(const RenderPassCommon* pass, uint32 subpass);
	};
}