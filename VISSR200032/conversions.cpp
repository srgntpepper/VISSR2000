#include "stdAfx.h"

/*********************************************************************************
	Conversions - Only necessary for Unicode Char Set

	The following functions are meant to convert types to the correct sizes for
	compatibility reason.  Conversion so far are LPSTR or char* to LPCWSTR, char*
	to wchar_t*, and wchar_t* to char*.
*********************************************************************************/
//This function is used to convert LPSTR or char* to LPCWSTR - Chris Davis
std::wstring ConvertToLPCWSTR(const char* charArray) {
	int size_needed = MultiByteToWideChar(CP_ACP, 0, charArray, -1, NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, &wstrTo[0], size_needed);
	return wstrTo;
}

//This function is used to convert a char* to a wchar_t*
wchar_t* ConvertToWideChar(const char* src) {
	setlocale(LC_ALL, "");  // Set the locale
	size_t len = strlen(src) + 1;

	wchar_t* dest = new wchar_t[len];
	size_t numCharsConverted = 0;  // Will hold the number of characters converted

	errno_t err = mbstowcs_s(&numCharsConverted, dest, len, src, _TRUNCATE);

	if (err != 0) { // If mbstowcs_s returns a non-zero value, an error has occurred
		delete[] dest; // Don't forget to delete the memory!
		return nullptr; // Handle the error
	}

	return dest;
}

//This function is used to convert a wchar_t* to char*
char* ConvertWCharToChar(const wchar_t* wstr) {
	std::size_t len = wcslen(wstr) + 1;  // Length of wide char string + null terminator
	char* str = new char[len];

	std::size_t convertedChars = 0;
	wcstombs_s(&convertedChars, str, len, wstr, _TRUNCATE);

	return str;
}
//////////////////////////////////////////////////////////////////////////////