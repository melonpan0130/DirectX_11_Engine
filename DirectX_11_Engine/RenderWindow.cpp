#include "RenderWindow.h"

bool RenderWindow::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	this->window_title = window_title;
	this->window_title_wide = StringConverter::StringToWide(this->window_title);
	this->window_class = window_class;
	this->window_class_wide = StringConverter::StringToWide(this->window_class);
	// wide string representation of class name

	this->RegisterWindowClass();

	this->handle = CreateWindowEx(0 //Extended windows style
		, this->window_class_wide.c_str() // Window class name
		, this->window_title_wide.c_str() // Window title
		, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU // Windows style
		, 0 // Window X position
		, 0 // Window Y position
		, this->width // Window width
		, this->height // Window height
		, NULL // Handle to parent of this window. - It has no parent window.
		, NULL // Handle to menu or child window identifier. - It has no menu.
		, this->hInstance // Handle to the instance of module to be used with this window.
		, nullptr
	); // Param to create window

	if (this->handle == NULL)
	{
		ErrorLogger::Log(GetLastError(), "CreateWindowEx Failed for window: " + this->window_title);
		return false;
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return true;
}

bool RenderWindow::ProcessMessages()
{
	// Handle the windows messages.
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG)); // Initialize the message structure.

	if (PeekMessage(&msg // Where to store message (if one exites)
		, this->handle // Handle to window we are checking messages for
		, 0 // Minimum Filter Msg Value - We are not filtering for specific messages.
		, 0 // Maximum Filter Msg Value
		, PM_REMOVE)) // Remove Message after capturing it via PeekMessage.
	{
		TranslateMessage(&msg); // Translate message from virtual key messages into character messages so we can
		DispatchMessage(&msg); // Dispatch message to our Window Proc for this window.
	}

	// Check if the window was closed.
	if (msg.message == WM_NULL)
	{
		if (!IsWindow(this->handle))
		{
			this->handle = NULL; // Message processing loop takes care of destroying this window.
			UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
			return false;
		}
	}
	return true;
}

RenderWindow::~RenderWindow()
{
	if (this->handle != NULL)
	{
		UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
		DestroyWindow(handle);
	}
}

void RenderWindow::RegisterWindowClass()
{
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->window_class_wide.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);
}
