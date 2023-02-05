#include "Core/macro.h"
#include "Public/Editor.h"
#include "Engine.h"

int main() {
	// Run Editor
	{
		LOG("Editor Mode");
		Engine::XXEngine engine{};
		Editor::XXEditor editor(&engine);
		editor.EditorRun();
	}

	//system("pause");
	return 0;
}