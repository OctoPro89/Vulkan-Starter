#pragma once
#include <windows.h>

#define VK_USE_PLATFORM_WIN32_KHR

/* ---- INCLUDE THIS AFTER vulkan.h OR DEFINES WILL NOT BE CORRECT ---- */

typedef struct {
#ifdef VK_USE_PLATFORM_WIN32_KHR
	HINSTANCE HInstance;
	HWND HWnd;
#elif defined VK_USE_PLATFORM_XLIB_KHR
	Display* Dpy;
	Window Window;
#elif defined VK_USE_PLATFORM_XCB_KHR
	xcb_connection_t* Connection;
	xcb_window_t Window;
#endif
} WindowPerameters;