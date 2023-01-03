#include "RHIInstance.h"
#include "RHIVulkan/RHIVulkan.h"
#include "Resource/Config/Config.h"
#include "Core/macro.h"
#include <memory>
#include <mutex>

namespace RHI{

	RHIInstance* GetInstance() {
		static std::unique_ptr<RHIInstance> s_Instance;
		static std::mutex s_InstanceMutex;

		if(nullptr == s_Instance) {
			std::lock_guard<std::mutex> lock(s_InstanceMutex);
			if(nullptr == s_Instance) {
				Resource::RHIType rhiType = Resource::GetConfigManager()->GetRHIType();
				if(Resource::RHI_Vulkan == rhiType) {
					s_Instance.reset(new RHIVulkan());
				}
				else {
					LOG_ERROR("Failed to initialize RHI!");
				}
			}
		}

		return s_Instance.get();
	}
}