#pragma once
#include "RHI.h"

#define RHI_INSTANCE RHI::GetInstance()
#define GET_RHI(name) RHI::RHIInstance* name = RHI_INSTANCE

namespace RHI {
	RHIInstance* GetInstance();
}