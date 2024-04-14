#include "AssetView.h"
#include "Functions/Public/EditorLevelMgr.h"
#include "EditorUI/Public/EditorUIMgr.h"
#include "EditorUI/Public/EditorWindow.h"

namespace Editor {
	FolderAssetView::FolderAssetView(FolderNode* node): m_Node(node) {
		m_ID = m_Node->GetID();
	}

	const String& FolderAssetView::GetName() {
		return m_Node->GetName();
	}

	bool FolderAssetView::IsFolder() {
		return true;
	}

	void FolderAssetView::Open() {
	}

	void FolderAssetView::Save() {
	}

	void FolderAssetView::OnDrag() {
		ImGui::SetDragDropPayload("Folder", m_Node, sizeof(FolderNode));
	}

	FileAssetView::FileAssetView(FileNode* node): m_Node(node) {
		m_ID = m_Node->GetID();
		m_Node->GetAsset<Engine::AUnknownAsset>();
	}

	const String& FileAssetView::GetName() {
		return m_Node->GetName();
	}

	bool FileAssetView::IsFolder() {
		return false;
	}

	void FileAssetView::OnDrag() {
		ImGui::SetDragDropPayload("File", m_Node, sizeof(FileNode));
	}

	void MeshAssetView::Open() {
		auto f = [node=m_Node, asset=m_Asset]() {
			for(auto& primitive: asset->Primitives) {

				//material
				File::FPath primitivePath = primitive.BinaryFile;
				ImGui::Text(primitivePath.stem().string().c_str()); ImGui::SameLine();
				if(ImGui::BeginDragDropTarget()) {
					ImGui::Button(primitive.MaterialFile.empty() ? "None" : primitive.MaterialFile.c_str());
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("File")) {
						ASSERT(payload->DataSize == sizeof(FileNode), "");
						const FileNode* fileNode = reinterpret_cast<const FileNode*>(payload->Data);
						primitive.MaterialFile = fileNode->GetPathStr();
						node->Save();
						EditorLevelMgr::Instance()->ReloadLevel();
					}
					ImGui::EndDragDropTarget();
				}
				else {
					ImGui::Button(primitive.MaterialFile.empty() ? "None" : primitive.MaterialFile.c_str());
				}
			}
			if(ImGui::Button("Save")) {
				node->Save();
				EditorLevelMgr::Instance()->ReloadLevel();
			}
		};
		EditorUIMgr::Instance()->AddWindow("MeshViewer", std::move(f), ImGuiWindowFlags_NoDocking);
	}

	void TextureAssetView::Open() {
	}

	void LevelAssetView::Open() {
		EditorLevelMgr::Instance()->LoadLevel(m_Asset, m_Node->GetPath());
	}

	void LevelAssetView::Save() {
	}
}
