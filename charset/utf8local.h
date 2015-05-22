/*****************************************
* @file     utf8local.h
* @brief    utf8与本地字符编译的相互转换,utf8为网页与linux常用的字符编码,而win的字符编码则因地区而异,
* @details  utf8为网页与linux常用的字符编码,而win的字符编码则因地区而异,因为需要把utf8与本地编码进行转换
            utf8->unicode->local
            local->unicode->uft8
* @author   phata,wqvbjhc@gmail.com
* @date     2012-12-21
* @mod      2013-11-05:发现转换耗时，故windows下使用windows自身的函数进行转换
******************************************/
#ifndef UTF8_LOCAL_H
#define UTF8_LOCAL_H
#include <string>

namespace utf8_convert_local
{
std::wstring s2ws(const std::string& s);
std::string ws2s(const std::wstring& ws);
std::string LocalToUtf8(const std::string& localstr);
std::string Utf8ToLocal(const std::string& utf8str);
}
/*************使用例子********
#include "utf8local.h"
#include <stdio.h>
#include <string>

int main()
{
	std::string localstr="你好";//gbk编码\xC4\xE3\xBA\xC3
	std::string utf8str=utf8_convert_local::LocalToUtf8(localstr);
	fprintf(stdout,"local[%s]->utf8[%s]\n",localstr.c_str(),utf8str.c_str());
	localstr=utf8_convert_local::Utf8ToLocal(utf8str);
	fprintf(stdout,"utf8[%s]->local[%s]\n",utf8str.c_str(),localstr.c_str());
	return 0;
}
********************************/
#endif//UTF8_LOCAL_H
