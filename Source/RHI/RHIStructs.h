#pragma once
#include "RHIEnum.h"
#include "Core/Container/Container.h"
#include "Core/typedefine.h"

namespace RHI {
	struct RSInitInfo {
		bool enableDebug;
		bool enableGeometryShader;
		int windowWidth;
		int windowHeight;
		void* windowHandle;
		const char* applicationName;
		uint8 maxFramesInFlight;
	};

	struct RSOffset2D {
		int32 x;
		int32 y;
	};

	struct RSExtent2D {
		uint32 width;
		uint32 height;
	};

	struct RSExtent3D {
		uint32 width;
		uint32 height;
		uint32 depth;
	};

	struct RSOffset3D {
		int32 x, y, z;
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
		RClearValueType Type{CLEAR_VALUE_NONE};
		union {
			struct {
				float r;
				float g;
				float b;
				float a;
			}Color;
			struct {
				float depth;
				uint32 stencil;
			}DepthStencil;
		};
		RSClear() = default;
		RSClear(float r, float g, float b, float a) {
			Type = CLEAR_VALUE_COLOR;
			Color.r = r; Color.g = g; Color.b = b; Color.a = a;
		}
		RSClear(float depth, uint32 stencil) {
			Type = CLEAR_VALUE_DEPTH_STENCIL;
			DepthStencil.depth = depth; DepthStencil.stencil = stencil;
		}
	};

	struct RSAttachment {
		RFormat format;
		RImageLayout initialLayout{ IMAGE_LAYOUT_UNDEFINED };
		RImageLayout finalLayout;
		RImageLayout refLayout;
		RImageLayout InputRefLayout;
		RSampleCountFlagBits sampleCount{ SAMPLE_COUNT_1_BIT };
		RAttachmentLoadOp loadOp{ ATTACHMENT_LOAD_OP_CLEAR };
		RAttachmentStoreOp storeOp{ ATTACHMENT_STORE_OP_STORE };
		RAttachmentLoadOp stencilLoadOp{ ATTACHMENT_LOAD_OP_DONT_CARE };
		RAttachmentStoreOp stencilStoreOp{ ATTACHMENT_STORE_OP_DONT_CARE };
		RSClear clear;
	};

	struct RSubPassInfo {
		RPipelineType Type;
		TVector<uint32> InputAttachments;
		TVector<uint32> ColorAttachments;
		int32 DepthStencilAttachment;
	};

	struct RSubpassDependency {
		uint32 SrcSubPass;
		RPipelineStageFlags SrcStage;
		RAccessFlags SrcAccess;
		uint32 DstSubPass;
		RPipelineStageFlags DstStage;
		RAccessFlags DstAccess;
		RDependencyFlags DependencyFlags;
	};

	struct RSImageBlit {
		RImageAspectFlags srcAspect;
		uint32		  srcMipLevel;
		uint32		  srcBaseLayer;
		uint32		  srcLayerCount;
		RSOffset3D		  srcOffsets[2];
		RImageAspectFlags dstAspect;
		uint32		  dstMipLevel;
		uint32		  dstBaseLayer;
		uint32		  dstLayerCount;
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
		float maxAnisotropy{0.0f};
		bool compareEnable{ false };
		RCompareOp compreOp;
		float minLod{ 0.0f };
		float maxLod{ 1000.0f }; // none
		RBorderColor borderColor{ BORDER_COLOR_FLOAT_OPAQUE_BLACK };
		bool unnormalizedCoordinates{ false };
	};

	struct RSDescriptorSetLayoutBinding {
		RDescriptorType      descriptorType;
		uint32              descriptorCount;
		RShaderStageFlags    stageFlags;
	};

	struct RSPushConstantRange {
		RShaderStageFlags stageFlags;
		uint32 offset;
		uint32 size;
	};

	// graphics pipeline create info
	struct RPipelineShaderInfo {

		RShaderStageFlagBits stage;
		TVector<int8> code;
		const char* funcName = "main";
	};

	struct RVertexInputBinding {
		uint32 binding;
		uint32 stride;
		RVertexInputRate inputRate;
	};

	struct RVertexInputAttribute
	{
		uint32 binding;
		RFormat format;
		uint32 offset;
	};

	struct RStencilOpState {
		RStencilOp failOp;
		RStencilOp passOp;
		RStencilOp depthFailOp;
		RCompareOp compareOp;
		uint32   compareMask;
		uint32   writeMask;
		uint32   reference;
	};

	struct RColorBlendAttachmentState {
		bool					blendEnable;
		RBlendFactor            srcColorBlendFactor{ BLEND_FACTOR_ZERO };
		RBlendFactor            dstColorBlendFactor{ BLEND_FACTOR_ZERO };
		RBlendOp                colorBlendOp{ BLEND_OP_ADD };
		RBlendFactor            srcAlphaBlendFactor{ BLEND_FACTOR_ZERO };
		RBlendFactor            dstAlphaBlendFactor{ BLEND_FACTOR_ZERO };
		RBlendOp                alphaBlendOp{ BLEND_OP_ADD };
		RColorComponentFlags    colorWriteMask{ COLOR_COMPONENT_ALL };
	};

	struct RGraphicsPipelineCreateInfo {

		// shader stages
		TVector<RPipelineShaderInfo> Shaders;

		// vertex input
		TVector<RVertexInputBinding> Bindings;
		TVector<RVertexInputAttribute> Attributes;

		// input assembly
		RPrimitiveTopology Topology{ PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
		bool PrimitiveRestartEnable{ false };

		// tessellation
		uint32 PatchControlPoints{0};

		// viewport
		RSViewport Viewport;
		RSRect2D Scissor;

		// rasterization
		bool DepthClampEnable;
		bool RasterizerDiscardEnable;
		RPolygonMode PolygonMode;
		RCullModeFlags CullMode;
		bool Clockwise{ false };
		bool DepthBiasEnable{ false };
		float DepthBiasConstantFactor{ 0.0f };
		float DepthBiasClamp{ 0.0f };
		float DepthBiasSlopeFactor{ 0.0f };
		float LineWidth{ 1.0f };

		// multi sample
		RSampleCountFlagBits RasterizationSamples{SAMPLE_COUNT_1_BIT};
		bool SampleShadingEnable{false};
		float MinSampleShading{0.0f};
		const RSampleMask* pSampleMask{nullptr};
		bool AlphaToCoverageEnable{false};
		bool AlphaToOneEnable{false};

		// depth stencil
		bool DepthTestEnable;
		bool DepthWriteEnable;
		RCompareOp DepthCompareOp{ COMPARE_OP_LESS };
		bool DepthBoundsTestEnable{false};
		bool StencilTestEnable {false};
		RStencilOpState FrontStencilOp;
		RStencilOpState BackStencilOp;
		float MinDepthBounds;
		float MaxDepthBounds;

		// color blend
		bool LogicOpEnable;
		RLogicOp LogicOp{ LOGIC_OP_COPY };
		TVector<RColorBlendAttachmentState> AttachmentStates;
		float BlendConstants[4];

		// dynamic
		TVector<RDynamicState> DynamicStates;
	};


	class RSampler;
	class RImageView;
	class RBuffer;

	// use for update descriptor set
	union RDescriptorInfo {
		struct {
			const RSampler* sampler;
			const RImageView* imageView;
		};

		struct {
			const RBuffer* buffer;
			uint64 offset;
			uint64 range;
		};
	};

}