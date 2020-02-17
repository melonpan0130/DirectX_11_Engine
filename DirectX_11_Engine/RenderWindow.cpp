#include "WindowContainer.h"

bool RenderWindow::Initialize(WindowContainer* pWindowContainer, HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
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
		, pWindowContainer
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

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			return 0;
		}
		default:
		{
			// retrieve ptr to window class
			WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			// forward message to window class handler
			return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr) // Sanity check
		{
			ErrorLogger::Log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
			exit(-1);
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	/*
	case WM_CHAR:
	{
		unsigned char letter = static_cast<unsigned char>(wParam);
		return 0;
	}
	case WM_KEYDOWN:
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		return 0;
	}
	*/
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void RenderWindow::RegisterWindowClass()
{
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup; // Pointer to Window Proc function for handling messages from this window.
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hInstance; // Handle to the instance that contains the Window Procedure.
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->window_class_wide.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);
}
