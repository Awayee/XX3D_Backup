#pragma once
#include "Core/Math/Math.h"

namespace Engine {
	class DirectionalLight {
	private:
		Math::FVector3 m_LightDir;
		Math::FVector3 m_LightColor;
	public:
		DirectionalLight() = default;
		void SetDir(const Math::FVector3& dir) { m_LightDir = dir; };
		void SetColor(const Math::FVector3& color) { m_LightColor = color; };
		const Math::FVector3& GetLightDir() const { return m_LightDir; }
		const Math::FVector3& GetLightColor() const { return m_LightColor; }
	};
}