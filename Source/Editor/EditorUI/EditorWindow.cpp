#include "EditorWindow.h"
#include "Resource/Config/Config.h"
#include "Resource/Assets/Assets.h"
#include "Render/Scene/RenderScene.h"
#include "EditorContext.h"
#include "EngineContext.h"
#include "Render/Camera/Camera.h"

namespace Editor {


	void FPSWindow::OnWindow() {
		ImGui::Text("%.1f",Editor::Context()->FPSRec()->GetFPS());
	}

	void ObjectsWindow::OnWindow() {
		Engine::RenderScene* scene = Engine::RenderScene::GetDefaultScene();
		auto& objects = scene->GetRenderObjects();
		for (uint32 i = 0; i < objects.size(); ++i) {
			ImGui::Selectable(objects[i]->Name());
		}
	}

	void DetailWindow::OnWindow() {
		ImGui::Text("Details");
	}

	void AssetsWindow::OnWindow() {
		ImGui::Text("Assets");
	}



	void MoveCamera(Engine::Camera* camera, float x, float y, float z, bool local) {
		Math::FVector3 eye = camera->GetView().Eye;
		Math::FVector3 at = camera->GetView().At;
		Math::FVector3 up = camera->GetView().Up;
		Math::FVector3 delta;
		if (local) {
			Math::FVector3 forward = at - eye;
			Math::FVector3 right = Math::FVector3::Cross(up, forward);
			delta = right * x + forward * z + up * y;
		}
		else {
			delta = { x, y, z };
		}
		eye += delta;
		at += delta;
		camera->SetView(eye, at, up);
	}

	void RotateCamera(Engine::Camera* camera, float x, float y, float z, bool localSpace) {
		Math::FVector3 eye = camera->GetView().Eye;
		Math::FVector3 at = camera->GetView().At;
		Math::FVector3 tempUp = camera->GetView().Up;
		if (localSpace) {
			Math::FVector3 forward = at - eye;
			forward.Normalize();
			Math::FVector3 right = Math::FVector3::Cross(tempUp, forward);
			right.Normalize();
			Math::FVector3 up = Math::FVector3::Cross(forward, right);
			Math::FQuaternion rotateQuat =
				Math::FQuaternion::AngleAxis(z * 0.004f, forward) *
				Math::FQuaternion::AngleAxis(y * 0.004f, up) *
				Math::FQuaternion::AngleAxis(x * 0.004f, right);
			forward = rotateQuat.RotateVector3(forward);
			at = eye + forward;
		}
		else {
		}
		camera->SetView(eye, at, tempUp);
	}

	void ViewportWindow::CameraControl() {
		ImGuiMouseButton btn = ImGuiMouseButton_Right;
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(btn)) {
			m_MouseDown = true;
		}
		if (ImGui::IsMouseReleased(btn)) {
			m_MouseDown = false;
			m_LastX = 0.0f;
			m_LastY = 0.0f;
		}

		//rotate
		if(m_MouseDown) {
			auto pos = ImGui::GetMousePos();
			float x = pos.x, y = pos.y;
			if (m_LastX == 0.0f && m_LastY == 0.0f) {
				m_LastX = x;
				m_LastY = y;
			}
			else {
				float dX = x - m_LastX;
				float dY = y - m_LastY;
				RotateCamera(Engine::RenderScene::GetDefaultScene()->GetMainCamera(), dY, dX, 0.0f, true);
				m_LastX = x;
				m_LastY = y;				
			}
		}

		//move
		{
			int x = ImGui::IsKeyDown(ImGuiKey_D) - ImGui::IsKeyDown(ImGuiKey_A);
			int z = ImGui::IsKeyDown(ImGuiKey_W) - ImGui::IsKeyDown(ImGuiKey_S);
			int y = ImGui::IsKeyDown(ImGuiKey_E) - ImGui::IsKeyDown(ImGuiKey_Q);
			if (x || y || z) {
				MoveCamera(Engine::RenderScene::GetDefaultScene()->GetMainCamera(), (float)x * 0.004f, (float)y * 0.004f, (float)z * 0.004f, true);
			}
		}
	}

	void ViewportWindow::Update(){
		// if this window is hided, disable the main pass rendering
		if(!m_Show && m_ViewportShow) {
			m_ViewportShow = false;
			Engine::Context()->Renderer()->SetRenderArea({ 0,0,0,0 });
		}
	}

	void ViewportWindow::OnWindow() {
		CameraControl();
		auto size = ImGui::GetWindowSize();
		auto pos = ImGui::GetWindowPos();
		if (!m_ViewportShow || !Math::FloatEqual(pos.x, m_Viewport.x) || !Math::FloatEqual(pos.y, m_Viewport.y) || !Math::FloatEqual(size.x, m_Viewport.w) || !Math::FloatEqual(size.y, m_Viewport.h)) {
			m_Viewport.x = static_cast<int32>(pos.x);
			m_Viewport.y = static_cast<int32>(pos.y);
			m_Viewport.w = static_cast<uint32>(size.x);
			m_Viewport.h = static_cast<uint32>(size.y);
			Engine::Context()->Renderer()->SetRenderArea(m_Viewport);
			Editor::Context()->CurrentScene()->GetMainCamera()->SetAspect(size.x / size.y);
			m_ViewportShow = true;
		}
	}
}