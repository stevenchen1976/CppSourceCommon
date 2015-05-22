/*****************************************
* @file     utf8wchar.h
* @brief    utf8与unicode编码(usc2 or usc4)的相互转换
* @details  utf8->w_chart w_chart->uft8
            根据转换函数使用clang代码中的ConvertUTF.c
* @author   phata,wqvbjhc@gmail.com
* @date     2012-12-21
* @mod      2012-07-23:使用C++11标准进行utf与wchar的转换。原先的转换对于纯ascii码返回长度有误，多了不少\0
                       摒弃了convert_utf.c,convert_urf.h文件
			2012-08-02:C++11的转换会抛出异常。对异常进行捕获并返回空串
			2013-11-05:发现转换耗时，故windows下使用windows自身的函数进行转换
******************************************/
#ifndef UTF8_WCHAR_H
#define UTF8_WCHAR_H
#include <string>

namespace utf8_convert_local
{
std::wstring Utf8ToWchar(const std::string& utf8string);
std::string WcharToUtf8(const std::wstring& widestring);
}

#endif
