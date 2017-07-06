#include "../idLogger.h"

static InitFunction cheatsFunc([]()
{
	// fullspec
	// dumb tired hooking attempt that never worked ;p
	/*injector::MakeNOP(0x4CCE20, 2);
	((int(__thiscall*)(int))0x4CCE00)(0);*/
});