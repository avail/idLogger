#include "Utils.h"

const char* va(const char* string, ...)
{
	static thread_local int currentBuffer;
	static thread_local char* buffer;

	if (!buffer)
	{
		buffer = new char[BUFFER_COUNT * BUFFER_LENGTH];
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vsnprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		exit(1);
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}


const wchar_t* va(const wchar_t* string, ...)
{
	static thread_local int currentBuffer;
	static thread_local wchar_t* buffer;

	if (!buffer)
	{
		buffer = new wchar_t[BUFFER_COUNT * BUFFER_LENGTH];
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vswprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		exit(1);
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}

void GetDesktopResolution(int& x, int& y)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	x = desktop.right;
	y = desktop.bottom;
}

int strcmp(const char* str1, const char* str2, bool csensitive)
{
	return (csensitive ? ::strcmp(str1, str2) : ::_stricmp(str1, str2));
}

int Compare(const std::string& str1, const std::string& str2, bool case_sensitive)
{
	if (str1.length() == str2.length())
		return strcmp(str1.c_str(), str2.c_str(), case_sensitive);

	return (str1.length() < str2.length() ? -1 : 1);
}

bool ToBool(const std::string& s)
{
	if (s.size() == 1) return s.front() != '0';
	return !!Compare(s, "false", false);
}