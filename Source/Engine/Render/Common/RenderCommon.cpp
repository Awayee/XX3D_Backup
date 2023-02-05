#include "RenderCommon.h"
#include  "Core/File/CoreFile.h"
#include "Core/String/String.h"
#include "Core/Singleton/TSingleton.h"
#include "RenderResources.h"

namespace Engine {

	DescsMgrIns::DescsMgrIns() {
		GET_RHI(rhi);
		m_Layouts.resize(DESCS_COUNT);
		// lighting, camera
		TVector<RHI::RSDescriptorSetLayoutBinding> sceneDescBindings;
		sceneDescBindings.push_back({ RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, RHI::SHADER_STAGE_FRAGMENT_BIT | RHI::SHADER_STAGE_VERTEX_BIT });
		sceneDescBindings.push_back({ RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, RHI::SHADER_STAGE_FRAGMENT_BIT | RHI::SHADER_STAGE_VERTEX_BIT });
		m_Layouts[DESCS_SCENE] = rhi->CreateDescriptorSetLayout(sceneDescBindings.size(), sceneDescBindings.data());
		// world transform
		TVector<RHI::RSDescriptorSetLayoutBinding> modelDescBindings;
		modelDescBindings.push_back({ RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, RHI::SHADER_STAGE_VERTEX_BIT });
		m_Layouts[DESCS_MODEL] = rhi->CreateDescriptorSetLayout(modelDescBindings.size(), modelDescBindings.data());
		// material
		TVector<RHI::RSDescriptorSetLayoutBinding> materialDescBindings;
		materialDescBindings.push_back({ RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, RHI::SHADER_STAGE_FRAGMENT_BIT });
		m_Layouts[DESCS_MATERIAL] = rhi->CreateDescriptorSetLayout(materialDescBindings.size(), materialDescBindings.data());
		// deferred lighting
		TVector<RHI::RSDescriptorSetLayoutBinding> deferredLightingBindings;
		deferredLightingBindings.push_back({ RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, RHI::SHADER_STAGE_FRAGMENT_BIT }); //light
		deferredLightingBindings.push_back({ RHI::DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, RHI::SHADER_STAGE_FRAGMENT_BIT });//normal
		deferredLightingBindings.push_back({ RHI::DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, RHI::SHADER_STAGE_FRAGMENT_BIT });//albedo
		deferredLightingBindings.push_back({ RHI::DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, RHI::SHADER_STAGE_FRAGMENT_BIT });// depth
		m_Layouts[DESCS_DEFERRED_LIGHTING] = rhi->CreateDescriptorSetLayout(deferredLightingBindings.size(), deferredLightingBindings.data());
	}

	DescsMgrIns::~DescsMgrIns()
	{
		GET_RHI(rhi);
		for(auto layout: m_Layouts) {
			rhi->DestroyDescriptorSetLayout(layout);
		}
		m_Layouts.clear();
	}

	SamplerMgrIns::SamplerMgrIns()
	{
		GET_RHI(rhi);
		m_Samplers.resize(SAMPLER_COUNT);
		RHI::RSSamplerInfo defaulIInfo{};
		defaulIInfo.minFilter = RHI::FILTER_LINEAR;
		defaulIInfo.magFilter = RHI::FILTER_LINEAR;
		defaulIInfo.addressModeU = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		defaulIInfo.addressModeV = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		defaulIInfo.addressModeW = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		m_Samplers[SAMPLER_DEFAULT] = rhi->CreateSampler(defaulIInfo);

		RHI::RSSamplerInfo deferredLightingInfo{};
		deferredLightingInfo.minFilter = RHI::FILTER_LINEAR;
		deferredLightingInfo.magFilter = RHI::FILTER_LINEAR;
		deferredLightingInfo.addressModeU = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		deferredLightingInfo.addressModeV = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		deferredLightingInfo.addressModeW = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		m_Samplers[SAMPLER_DEFERRED_LIGHTING] = rhi->CreateSampler(deferredLightingInfo);
	}

	SamplerMgrIns::~SamplerMgrIns()
	{
		GET_RHI(rhi);
		for(auto sampler: m_Samplers) {
			rhi->DestroySampler(sampler);
		}
		m_Samplers.clear();
	}

	void Attachment::Create(RHI::RFormat format, uint32 width, uint32 height, RHI::RImageUsageFlags usage)
	{
		Release();
		GET_RHI(rhi);
		Image = rhi->CreateImage2D(format, width, height, 1, RHI::SAMPLE_COUNT_1_BIT, RHI::IMAGE_TILING_OPTIMAL, usage);
		Memory = rhi->CreateImageMemory(Image, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
		const bool bForDepth = usage & RHI::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		View = rhi->CreateImageView(Image, RHI::IMAGE_VIEW_TYPE_2D, bForDepth ? RHI::IMAGE_ASPECT_DEPTH_BIT: RHI::IMAGE_ASPECT_COLOR_BIT,
			0, 1, 0, 1);
	}

	void Attachment::Release()
	{
		GET_RHI(rhi);
		if (nullptr != View) {
			rhi->DestroyImageView(View);
			View = nullptr;
		}
		if (nullptr != Memory) {
			rhi->FreeMemory(Memory);
			Memory = nullptr;
		}
		if (nullptr != Image) {
			rhi->DestroyImage(Image);
			Image = nullptr;
		}
	}
	void BufferCommon::Create(uint64 size, RHI::RBufferUsageFlags usage, RHI::RMemoryPropertyFlags memoryFlags, void* pData) {
		if (Size) Release();
		RHI_INSTANCE->CreateBufferWithMemory(size, usage, memoryFlags, Buffer, Memory, size, pData);
		Size = size;
		Usage = usage;
	}

	void BufferCommon::UpdateData(void* pData)
	{
		GET_RHI(rhi);
		void* p;
		rhi->MapMemory(Memory, &p);
		memcpy(p, pData, Size);
		rhi->UnmapMemory(Memory);
	}

	void BufferCommon::Release() {
		if(Size) {
			GET_RHI(rhi);
			rhi->DestroyBuffer(Buffer);
			Buffer = nullptr;
			rhi->FreeMemory(Memory);
			Memory = nullptr;
			Size = 0u;
			Usage = RHI::BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
		}
	}


	RenderPassCommon::~RenderPassCommon()
	{
		for(auto attachment: m_Attachments) {
			attachment.Release();
		}
		m_Attachments.clear();
		GET_RHI(rhi);
		if(nullptr != m_Framebuffer) rhi->DestroyFramebuffer(m_Framebuffer);
		if(nullptr != m_RHIPass) rhi->DestroyRenderPass(m_RHIPass);
	}
	void RenderPassCommon::Begin(RHI::RCommandBuffer* cmd)
	{
		cmd->BeginRenderPass(m_RHIPass, m_Framebuffer, { {0, 0}, {m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight()} });
	}

	RHI::RImageView* RenderPassCommon::GetAttachment(uint32 attachmentIdx) const
	{
		ASSERT(attachmentIdx < m_Attachments.size());
		return m_Attachments[attachmentIdx].View;
	}

	DeferredLightingPass::DeferredLightingPass()
	{
		GET_RHI(rhi);
		uint32 width = rhi->GetSwapchainExtent().width;
		uint32 height = rhi->GetSwapchainExtent().height;
		// create attachments
		m_Attachments.resize(ATTACHMENT_COLOR_KHR);
		m_Attachments[ATTACHMENT_DEPTH].Create(rhi->GetDepthFormat(), width, height, RHI::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT|RHI::IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
		m_Attachments[ATTACHMENT_NORMAL].Create(RHI::FORMAT_R8G8B8A8_UNORM, width, height, RHI::IMAGE_USAGE_COLOR_ATTACHMENT_BIT|RHI::IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
		m_Attachments[ATTACHMENT_ALBEDO].Create(RHI::FORMAT_R8G8B8A8_UNORM, width, height, RHI::IMAGE_USAGE_COLOR_ATTACHMENT_BIT|RHI::IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
		
		TVector<RHI::RSAttachment> attachments(ATTACHMENT_COUNT);
		// depth
		attachments[ATTACHMENT_DEPTH].Format = rhi->GetDepthFormat();
		attachments[ATTACHMENT_DEPTH].InitialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		attachments[ATTACHMENT_DEPTH].FinalLayout = RHI::IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[ATTACHMENT_DEPTH].Clear = { 1.0, 0 };
		// g buffer normal
		attachments[ATTACHMENT_NORMAL].Format = RHI::FORMAT_R8G8B8A8_UNORM;
		attachments[ATTACHMENT_NORMAL].InitialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		attachments[ATTACHMENT_NORMAL].FinalLayout = RHI::IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		attachments[ATTACHMENT_NORMAL].Clear = { 0.0f, 0.0f, 0.0f, 0.0f };
		// g buffer albedo
		attachments[ATTACHMENT_ALBEDO] = attachments[ATTACHMENT_NORMAL];
		// swapchain
		attachments[ATTACHMENT_COLOR_KHR].Format = rhi->GetSwapchainImageFormat();
		attachments[ATTACHMENT_COLOR_KHR].InitialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		attachments[ATTACHMENT_COLOR_KHR].FinalLayout = RHI::IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachments[ATTACHMENT_COLOR_KHR].Clear = { 0.0f, 0.0f, 0.0f, 0.0f };

		// subpass
		m_ColorAttachments.resize(SUBPASS_COUNT);
		m_ColorAttachments[SUBPASS_BASE] = { &m_Attachments[ATTACHMENT_NORMAL], &m_Attachments[ATTACHMENT_ALBEDO] };
		m_DepthAttachments.resize(1);
		m_DepthAttachments[SUBPASS_BASE] = &m_Attachments[ATTACHMENT_DEPTH];
		TVector<RHI::RSubPassInfo> subpasses(SUBPASS_COUNT);
		// base pass
		subpasses[SUBPASS_BASE].ColorAttachments = {
			{ATTACHMENT_NORMAL, RHI::IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
			{ATTACHMENT_ALBEDO, RHI::IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
		};
		subpasses[SUBPASS_BASE].DepthStencilAttachment = { ATTACHMENT_DEPTH, RHI::IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		// lighting pass
		subpasses[SUBPASS_DEFERRED_LIGHTING].ColorAttachments = {
			{(uint32)attachments.size() - 1, RHI::IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
		};
		subpasses[SUBPASS_DEFERRED_LIGHTING].InputAttachments = {
			{ATTACHMENT_NORMAL, RHI::IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
			{ATTACHMENT_ALBEDO, RHI::IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
			{ATTACHMENT_DEPTH,  RHI::IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
		};
		subpasses[SUBPASS_DEFERRED_LIGHTING].DepthStencilAttachment = { 0, RHI::IMAGE_LAYOUT_UNDEFINED };

		// dependencies
		TVector<RHI::RSubpassDependency> dependencies(2);
		dependencies[0].SrcSubPass = SUBPASS_INTERNAL;
		dependencies[0].DstSubPass = SUBPASS_BASE;
		dependencies[0].SrcStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].DstStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].SrcAccess = 0;
		dependencies[0].DstAccess = RHI::ACCESS_COLOR_ATTACHMENT_WRITE_BIT | RHI::ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		// lighting pass depends on base pass
		dependencies[1].SrcSubPass = SUBPASS_BASE;
		dependencies[1].DstSubPass = SUBPASS_DEFERRED_LIGHTING;
		dependencies[1].SrcStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].DstStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].SrcAccess = RHI::ACCESS_SHADER_WRITE_BIT | RHI::ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].DstAccess = RHI::ACCESS_SHADER_READ_BIT | RHI::ACCESS_COLOR_ATTACHMENT_READ_BIT;
		dependencies[1].DependencyFlags = RHI::DEPENDENCY_BY_REGION_BIT;

		m_RHIPass = rhi->CreateRenderPass(attachments.size(), attachments.data(),
			subpasses.size(), subpasses.data(),
			dependencies.size(), dependencies.data());

		// create framebuffers
		uint32 maxImageCount = rhi->GetSwapchainMaxImageCount();
		m_SwapchainFramebuffers.resize(maxImageCount);
		for(uint32 i=0; i<maxImageCount;++i) {
			TVector<RHI::RImageView*> attachments;
			for (auto& img : m_Attachments) {
				attachments.push_back(img.View);
			}
			attachments.push_back(rhi->GetSwapchainImageView(i));
			m_SwapchainFramebuffers[i] = rhi->CreateFrameBuffer(m_RHIPass, attachments.size(), attachments.data(), width, height, 1);
		}
		m_Framebuffer = m_SwapchainFramebuffers[0];
	}

	DeferredLightingPass::~DeferredLightingPass()
	{
		m_Framebuffer = nullptr;
		GET_RHI(rhi);
		for(auto framebuffer: m_SwapchainFramebuffers) {
			rhi->DestroyFramebuffer(framebuffer);
		}
		m_SwapchainFramebuffers.clear();
	}

	void DeferredLightingPass::SetImageIndex(uint32 i)
	{
		m_Framebuffer = m_SwapchainFramebuffers[i];
	}

	GraphicsPipelineCommon::~GraphicsPipelineCommon()
	{
		GET_RHI(rhi);
		rhi->DestroyPipelineLayout(m_Layout);
		rhi->DestroyPipeline(m_Pipeline);
	}

	void GraphicsPipelineCommon::Bind(RHI::RCommandBuffer* cmd) {
		cmd->BindPipeline(m_Pipeline);
	}


	GBufferPipeline::GBufferPipeline(const RenderPassCommon* pass, uint32 subpass)
	{
		GET_RHI(rhi);
		// layout
		TVector<RHI::RDescriptorSetLayout*> setLayouts;
		setLayouts.push_back(DescsMgr::Get(DESCS_SCENE));
		setLayouts.push_back(DescsMgr::Get(DESCS_MODEL));
		setLayouts.push_back(DescsMgr::Get(DESCS_MATERIAL));
		
		m_Layout = rhi->CreatePipelineLayout(setLayouts.size(), setLayouts.data(), 0, nullptr);
		// pipeline
		// shader
		TVector<int8> vertShaderCode;
		LoadShaderFile("GBuffer.vert.spv", vertShaderCode);
		TVector<int8> fragShaderCode;
		LoadShaderFile("GBuffer.frag.spv", fragShaderCode);
		RHI::RGraphicsPipelineCreateInfo info{};
		info.Shaders.push_back({ RHI::SHADER_STAGE_VERTEX_BIT, vertShaderCode, "main" });
		info.Shaders.push_back({ RHI::SHADER_STAGE_FRAGMENT_BIT, fragShaderCode, "main" });
		// input
		FillVertexInput(info.Bindings, info.Attributes);
		// viewport
		uint32 w = pass->GetFramebuffer()->GetWidth();
		uint32 h = pass->GetFramebuffer()->GetHeight();
		info.Viewport = { 0.0f, 0.0f, (float)w, (float)h, 0.0f, 1.0f };
		info.Scissor = { {0, 0}, {w, h} };
		// rasterization
		info.DepthClampEnable = false;
		info.RasterizerDiscardEnable = false;
		info.PolygonMode = RHI::POLYGON_MODE_FILL;
		info.CullMode = RHI::CULL_MODE_BACK_BIT;
		info.Clockwise = true;
		// depth
		info.DepthTestEnable = true;
		info.DepthWriteEnable = true;
		info.DepthCompareOp = RHI::COMPARE_OP_LESS;
		// blend
		info.LogicOpEnable = false;
		info.AttachmentStates.resize(pass->GetColorAttachmentCount(subpass), {false});

		m_Pipeline = rhi->CreateGraphicsPipeline(info, m_Layout, pass->GetRHIPass(), subpass, nullptr, 0);
	}

	DeferredLightingPipeline::DeferredLightingPipeline(const RenderPassCommon* pass, uint32 subpass)
	{
		GET_RHI(rhi);
		TVector<RHI::RDescriptorSetLayout*> setLayouts{ DescsMgr::Get(DESCS_DEFERRED_LIGHTING) };
		m_Layout = rhi->CreatePipelineLayout(setLayouts.size(), setLayouts.data(), 0, nullptr);
		// shader
		TVector<int8> vertShaderCode;
		LoadShaderFile("DeferredLighting.vert.spv", vertShaderCode);
		TVector<int8> fragShaderCode;
		LoadShaderFile("DeferredLighting.frag.spv", fragShaderCode);
		RHI::RGraphicsPipelineCreateInfo info{};
		info.Shaders.push_back({ RHI::SHADER_STAGE_VERTEX_BIT, vertShaderCode, "main" });
		info.Shaders.push_back({ RHI::SHADER_STAGE_FRAGMENT_BIT, fragShaderCode, "main" });
		// vertex input
		//FillVectorInput(info.Bindings, info.Attributes);
		// viewport
		uint32 w = pass->GetFramebuffer()->GetWidth();
		uint32 h = pass->GetFramebuffer()->GetHeight();
		info.Viewport = { 0.0f, 0.0f, (float)w, (float)h, 0.0f, 1.0f };
		info.Scissor = { {0, 0}, {w, h} };
		// rasterization
		info.DepthClampEnable = false;
		info.RasterizerDiscardEnable = false;
		info.PolygonMode = RHI::POLYGON_MODE_FILL;
		info.CullMode = RHI::CULL_MODE_BACK_BIT;
		// depth
		info.DepthTestEnable = false;
		info.DepthWriteEnable = false;
		// blend
		info.LogicOpEnable = false;
		info.AttachmentStates.resize(1, { false });//swapchain
		m_Pipeline = rhi->CreateGraphicsPipeline(info, m_Layout, pass->GetRHIPass(), subpass, nullptr, 0);
	}
}