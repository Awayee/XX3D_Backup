#pragma once
#include "RHIEnum.h"
#include <cstdint>

namespace RHI {
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
		RImageLayout initialLayout{IMAGE_LAYOUT_UNDEFINED};
		RImageLayout finalLayout;
		RAttachmentType type;
		RSampleCountFlagBits sampleCount{ SAMPLE_COUNT_1_BIT };
		RAttachmentLoadOp loadOp{ ATTACHMENT_LOAD_OP_CLEAR };
		RAttachmentStoreOp storeOp{ ATTACHMENT_STORE_OP_STORE };
		RAttachmentLoadOp stencilLoadOp{ ATTACHMENT_LOAD_OP_DONT_CARE };
		RAttachmentStoreOp stencilStoreOp{ ATTACHMENT_STORE_OP_DONT_CARE };
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

	struct RSViewport {
		float x;
		float y;
		float width;
		float height;
		float minDepth;
		float maxDepth;
	};

	struct RSClear {
		RClearValueType clearType;
		union {
			float color[4];
			struct {
				float depth;
				uint32_t stencil;
			}depthStencil;
		}clearValue;
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
		RBorderColor borderColor{ BORDER_COLOR_FLOAT_OPAQUE_BLACK };
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

	// graphics pipeline create info
	struct RPipelineShaderInfo {
		RShaderStageFlagBits stage;
		size_t codeSize;
		const uint32_t* pCode;
		const char* funcName = "main";
	};

	struct RVertexInputBinding {
		uint32_t binding;
		uint32_t stride;
		RVertexInputRate inputRate;
	};

	struct RVertexInputAttributeDescription
	{
		uint32_t location;
		uint32_t binding;
		RFormat format;
		uint32_t offset;
	};

	struct RVertexInput {
		uint32_t bindingCount;
		const RVertexInputBinding* bindings;
		uint32_t attributeCount;
		const RVertexInputAttributeDescription* attributes;
	};

	struct RPipelineInputAssembly
	{
		RPrimitiveTopology topology{PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
		bool primitiveRestartEnable{false};
	};

	struct RPipelineTessellation {
		uint32_t patchControlPoints;
	};

	struct RPipelineViewport {
		RSViewport viewport;
		RSRect2D Scissor;
	};

	//struct RPipelineViewport {
	//	uint32_t viewportCount;
	//	const RSViewport* pViewports;
	//	uint32_t scissorCount;
	//	const RSRect2D* pScissors;
	//};

	struct RPipelineRasterizationState {
		bool depthClampEnable;
		bool rasterizerDiscardEnable;
		RPolygonMode polygonMode;
		RCullModeFlags cullMode;
		bool clockwise{false};
		bool depthBiasEnable{false};
		float depthBiasConstantFactor{0.0f};
		float depthBiasClamp{0.0f};
		float depthBiasSlopeFactor{0.0f};
		float lineWidth{1.0f};
	};

	struct RPipelineMultisample {
		RSampleCountFlagBits rasterizationSamples;
		bool sampleShadingEnable;
		float minSampleShading;
		const RSampleMask* pSampleMask;
		bool alphaToCoverageEnable;
		bool alphaToOneEnable;
	};

	struct RStencilOpState {
		RStencilOp failOp;
		RStencilOp passOp;
		RStencilOp depthFailOp;
		RCompareOp compareOp;
		uint32_t   compareMask;
		uint32_t   writeMask;
		uint32_t   reference;
	};

	struct RPipelineDepthStencil {
		bool depthTestEnable;
		bool depthWriteEnable;
		RCompareOp depthCompareOp;
		bool depthBoundsTestEnable;
		bool stencilTestEnable;
		RStencilOpState front;
		RStencilOpState back;
		float minDepthBounds;
		float maxDepthBounds;
	};

	struct RColorBlendAttachmentState {
		bool					blendEnable;
		RBlendFactor            srcColorBlendFactor;
		RBlendFactor            dstColorBlendFactor;
		RBlendOp                colorBlendOp;
		RBlendFactor            srcAlphaBlendFactor;
		RBlendFactor            dstAlphaBlendFactor;
		RBlendOp                alphaBlendOp;
		RColorComponentFlags    colorWriteMask;
	};

	struct RPipelineColorBlend {
		bool logicOpEnable;
		RLogicOp logicOp;
		uint32_t attachmentCount;
		const RColorBlendAttachmentState* pAttachments;
		float blendConstants[4];
	};

	struct RGraphicsPipelineCreateInfo {

		// shader stages
		uint32_t shaderCount;
		const RPipelineShaderInfo* shaders;

		// vertex input
		uint32_t bindingCount;
		const RVertexInputBinding* bindings;
		uint32_t attributeCount;
		const RVertexInputAttributeDescription* attributes;

		// input assembly
		RPrimitiveTopology topology{ PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
		bool primitiveRestartEnable{ false };

		// tessellation
		uint32_t patchControlPoints;

		// viewport
		RSViewport viewport;
		RSRect2D Scissor;

		// rasterization
		bool depthClampEnable;
		bool rasterizerDiscardEnable;
		RPolygonMode polygonMode;
		RCullModeFlags cullMode;
		bool clockwise{ false };
		bool depthBiasEnable{ false };
		float depthBiasConstantFactor{ 0.0f };
		float depthBiasClamp{ 0.0f };
		float depthBiasSlopeFactor{ 0.0f };
		float lineWidth{ 1.0f };

		// multi sample
		RSampleCountFlagBits rasterizationSamples{SAMPLE_COUNT_1_BIT};
		bool sampleShadingEnable{false};
		float minSampleShading{0.0f};
		const RSampleMask* pSampleMask{nullptr};
		bool alphaToCoverageEnable{false};
		bool alphaToOneEnable{false};

		// depth stencil
		bool depthTestEnable;
		bool depthWriteEnable;
		RCompareOp depthCompareOp;
		bool depthBoundsTestEnable{false};
		bool stencilTestEnable {false};
		RStencilOpState front;
		RStencilOpState back;
		float minDepthBounds;
		float maxDepthBounds;

		// color blend
		bool logicOpEnable;
		RLogicOp logicOp;
		uint32_t attachmentCount;
		const RColorBlendAttachmentState* pAttachments;
		float blendConstants[4];

		// dynamic
		uint32_t dynamicStateCount;
		const RDynamicState* pDynamicStates;
	};

}