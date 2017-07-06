#include "../idLogger.h"
#include <stdlib.h>

int cgGetProgramStringHk(int a1, int a2)
{
	// call original function
	((int(__cdecl*)(int, int))0xDBB8EC)(a1, a2);
}

static InitFunction amdHookFunc([]()
{
	/*_putenv("regal_trace=1");
	_putenv("REGAL_TRACE_FILE=e:\\Games\\ID6AA\\plugins\\regal.trace");
	_putenv("regal_log_file=e:\\Games\\ID6AA\\plugins\\regal.log");
	_putenv("regal_statistics=1");
	_putenv("regal_frame_statistics=1");
	_putenv("regal_force_emulation=1");*/
	
	// replace opengl32.dll with regal32.dll
	// regal32 is an opengl debugging library
	// it seems to force a bunch of hooks as well which sort of made it work initially.
	// but we do not want to ship a debugging library with a fix,
	// so we will fix it ourselves below.
	/*strcpy((CHAR*)0xE781A0, "regal32.dll");
	wcscpy((WCHAR*)0xE9F400, L"regal32.dll");*/

	// testing to see whether this change makes it black again
	//strcpy((CHAR*)0xE75580, "qlProgramParameters4fvNV");

	// force env vertex program presence
	injector::MakeNOP(0xA15DF0 + 0x522B, 2);

	// force volatile texture (driver bug(?) check) presence
	injector::MakeNOP(0xA15DF0 + 0x5667, 2);

	// hook cgGetProgramString
	// doesn't seem to be needed for base game rendering,
	// but will hook and emulate eventually anways, as it
	// *might* fix some issues.
	//*(int*)0xDBB8EC = (int)cgGetProgramStringHk;
});