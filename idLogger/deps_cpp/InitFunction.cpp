#include <InitFunction.h>

std::forward_list<void (*)()> g_initFunctions;

void InitFunction::RunFunctions()
{
	for (auto& it : g_initFunctions)
	{
		(*it)();
	}
}