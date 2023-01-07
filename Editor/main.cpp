#include "Core/macro.h"
#include "Public/Editor.h"
#include "Engine/Engine.h"

#include "Resource/ResourcesMacro.h"

#include <unordered_map>

int main() {
	// Run Editor
	{
		LOG("Editor Mode");
		Engine::XXEngine engine{};
		Editor::XXEditor editor(&engine);
		editor.EditorRun();
	}

	system("pause");
	return 0;
}