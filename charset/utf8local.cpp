#include "utf8local.h"
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include "utf8wchar.h"
#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#endif

namespace utf8_convert_local
{
//可移植版本 string => wstring
std::wstring s2ws(const std::string& s)
{
#if defined(WIN32) || defined(_WIN32)
	const char* _Source = s.c_str();
	size_t _Dsize = MultiByteToWideChar(CP_ACP, 0, _Source, -1, NULL, 0);
	wchar_t *_Dest = new wchar_t[_Dsize];
	MultiByteToWideChar(CP_ACP, 0, _Source, -1, _Dest, _Dsize);
	std::wstring result = _Dest;
	delete []_Dest;
	return std::move(result);
#else
	std::string curLocale = setlocale(LC_ALL, "");
	const char* _Source = s.c_str();
	size_t _Dsize = mbstowcs(NULL, _Source, 0) + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest,_Source,_Dsize);
	std::wstring result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return std::move(result);
#endif
}

//可移植版本 wstring => string
std::string ws2s(const std::wstring& ws)
{
#if defined(WIN32) || defined(_WIN32)
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = WideCharToMultiByte(CP_ACP, 0, _Source, -1, NULL, 0, NULL, NULL);
	char *_Dest = new char[_Dsize];
	WideCharToMultiByte(CP_ACP, 0, _Source, -1, _Dest, _Dsize, NULL, NULL);
	std::string result = _Dest;
	delete []_Dest;
	return std::move(result);
#else
	std::string curLocale = setlocale(LC_ALL, "");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = wcstombs(NULL, _Source, 0) + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest,0,_Dsize);
	wcstombs(_Dest,_Source,_Dsize);
	std::string result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return std::move(result);
#endif
}

std::string LocalToUtf8(const std::string& localstr)
{
	std::wstring wcharstr=s2ws(localstr);
	return std::move(WcharToUtf8(wcharstr));
}

std::string Utf8ToLocal(const std::string& utf8str)
{
	std::wstring wcharstr=Utf8ToWchar(utf8str);
	return std::move(ws2s(wcharstr));
}

}
