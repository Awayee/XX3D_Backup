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
	};

	struct RSAttachment {
		RFormat format;
		RImageLayout initialLayout;
		RImageLayout finalLayout;
		RAttachmentType type;
	};

	class RWindowHandle {
	};

	class RRenderPass {
	};

	class RBuffer {};
	class RTexture {};
	class RFrameBuffer {};
	class RSampler{};
	class RShader {};

	class RDescriptorSet {};

	class RDevice {};
	class RFence {};
	class RSemaphore {};
	class RQueue {};
	class RPipeline {};
}