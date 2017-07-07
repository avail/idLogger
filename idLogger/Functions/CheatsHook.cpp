#include "../idLogger.h"

int hookedConnectToNet()
{
	return 0;
}

static InitFunction cheatsFunc([]()
{
	// fullspec
	// dumb tired hooking attempt that never worked ;p
	/*injector::MakeNOP(0x4CCE20, 2);
	((int(__thiscall*)(int))0x4CCE00)(0);*/

	// fail network init instantly
	injector::MakeCALL(0x9B1C70 + 0x3B8, hookedConnectToNet);
});