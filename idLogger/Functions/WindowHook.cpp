#include "../idLogger.h"

DWORD g_windowStyle;

FILE* windowLog;

int g_x = 0;
int g_y = 0;

int g_width = 0;
int g_height = 0;

bool enableWindowLog = false;
bool enableBorderless = false;

HWND hwndWindowA;
HWND hwndWindowW;

HWND WINAPI CreateWindowExAHk(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (enableWindowLog)
	{
		fprintf(windowLog, "CreateWindowExAHooked: lpClassName %s lpWindowName %s x %i y %i nWidth %i nHeight %i dwStyle %p\n", lpClassName, lpWindowName, x, y, nWidth, nHeight, dwStyle);
		fflush(windowLog);
	}

	if (lpWindowName)
	{
		dwStyle = g_windowStyle;
	}

	HWND thisWindow = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, g_x, g_y, g_width, g_height, hWndParent, hMenu, hInstance, lpParam);

	if (lpWindowName)
	{
		hwndWindowA = thisWindow;
	}

	return thisWindow;
}

HWND WINAPI CreateWindowExWHk(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (enableWindowLog)
	{
		fprintf(windowLog, "CreateWindowExWHooked: lpClassName %s lpWindowName %s x %i y %i nWidth %i nHeight %i dwStyle %p\n", lpClassName, lpWindowName, x, y, nWidth, nHeight, dwStyle);
		fflush(windowLog);
	}

	if (lpWindowName)
	{
		dwStyle = g_windowStyle;
	}

	HWND thisWindow = CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, g_x, g_y, g_width, g_height, hWndParent, hMenu, hInstance, lpParam);

	if (lpWindowName)
	{
		hwndWindowW = thisWindow;
	}

	return thisWindow;
}

BOOL WINAPI AdjustWindowRectHk(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
{
	if (enableWindowLog)
	{
		fprintf(windowLog, "AdjustWindowRectHooked: lpRect %i dwStyle %p bMenu %i\n", lpRect, dwStyle, bMenu);
		fflush(windowLog);
	}

	dwStyle = g_windowStyle;

	return AdjustWindowRect(lpRect, dwStyle, bMenu);
}

BOOL WINAPI SetWindowPosHk(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	if (enableWindowLog)
	{
		fprintf(windowLog, "SetWindowPosHooked: hWnd %p X %i Y %i cx %i cy %i uFlags %i\n", hWnd, X, Y, cx, cy);
		fflush(windowLog);
	}

	if (hwndWindowA == hWnd || hwndWindowW == hWnd)
	{
		X = g_x;
		Y = g_y;
	}

	return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

LONG __stdcall ChangeDisplaySettingsHk(DEVMODEA *lpDevMode, DWORD dwFlags)
{
	if (enableWindowLog)
	{
		fprintf(windowLog, "ChangeDisplaySettingsHooked: lpDevMode %i dwFlags %p\n", lpDevMode, dwFlags);
		fflush(windowLog);
	}

	lpDevMode = NULL; // retain original changes instead of applying Initial D's modified values

	if (enableBorderless)
		return DISP_CHANGE_SUCCESSFUL;
	else
		return ChangeDisplaySettingsA(lpDevMode, dwFlags);
}

BOOL WINAPI UpdateWindowHk(HWND hWnd)
{
	return true;
}

static InitFunction windowHookFunc([]()
{
	bool enableWindowed = ToBool(config["WindowHooks"]["EnableWindowedModeHook"]);
	enableBorderless = ToBool(config["WindowHooks"]["BorderlessModeHook"]);
	enableWindowLog = ToBool(config["Logging"]["EnableWindowLog"]);
	bool enableResolutionPatches = ToBool(config["Logging"]["EnableResolutionPatches"]);

	linb::ini gameConfig;
	gameConfig.read_file("../InitialD.ini");

	g_width = atoi(gameConfig[" Graphics "]["Width"].c_str());
	g_height = atoi(gameConfig[" Graphics "]["Height"].c_str());

	if (enableWindowLog)
	{
		windowLog = fopen("idLogger-window.txt", "w");
	}

	if (enableBorderless)
	{
		// borderless windowed mode
		g_windowStyle = WS_POPUP | WS_VISIBLE | WS_SYSMENU;
		GetDesktopResolution(g_width, g_height);

		g_x = 0;
		g_y = 0;
	}
	else
	{
		// regular windowed mode
		g_windowStyle = WS_VISIBLE | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

		int rx, ry;
		GetDesktopResolution(rx, ry);

		g_x = (rx / 2) - (g_width / 2);
		g_y = (ry / 2) - (g_height / 2);
	}

	if (enableWindowed || enableBorderless)
	{
		*(HWND*)0xDBB774 = (HWND)CreateWindowExAHk;
		*(HWND*)0xDBB754 = (HWND)CreateWindowExWHk;
		*(BOOL*)0xDBB7A4 = (BOOL)AdjustWindowRectHk;
		*(BOOL*)0xDBB7A8 = (BOOL)SetWindowPosHk;
		*(LONG*)0xDBB7BC = (LONG)ChangeDisplaySettingsHk;
		*(BOOL*)0xDBB7B4 = (BOOL)UpdateWindowHk;
	}

	if (enableResolutionPatches)
	{
		// height
		injector::WriteMemory(0x768A20 + 0x24E + 1, g_height, true);
		injector::WriteMemory(0x768DC5, g_height, true);
		injector::WriteMemory(0x768A20 + 0xBB7 + 1, g_height, true);
		injector::WriteMemory(0x76C8A8, g_height, true);
		injector::WriteMemory(0x76D129, g_height, true);
		injector::WriteMemory(0x7B4400 + 0x44 + 1, g_height, true);
		injector::WriteMemory(0x7B4640 + 0x5B + 1, g_height, true);
		injector::WriteMemory(0x7B4640 + 0x71 + 1, g_height, true);
		injector::WriteMemory(0x83D82E, g_height, true);
		injector::WriteMemory(0x865780 + 0x44 + 1, g_height, true);
		injector::WriteMemory(0x86D271, g_height, true);
		injector::WriteMemory(0x87EE20, g_height, true);

		// width
		injector::WriteMemory(0x768A20 + 0x253 + 1, g_width, true);
		injector::WriteMemory(0x768DBA, g_width, true);
		injector::WriteMemory(0x768A20 + 0xBBC + 1, g_width, true);
		injector::WriteMemory(0x76C8A3, g_width, true);
		injector::WriteMemory(0x76D122, g_width, true);
		injector::WriteMemory(0x7B4400 + 0x49 + 1, g_width, true);
		injector::WriteMemory(0x7B4640 + 0x60 + 1, g_width, true);
		injector::WriteMemory(0x7B4640 + 0x76 + 1, g_width, true);
		injector::WriteMemory(0x83D825, g_width, true);
		injector::WriteMemory(0x865780 + 0x49 + 1, g_width, true);
		injector::WriteMemory(0x86D269, g_width, true);
		injector::WriteMemory(0x87EE18, g_width, true);

		// remove limits in reading resolution from configuration
		// less than 640, bigger than 1600 (width)
		injector::MakeNOP(0x9B4C30 + 0x133, 2);
		injector::WriteMemory<BYTE>(0x9B4C30 + 0x13A, 0xEB, true);

		// some resolutions are not divisble by 8, so skip that check too
		injector::WriteMemory<BYTE>(0x9B4C30 + 0x144, 0xEB, true);

		// less than 400, bigger than 1200 (height)
		injector::MakeNOP(0x9B4C30 + 0x16F, 2);
		injector::WriteMemory<BYTE>(0x9B4C30 + 0x177, 0xEB, true);

		// some resolutions are not divisble by 8, so skip that check too
		injector::WriteMemory<BYTE>(0x9B4C30 + 0x181, 0xEB, true);
	}

	if (enableBorderless)
	{
		// set ppfd flag to allow for composite
		injector::WriteMemory<WORD>(0xAADB54, 0x9025, true);
	}
});