#include "../Public/Editor.h"
#include "Resource/Config/Config.h"
#include "Resource/Assets/Assets.h"
#include "Render/Camera/Camera.h"

namespace Editor {

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
			delta = {x, y, z};
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
				Math::FQuaternion::AngleAxis(z * 0.01f, forward) *
				Math::FQuaternion::AngleAxis(y * 0.01f, up) *
				Math::FQuaternion::AngleAxis(x * 0.01f, right);
			forward = rotateQuat.RotateVector3(forward);
			at = eye + forward;
		}
		else {
		}
		camera->SetView(eye, at, tempUp);
	}

	void XXEditor::LoadMesh(){
		AExternalMeshAsset meshAsset = AssetMgr::LoadAsset<AExternalMeshAsset>("keqing/keqing.glb");
		m_Ms.reset(new Engine::RenderMesh(meshAsset.Primitives, nullptr));
	}

	void XXEditor::RegisterEvents(){
		auto windowSys = m_Engine->Window();
		windowSys->RegisterOnKeyFunc([this](Engine::EKeyCode keyCode, Engine::EInputType inputType) {
			this->OnKeyEvent(keyCode, inputType);
		});
		windowSys->RegisterOnMouseButtonFunc([this](Engine::EButtonCode btnCode, Engine::EInputType inputType) {
			this->OnMouseButtonEvent(btnCode, inputType);
		});
		windowSys->RegisterOnCursorPosFunc([this](double x, double y) {
			this->OnMouseMoveEvent((float)x, (float)y);
		});
	}

	void XXEditor::InitResources(){
		m_MainScene = Engine::RenderScene::GetDefaultScene();
		m_MainCamera = m_MainScene->GetMainCamera();
		LoadMesh();
	}

	void XXEditor::OnKeyEvent(Engine::EKeyCode keyCode, Engine::EInputType inputType){
		if (inputType == Engine::EInputType::REPEAT || inputType == Engine::EInputType::PRESS) {
			float z = (float)(keyCode == Engine::EKeyCode::W) * 1.0f + (float)(keyCode == Engine::EKeyCode::S) * -1.0f;
			float y = (float)(keyCode == Engine::EKeyCode::E) * 1.0f + (float)(keyCode == Engine::EKeyCode::Q) * -1.0f;
			float x = (float)(keyCode == Engine::EKeyCode::D) * 1.0f + (float)(keyCode == Engine::EKeyCode::A) * -1.0f;
			if ( 0.0f != z || 0.0f != y || 0.0f !=x) {
				MoveCamera(m_MainCamera, x * 0.1f, y * 0.1f, z * 0.1f, true);
			}
		}
	}

	void XXEditor::OnMouseButtonEvent(Engine::EButtonCode btnCode, Engine::EInputType inputType){
		if (inputType == Engine::EInputType::PRESS && btnCode == Engine::EButtonCode::RIGHT) {
			m_MousePressed = true;
		}
		else if (inputType == Engine::EInputType::RELEASE && btnCode == Engine::EButtonCode::RIGHT) {
			m_MousePressed = false;
			m_LastX = 0.0f; m_LastY = 0.0f;
		}
	}

	void XXEditor::OnMouseMoveEvent(float x, float y){
		if (!m_MousePressed) return;
		if (m_LastX == 0.0f && m_LastY == 0.0f) {
			m_LastX = x;
			m_LastY = y;
			return;
		}
		float dX = x - m_LastX;
		float dY = y - m_LastY;
		RotateCamera(m_MainCamera, dY, dX, 0.0f, true);
		m_LastX = x;
		m_LastY = y;
	}

	void XXEditor::UpdateUI()
	{
	}

	XXEditor::XXEditor(Engine::XXEngine* engine){
		m_Engine = engine;
		m_EditorUI.reset(new EditorUI(engine));
		URect2D rect{ 0, 0, GetConfig()->GetWindowSize().w, GetConfig()->GetWindowSize().h };
		engine->Renderer()->SetRenderArea(std::move(rect));
		engine->Renderer()->InitUIPass(m_EditorUI.get());
		engine->Renderer()->SetEnable(true);
		RegisterEvents();
		InitResources();
	}

	XXEditor::~XXEditor() {
	}

	void XXEditor::EditorRun(){
		while(true) {
			if(!m_Engine->Tick()) {
				return;
			}
		}
	}
}