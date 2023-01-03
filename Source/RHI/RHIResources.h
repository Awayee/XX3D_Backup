#pragma once
#include "RHIEnum.h"
#include <cstdint>

namespace RHI{
	class RCommandBuffer {
	public:
		virtual ~RCommandBuffer() {}
	};

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

	struct RSRect2D {
		RSOffset2D offset;
		RSExtent2D extent;
	};

	struct RSClearDepthStencilValue {
		float depth;
		uint32_t stencil;
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

	class RImageView {
	public:
		virtual ~RImageView() {};
	};

	class RFramebuffer {
		
	};

	class RBuffer {};
	class RTexture {};
	class RSampler{};
	class RShader {};

	class RDescriptorSet {};

	class RDevice {};
	class RFence {};
	class RSemaphore {};
	class RQueue {};
	class RPipeline {};
}