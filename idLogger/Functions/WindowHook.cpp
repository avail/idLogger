#include "../idLogger.h"

DWORD g_windowStyle = WS_VISIBLE | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

FILE* windowLog;

int g_x = 0;
int g_y = 0;

HWND hwndWindowA;
HWND hwndWindowW;

HWND WINAPI CreateWindowExAHk(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	fprintf(windowLog, "CreateWindowExAHooked: lpClassName %s lpWindowName %s x %i y %i nWidth %i nHeight %i dwStyle %p\n", lpClassName, lpWindowName, x, y, nWidth, nHeight, dwStyle);
	fflush(windowLog);

	int rx, ry;
	GetDesktopResolution(rx, ry);

	g_x = (rx / 2) - (nWidth / 2);
	g_y = (ry / 2) - (nHeight / 2);

	if (lpWindowName)
	{
		dwStyle = g_windowStyle;
	}

	HWND thisWindow = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, g_x, g_y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	if (lpWindowName)
	{
		hwndWindowA = thisWindow;
	}

	return thisWindow;
}

HWND WINAPI CreateWindowExWHk(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	fprintf(windowLog, "CreateWindowExWHooked: lpClassName %s lpWindowName %s x %i y %i nWidth %i nHeight %i dwStyle %p\n", lpClassName, lpWindowName, x, y, nWidth, nHeight, dwStyle);
	fflush(windowLog);

	int rx, ry;
	GetDesktopResolution(rx, ry);

	g_x = (rx / 2) - (nWidth / 2);
	g_y = (ry / 2) - (nHeight / 2);

	if (lpWindowName)
	{
		dwStyle = g_windowStyle;
	}

	HWND thisWindow = CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, g_x, g_y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	if (lpWindowName)
	{
		hwndWindowW = thisWindow;
	}

	return thisWindow;
}

BOOL WINAPI AdjustWindowRectHk(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
{
	fprintf(windowLog, "AdjustWindowRectHooked: lpRect %i dwStyle %p bMenu %i\n", lpRect, dwStyle, bMenu);
	fflush(windowLog);

	dwStyle = g_windowStyle;

	return AdjustWindowRect(lpRect, dwStyle, bMenu);
}

BOOL WINAPI SetWindowPosHk(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	fprintf(windowLog, "SetWindowPosHooked: hWnd %p X %i Y %i cx %i cy %i\n", hWnd, X, Y, cx, cy);
	fflush(windowLog);

	if (hwndWindowA == hWnd || hwndWindowW == hWnd)
	{
		X = g_x;
		Y = g_y;
	}

	return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

LONG __stdcall ChangeDisplaySettingsHk(DEVMODEA *lpDevMode, DWORD dwFlags)
{
	fprintf(windowLog, "ChangeDisplaySettingsHooked: lpDevMode %i dwFlags %p\n", lpDevMode, dwFlags);
	fflush(windowLog);

	lpDevMode = NULL; // retain original changes instead of applying Initial D's modified values

	return ChangeDisplaySettingsA(lpDevMode, dwFlags);
}

static InitFunction windowHookFunc([]()
{
	windowLog = fopen("idLogger-window.txt", "w");

	*(HWND*)0xDBB774 = (HWND)CreateWindowExAHk;
	*(HWND*)0xDBB754 = (HWND)CreateWindowExWHk;
	*(BOOL*)0xDBB7A4 = (BOOL)AdjustWindowRectHk;
	*(BOOL*)0xDBB7A8 = (BOOL)SetWindowPosHk;
	*(LONG*)0xDBB7BC = (LONG)ChangeDisplaySettingsHk;
});