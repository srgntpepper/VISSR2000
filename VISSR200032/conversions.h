#pragma once
#include "stdAfx.h"
/***************************************************************************
	CONVERSION FUNCTIONS - Only useful for unicode implementation
***************************************************************************/

//This function is used to convert LPSTR or char* to LPCWSTR - Chris Davis
std::wstring ConvertToLPCWSTR(const char* charArray);

//This function is used to convert a char* to a wchar_t*
wchar_t* ConvertToWideChar(const char*);

//This function is used to convert a wchar_t* to char*
char* ConvertWCharToChar(const wchar_t*);