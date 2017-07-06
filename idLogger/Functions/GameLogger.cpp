#include "../idLogger.h"

FILE* gameLog;

void hookedPrint(const char* str, ...)
{
	char* buf = NULL;
	size_t size;

	va_list args;
	va_start(args, str);
	
	size = _vscprintf(str, args);
	buf = (char*)malloc(size + 1);
	vsnprintf(buf, size + 1, str, args);

	va_end(args);

	fprintf(gameLog, buf);

	free(buf);

	fprintf(gameLog, "\n");
	fflush(gameLog);
}

void hookedFprintf(void* a1, const char* a2, ...)
{
	char* buf = NULL;
	size_t size;

	va_list args;
	va_start(args, a2);

	size = _vscprintf(a2, args);
	buf = (char*)malloc(size + 1);
	vsnprintf(buf, size + 1, a2, args);

	va_end(args);

	fprintf(gameLog, buf);

	free(buf);

	fprintf(gameLog, "\n");
	fflush(gameLog);
}

static InitFunction gameLoggerFunc([]()
{
	bool enableWindowLog = ToBool(config["Logging"]["EnableGameLog"]);

	if (enableWindowLog)
	{
		// open logging stuff
		gameLog = fopen("idLogger-game.txt", "w");

		// nullsub_34 (debug logging)
		injector::MakeJMP(0x4075E0, hookedPrint);
		injector::MakeNOP(0xAA3293, 6);
		injector::MakeCALL(0xAA3293, hookedFprintf);
		injector::MakeNOP(0xAC22B3, 6);
		injector::MakeCALL(0xAC22B3, hookedFprintf);
	}
});