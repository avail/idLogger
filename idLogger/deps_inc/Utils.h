#pragma once

#include <windows.h>
#include <string>

#define BUFFER_COUNT 8
#define BUFFER_LENGTH 32768

const char* va(const char* string, ...);
const wchar_t* va(const wchar_t* string, ...);
void GetDesktopResolution(int& x, int& y);
int Compare(const std::string& str1, const std::string& str2, bool case_sensitive);
bool ToBool(const std::string& s);