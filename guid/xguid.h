/*****************************************
* @file     xguid.h
* @brief    跨平台生成uuid/guid函数(由网上代码修改而来,很多人转载不注明出处,找不到原作者)
* @details  通过调用各平台的函数实现，win平台需要链接libole32.a/ole32.lib库文件。
            linux平台需要uuid库的支持(如果系统没有该库，则通过sudo apt-get install uuid-dev安装)，需要链接libuuid.a库文件
            cb链接选项:[[ if (PLATFORM == PLATFORM_MSW) { print( _T("-lole32"));} else {print( _T("-luuid"));}]]
* @author   phata,wqvbjhc@gmail.com
* @date     2012-10-31
* @mod      2014-04-03  phata  只有头文件，所以函数必须使用inline.
******************************************/
#ifndef XGUID_H
#define XGUID_H

#include <string>
#include <stdio.h>
#if (defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64)
#include <objbase.h>
#ifdef _MSC_VER
#pragma comment(lib,"ole32.lib")
#endif // _MSC_VER
#elif defined(__linux__)
#include <uuid/uuid.h>
typedef struct _GUID {
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID, UUID;
#else
#error "no supported os"
#endif

namespace XGUID
{
inline GUID CreateGuid()
{
    GUID guid;
#if (defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64)
    CoCreateGuid(&guid);
#elif defined(__linux__)
    uuid_generate(reinterpret_cast<unsigned char *>(&guid));
#else
#endif
    return guid;
}

inline const char* GuidToString(const GUID &guid)
{//8-4-4-4-12格式，共36字节
    static char buf[37] = {0};
#ifdef __GNUC__
    snprintf(
#else // MSVC
    _snprintf_s(
#endif
        buf,
        sizeof(buf),
        "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1],
        guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5],
        guid.Data4[6], guid.Data4[7]);
    return buf;
}

inline const char* CreateGuidString()
{
    return GuidToString(CreateGuid());
}

}//namespace
//用法
//const char* strguid=XGUID::CreateGuidString();
#endif // XGUID_H
