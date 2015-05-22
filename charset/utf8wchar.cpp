#include "utf8wchar.h"
#include <stdio.h>
#include <codecvt>
#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#endif

namespace utf8_convert_local
{
std::wstring Utf8ToWchar(const std::string& utf8string)
{
	std::wstring wideStr;
#if defined(WIN32) || defined(_WIN32)
	const char* _Source = utf8string.c_str();
	size_t _Dsize = MultiByteToWideChar(CP_UTF8, 0, _Source, -1, NULL, 0);
	wchar_t *_Dest = new wchar_t[_Dsize];
	MultiByteToWideChar(CP_UTF8, 0, _Source, -1, _Dest, _Dsize);
	wideStr = _Dest;
	delete []_Dest;
#else
    //C+11标准
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;//字符集unicode<->uft8
	try {//from_bytes会抛出range_error
        wideStr = conv.from_bytes(utf8string);//utf8->unicode
    } catch (std::exception &e) {
        fprintf(stdout,"Caught: %s.\n",e.what());
        return wideStr;
    }
#endif
	return std::move(wideStr);
}

std::string WcharToUtf8(const std::wstring& widestring)
{
	std::string narrowStr;
#if defined(WIN32) || defined(_WIN32)
	const wchar_t* _Source = widestring.c_str();
	size_t _Dsize = WideCharToMultiByte(CP_UTF8, 0, _Source, -1, NULL, 0, NULL, NULL);
	char *_Dest = new char[_Dsize];
	WideCharToMultiByte(CP_UTF8, 0, _Source, -1, _Dest, _Dsize, NULL, NULL);
	narrowStr = _Dest;
	delete []_Dest;
#else
    //C+11标准
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;//字符集unicode<->uft8
	try {//to_bytes会抛出range_error
		narrowStr = conv.to_bytes(widestring);//unicode->uft8
	} catch (std::exception &e) {
		fprintf(stdout,"Caught: %s.\n",e.what());
		return narrowStr;
	}
#endif
	return std::move(narrowStr);
}
}
