#pragma once
#include "RHI.h"

#define GET_RHI(name) RHI::RHIInstance* name = RHI::GetInstance()

namespace RHI {
	RHIInstance* GetInstance();
}