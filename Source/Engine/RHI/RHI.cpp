#include "RHI.h"
#include "RHIVulkan/RHIVulkan.h"
#include "Resource/Config/Config.h"
#include "Core/macro.h"
#include "Core/Memory/SmartPointer.h"
#include "Core/Concurrency/Concurrency.h"

using namespace Engine;
namespace RHI{

	RHIInstance* GetInstance() {
		static TUniquePtr<RHIInstance> s_Instance;
		static Mutex s_InstanceMutex;

		if(nullptr == s_Instance) {
			MutexLock lock(s_InstanceMutex);
			if(nullptr == s_Instance) {
				ERHIType rhiType = GetConfig()->GetRHIType();
				if(RHI_Vulkan == rhiType) {
					s_Instance.reset(new RHIVulkan());
				}
				else {
					ERROR("Failed to initialize RHI!");
				}
			}
		}

		return s_Instance.get();
	}
}
