#pragma once
#include "Objects/Public/Timer.h"
#include "Core/Public/TSingleton.h"

namespace Editor {
	class EditorTimer : public TSingleton<EditorTimer>, public Engine::CTimer {};
}