#pragma once
#include "Core/Math/Math.h"

namespace Engine {
	enum EProjectiveType {
		PROJECT_PERSPECTIVE,
		PROJECT_ORTHO
	};

	class Camera {
	private:
		// transform
		Math::FMatrix4x4 m_ViewMatrix;

		// projection
		EProjectiveType m_ProjType;
		float m_Aspect;
		float m_Near;
		float m_Far;
		union {
			float m_Fov;
			float m_Height;
		};

		Math::FMatrix4x4 m_ProjectMatrix;
		Math::FMatrix4x4 m_ViewProjectMatrix;
		void UpdateProjectMatrix();
		void UpdateViewMatrix();

	public:
		Camera() = default;
		Camera(EProjectiveType type, float aspect, float zNear, float zFar, float param);
		Camera(const Camera&) = default;
		Camera(Camera&&) = default;
		float GetNear() const { return m_Near; }
		void  SetNear(float n) { m_Near = n; UpdateProjectMatrix(); }
		float GetFar() const { return m_Far; }
		void  SetFar(float f) { m_Far = f; UpdateProjectMatrix(); }
		float GetAspect() const { return m_Aspect; }
		void  SetAspect(float a) { m_Aspect = a; UpdateProjectMatrix(); }
		float GetFov() const { return PROJECT_PERSPECTIVE == m_ProjType ? m_Fov : 0.0f; }
		void  SetFov(float fov) { if (PROJECT_PERSPECTIVE == m_ProjType) { m_Fov = fov; UpdateProjectMatrix(); } }
		float GetHeight() const { return PROJECT_ORTHO == m_ProjType ? m_Height: 0.0f; }
		void  SetHeight(float height) { if (PROJECT_ORTHO == m_ProjType) { m_Height = height; UpdateProjectMatrix(); } }
		const Math::FMatrix4x4& GetViewMatrix() { return m_ViewMatrix; }
		const Math::FMatrix4x4& GetProjectMatrix() { return m_ProjectMatrix; }
		const Math::FMatrix4x4& GetViewProjectMatrix() { return m_ViewProjectMatrix; }
		~Camera();
	};
}