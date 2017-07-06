#include <forward_list>

extern std::forward_list<void (*)()> g_initFunctions;

class InitFunction
{
public:

	InitFunction(void (* callback)())
	{
		g_initFunctions.push_front(callback);
	}

	static void RunFunctions();
};
