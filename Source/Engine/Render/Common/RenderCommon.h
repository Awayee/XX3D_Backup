#pragma once
#include "RHI/RHI.h"
#include "Core/Memory/SmartPointer.h"
#include "Core/Singleton/TSingleton.h"
namespace Engine {
	enum EDescsType {
		DESCS_SCENE,
		DESCS_MODEL,
		DESCS_MATERIAL,
		DESCS_DEFERRED_LIGHTING,

		DESCS_COUNT
	};

	class DescsMgrIns {
	private:
		TVector<RHI::RDescriptorSetLayout*> m_Layouts;
	public:
		DescsMgrIns();
		~DescsMgrIns();
		RHI::RDescriptorSetLayout* Get(EDescsType type) { return m_Layouts[type]; }
	};

	enum ESamplerType{
		SAMPLER_DEFAULT,
		SAMPLER_DEFERRED_LIGHTING,
		SAMPLER_COUNT
	};
	class SamplerMgrIns {
	private:
		TVector<RHI::RSampler*> m_Samplers;
	public:
		SamplerMgrIns();
		~SamplerMgrIns();
		RHI::RSampler* Get(ESamplerType type) { return m_Samplers[type]; }
	};

	class DescsMgr: public TSingleton<DescsMgrIns> {
	public:
		static RHI::RDescriptorSetLayout* Get(EDescsType type) { return Instance()->Get(type); }
	};

	class SamplerMgr: public TSingleton<SamplerMgrIns> {
	public:
		static RHI::RSampler* Get(ESamplerType type) { return Instance()->Get(type); }
	};

	struct TextureData {
	public:
		uint32 Width{0};
		uint32 Height{0};
		uint32 Depth{0};
		uint32 Miplevels{0};
		uint32 ArrayLayers{0};
		void* Pixels{0};
		RHI::RFormat Format{RHI::FORMAT_UNDEFINED};
		TextureData() = default;
		TextureData(uint32 width, uint32 height, uint32 depth=1) : Width(width), Height(height), Depth(depth) {};
		TextureData(const char* file, uint8 channels=4);
		~TextureData();
	};

	struct TextureCommon {
		RHI::RImage* Image{nullptr};
		RHI::RImageView* View{nullptr};
		RHI::RMemory* Memory{nullptr};
		void Create(RHI::RFormat format, uint32 width, uint32 height, RHI::RImageUsageFlags usage);
		void Release();
	};

	struct BufferCommon {
		RHI::RBuffer* Buffer{nullptr};
		RHI::RMemory* Memory{nullptr};
		uint64 Size{ 0u };
		RHI::RBufferUsageFlags Usage{RHI::BUFFER_USAGE_FLAG_BITS_MAX_ENUM};
	public:
		BufferCommon() = default;
		BufferCommon(const BufferCommon&) = default;
		BufferCommon(BufferCommon&&) = default;
		BufferCommon(uint64 size, RHI::RBufferUsageFlags usage, RHI::RMemoryPropertyFlags memoryFlags, void* pData) { Create(size, usage, memoryFlags, pData); }
		void Create(uint64 size, RHI::RBufferUsageFlags usage, RHI::RMemoryPropertyFlags memoryFlags, void* pData);
		void UpdateData(void* pData);
		void Release();
		~BufferCommon() { Release(); }

		// vertex buffer
		void CreateForVertex(uint64 size){
			Create(size, RHI::BUFFER_USAGE_VERTEX_BUFFER_BIT | RHI::BUFFER_USAGE_TRANSFER_DST_BIT, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
		};
		// index buffer
		void CreateForIndex(uint64 size){
			Create(size, RHI::BUFFER_USAGE_INDEX_BUFFER_BIT | RHI::BUFFER_USAGE_TRANSFER_DST_BIT, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
		};
		// staging buffer
		void CreateForStaging(uint64 size, void* pData){
			Create(size, RHI::BUFFER_USAGE_TRANSFER_SRC_BIT, RHI::MEMORY_PROPERTY_HOST_COHERENT_BIT | RHI::MEMORY_PROPERTY_HOST_VISIBLE_BIT, pData);
		};
		// uniform buffer
		void CreateForUniform(uint64 size, void* pData){
			Create(size, RHI::BUFFER_USAGE_UNIFORM_BUFFER_BIT, RHI::MEMORY_PROPERTY_HOST_COHERENT_BIT | RHI::MEMORY_PROPERTY_HOST_VISIBLE_BIT, pData);
		};
		/*
		// vertex buffer
		static BufferCommon* CreateForVertex(uint64 size) {
			return new BufferCommon(size, RHI::BUFFER_USAGE_VERTEX_BUFFER_BIT | RHI::BUFFER_USAGE_TRANSFER_DST_BIT, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
		}
		// index buffer
		static BufferCommon* CreateForIndex(uint64 size){
			return new BufferCommon(size, RHI::BUFFER_USAGE_INDEX_BUFFER_BIT | RHI::BUFFER_USAGE_TRANSFER_DST_BIT, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
		}
		// staging buffer
		static BufferCommon* CreateForStaging(uint64 size, void* pData){
			return new BufferCommon(size, RHI::BUFFER_USAGE_TRANSFER_SRC_BIT, RHI::MEMORY_PROPERTY_HOST_COHERENT_BIT | RHI::MEMORY_PROPERTY_HOST_VISIBLE_BIT, pData);
		}
		// uniform buffer
		static BufferCommon* CreateForUniform(uint64 size){
			return new BufferCommon(size, RHI::BUFFER_USAGE_UNIFORM_BUFFER_BIT, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
		}
		*/
	};

	class RenderPassCommon {
	protected:
		RHI::RRenderPass* m_RHIPass{nullptr};
		RHI::RFramebuffer* m_Framebuffer{nullptr};
		TVector<TextureCommon> m_Attachments;
		TVector<TVector<TextureCommon*>> m_ColorAttachments;
		TVector<TextureCommon*> m_DepthAttachments;
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

	class DeferredLightingPass final: public RenderPassCommon {
	private:
		TVector<RHI::RFramebuffer*> m_SwapchainFramebuffers;
	public:
		enum {
			ATTACHMENT_DEPTH,
			ATTACHMENT_NORMAL,
			ATTACHMENT_ALBEDO,
			ATTACHMENT_COLOR_KHR,
			ATTACHMENT_COUNT,
		};

		enum {
			SUBPASS_BASE,
			SUBPASS_DEFERRED_LIGHTING,
			SUBPASS_COUNT
		};
		DeferredLightingPass();
		~DeferredLightingPass();
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

	class DeferredLightingPipeline: public GraphicsPipelineCommon {
	public:
		DeferredLightingPipeline(const RenderPassCommon* pass, uint32 subpass);
	};
}