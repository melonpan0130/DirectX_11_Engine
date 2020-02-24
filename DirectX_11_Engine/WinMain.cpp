#include "Engine.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance
					, _In_opt_ HINSTANCE hPrevInstance
					, _In_ LPWSTR lpCmdLine
					, _In_ int nCmdShow)
{
	Engine engine;
	engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600);
	while (engine.ProcessMessages() == true)
	{
		printf("hello");
		OutputDebugStringA("hello2");
		engine.Update();
	}
	return 0;
}