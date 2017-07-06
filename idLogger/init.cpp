#include "idLogger.h"

linb::ini config;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		
		if (!config.load_file("idLogger-config.ini"))
		{
			MessageBoxA(NULL, "Failed to open idLogger-config.ini", "Oshibka", MB_OK);
			std::_Exit(0);
		}

		InitFunction::RunFunctions();
	}

	return TRUE; // false
}