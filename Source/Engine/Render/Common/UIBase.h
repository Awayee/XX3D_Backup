#pragma once

namespace Engine {
	class UIBase {
	public:
		virtual	~UIBase() {}
		virtual void Tick() = 0;
	};
}