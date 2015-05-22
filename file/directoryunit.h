/***************************************
* @file     directoryunit.h
* @brief    与目录相关的操作函数与类
* @details	Directory类：有如下几个静态函数
				combine--组合两个路径
				getDirectoryName--获取目录名
				changeExtension--修改路径后缀
				getExtension--获取后缀
				getFileName--获取文件名
				getFileNameWithoutExtension--获取不带后缀的文件名
				getFullPath--获取绝对路径
				makePath--创建路径（自动创建多级路径）
				getSubDirs--获取指定目录下的所有子目录
				removePath--删除指定的目录
				exists --判断目录是否存在
				getExePath--返回exe或dll所在目录,\结尾
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-4
* @mod      2014-11-13 phata 添加exists函数
            2015-03-11 phata 添加getExePath函数
****************************************/
/////////////////////////////////////////////////////////多平台头文件定义////////////////////////
#if defined WIN32 || defined _WIN32 || defined WINCE
#ifndef _WIN32_WINNT           // This is needed for the declaration of TryEnterCriticalSection in winbase.h with Visual Studio 2005 (and older?)
#define _WIN32_WINNT 0x0500  // http://msdn.microsoft.com/en-us/library/ms686857(VS.85).aspx
#endif
#include <windows.h>
#undef small
#undef min
#undef max
#undef abs
#include <tchar.h>
#if defined _MSC_VER
#if _MSC_VER >= 1400
#include <intrin.h>
#endif
#endif
#else
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#endif


#if defined __linux__ || defined __APPLE__
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
/////////////////////////////////////////////////////////多平台头文件定义-结束////////////////////////
#include <stdarg.h>
#include <direct.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <sstream>

//////////////////////////////////////////////////////////////////////////
//路径名称解析
#if defined WIN32 || defined _WIN32

//路径名称解析
class Directory
{
    class CFileInfo
    {
    public:
        //std::string m_strName;
        std::string m_strFile;  //文件名（不包含路径）
        //std::string m_strPath;
        int m_iType;
    };
public:
    //////////////////////////////////////////////////////////////////////////
    //组合两个路径
    static std::string combine(const std::string& path1, const std::string& path2)
    {
        std::stringstream cache;
        cache << path1 << getPathSeparator() << path2;
        return cache.str();
    }
    //////////////////////////////////////////////////////////////////////////
    //获取目录名
    static std::string getDirectoryName(const std::string& path)
    {
        //递归调用，去掉非windows格式分隔符
        if (hasNonwinFormatSeparator(path)) {
            return getDirectoryName(formatPathSeparator(path));
        }
        std::string::size_type offset = getPathSeparatorOffset(path);
        if (std::string::npos == offset) {
            //没有路径名
            return "";
        }
        std::string::size_type drive = path.find(":\\");
        if (std::string::npos == drive) {
            return path.substr(0, offset);
        } else {
            if (drive == (offset - 1)) {
                //只有盘符，没有路径
                return "";
            }
            //有盘符
            std::string::size_type length = offset - drive - 2;
            return path.substr(drive + 2, length);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //修改路径后缀
    static std::string changeExtension(const std::string& path, const std::string& ext)
    {
        std::string filename = getFileNameWithoutExtension(path);
        if (0 == filename.size()) {
            //无文件名
            return path;
        }
        std::stringstream cache;
        cache << getFullPath(path);
        cache << filename;
        cache << getFileSeparator() << ext;
        return cache.str();
    }
    //////////////////////////////////////////////////////////////////////////
    //获取后缀
    static std::string getExtension(const std::string& path)
    {
        std::string::size_type offset = getFileSeparatorOffset(path);
        if (std::string::npos == offset) {
            //没有间隔符
            return "";
        }
        return path.substr(offset + 1, path.length() - offset - 1);
    }
    //////////////////////////////////////////////////////////////////////////
    //获取文件名
    static std::string getFileName(const std::string& path)
    {
        //递归调用，去掉非windows格式分隔符
        if (hasNonwinFormatSeparator(path)) {
            return getFileName(formatPathSeparator(path));
        }
        std::string::size_type offset = getPathSeparatorOffset(path);
        if (std::string::npos == offset) {
            //没有路径名
            return path;
        }
        return path.substr(offset + 1, path.length() - offset - 1);
    }
    //////////////////////////////////////////////////////////////////////////
    //获取不带后缀的文件名
    static std::string getFileNameWithoutExtension(const std::string& path)
    {
        //递归调用，去掉非windows格式分隔符
        if (hasNonwinFormatSeparator(path)) {
            return getFileNameWithoutExtension(formatPathSeparator(path));
        }
        size_t extOffset = getFileSeparatorOffset(path);
        if (std::string::npos == extOffset) {
            //没有扩展名，与文件名相同
            return getFileName(path);
        }
        size_t pathOffset = getPathSeparatorOffset(path);
        if (std::string::npos == pathOffset) {
            //没有路径名
            return path.substr(0, extOffset);
        }
        size_t length = extOffset - pathOffset - 1;
        return path.substr(pathOffset + 1, length);
    }
    //////////////////////////////////////////////////////////////////////////
    //获取绝对路径
    static std::string getFullPath(const std::string& path)
    {
        //递归调用，去掉非windows格式分隔符
        if (hasNonwinFormatSeparator(path)) {
            return getFullPath(formatPathSeparator(path));
        }
        std::string::size_type offset = getPathSeparatorOffset(path);
        if (std::string::npos == offset) {
            //没有路径名
            return "";
        }
        return path.substr(0, getPathSeparatorOffset(path) + 1);
    }
    //创建路径（自动创建多级路径）
    //@pathName:路径名
    //@return:true创建成功
    static bool makePath(const std::string& pathName)
    {
        std::string dirname = formatPathSeparator(pathName);

        if (*dirname.rbegin() != getPathSeparator()) {
            dirname.push_back(getPathSeparator());
        }

        size_t len = dirname.size();
        size_t i = 0;

        for (i = 1;  i < len;   i++) {
            if (dirname[i] == getPathSeparator()) {
                dirname[i] = '\0';
                if (_access(dirname.c_str(), 0) != 0) {
                    if (_makePath(dirname) == false) {
                        return false;
                    }
                }
                dirname[i] = getPathSeparator();
            }
        }

        return true;
    }
    //获取指定目录下的所有子目录
    static int getSubDirs(const std::string& pathName, std::vector<std::string>& vecDirs)
    {
        std::string dirname = formatPathSeparator(pathName);

        if (*dirname.rbegin() != getPathSeparator()) {
            dirname.push_back(getPathSeparator());
        }
        std::vector<CFileInfo> vecFileInfo;

        std::string strCondition = dirname + "*.*";
        int iCount = _getAllFile(strCondition, vecFileInfo);

        for (int i = 0; i < iCount; i++) {
            std::string strFile = dirname + vecFileInfo[i].m_strFile;
            if (_A_SUBDIR == vecFileInfo[i].m_iType) {
                if ("." != vecFileInfo[i].m_strFile && ".." != vecFileInfo[i].m_strFile) {
                    vecDirs.push_back(vecFileInfo[i].m_strFile);
                }
            }
        }
        return vecDirs.size();
    }
    //删除指定的目录（包括目录下的文件）
    static bool removePath(const std::string& pathName)
    {
        std::string dirname = formatPathSeparator(pathName);

        if (*dirname.rbegin() != getPathSeparator()) {
            dirname.push_back(getPathSeparator());
        }

        std::vector<CFileInfo> vecFileInfo;

        std::string strCondition = dirname + "*.*";
        int iCount = _getAllFile(strCondition, vecFileInfo);

        for (int i = 0; i < iCount; i++) {
            std::string strFile = dirname + vecFileInfo[i].m_strFile;
            if (_A_SUBDIR == vecFileInfo[i].m_iType) {
                if ("." != vecFileInfo[i].m_strFile && ".." != vecFileInfo[i].m_strFile) {
                    removePath(strFile);
                }
            } else {
                _unlink(strFile.c_str());
            }
        }
        return _removePath(dirname);
    }

    //判断目录是否存在
    static bool exists(const std::string& dirname)
    {
        struct _stat fileStat;
        if ((_stat(dirname.c_str(), &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
            return true;
        }
        return false;
    }

    //返回exe或dll所在目录,\结尾
    static std::string getExePath()
    {
        HMODULE module = GetModuleHandle(0);
        char szPath[MAX_PATH];
        if (!GetModuleFileNameA(module, szPath, MAX_PATH)) {
            return "";
        }
        std::string strPath = szPath;
		int nIndex = strPath.rfind('\\');
		strPath = strPath.substr(0,nIndex+1);
        return strPath;
    }
private:
    //////////////////////////////////////////////////////////////////////////
    //获取路径间隔符
    static const char& getPathSeparator()
    {
        static const char pathSep = '\\';
        return pathSep;
    }
    //////////////////////////////////////////////////////////////////////////
    //获取文件间隔符
    static const char& getFileSeparator()
    {
        static const char fileSep = '.';
        return fileSep;
    }
    //////////////////////////////////////////////////////////////////////////
    //获取文件间隔符偏移位置
    static std::string::size_type getFileSeparatorOffset(const std::string& path)
    {
        return path.find_last_of(getFileSeparator());
    }
    //////////////////////////////////////////////////////////////////////////
    //获取路径间隔符偏移位置（最右侧路径间隔符）
    static std::string::size_type getPathSeparatorOffset(const std::string& path)
    {
        return path.find_last_of(getPathSeparator());
    }
    //////////////////////////////////////////////////////////////////////////
    //路径中包含非windows格式间隔符
    static bool hasNonwinFormatSeparator(const std::string& path)
    {
        return (std::string::npos != path.find('/'));
    }
    //////////////////////////////////////////////////////////////////////////
    //替换非windows格式间隔符
    static std::string formatPathSeparator(const std::string& path)
    {
        std::stringstream cache;
        char ch = 0;
        for (size_t offset = 0; offset < path.size(); offset++) {
            ch = path.at(offset);
            if (ch == '/') {
                //替换为windows间隔符
                cache << getPathSeparator();
            } else {
                cache << ch;
            }
        }
        return cache.str();
    }

    //////////////////////////////////////////////////////////////////////////
    //创建路径（不可自动创建多级路径）
    //@path:路径名
    //@return:true创建成功
    static bool _makePath(const std::string& path)
    {
        return (0 == _mkdir(path.c_str()));
    }

    //删除路径
    static bool _removePath(const std::string& path)
    {
        return (0 == _rmdir(path.c_str()));
    }

    static int _getAllFile(const std::string& condition, std::vector<CFileInfo>& vecFileInfo)
    {
        struct _finddata_t file_data;
        intptr_t hFile;
        hFile = _findfirst(condition.c_str(), &file_data);
        if (-1L == hFile) {
            //没有匹配文件
            return 0;
        }
        do {
            //遍历所有文件
            CFileInfo fileInfo;
            fileInfo.m_strFile = file_data.name;
            fileInfo.m_iType = file_data.attrib;
            vecFileInfo.push_back(fileInfo);
        } while (0 == _findnext(hFile, &file_data));
        _findclose(hFile);

        return vecFileInfo.size();
    }
};
#elif defined __linux__ && !defined ANDROID
class Directory
{
public:
    //////////////////////////////////////////////////////////////////////////
    //组合两个路径
    static std::string combine(const std::string& path1, const std::string& path2);
    //////////////////////////////////////////////////////////////////////////
    //获取目录名
    static std::string getDirectoryName(const std::string& path)
    {
        std::string::size_type pos = path.find_last_of("/");
        if (pos == std::string::npos) {
            return path;
        }

        return path.substr(0, pos);
    }
    //////////////////////////////////////////////////////////////////////////
    //修改路径后缀
    static std::string changeExtension(const std::string& path, const std::string& ext);
    //////////////////////////////////////////////////////////////////////////
    //获取后缀
    static std::string getExtension(const std::string& path);
    //////////////////////////////////////////////////////////////////////////
    //获取文件名
    static std::string getFileName(const std::string& path)
    {
        std::string::size_type pos = path.find_last_of("/");
        if (pos == std::string::npos) {
            return path;
        }
        return path.substr(pos + 1);
    }
    //////////////////////////////////////////////////////////////////////////
    //获取不带后缀的文件名
    static std::string getFileNameWithoutExtension(const std::string& path)
    {
        std::string::size_type ext_offset = path.find_last_of('.');
        if (std::string::npos == ext_offset) {
            //没有后缀名
            return getFileName(path);
        }
        std::string::size_type path_offset = path.find_last_of('/');
        if (std::string::npos == path_offset) {
            //没有带路径
            return path.substr(0, ext_offset);
        }
        size_t length = ext_offset - path_offset - 1;
        return path.substr(path_offset + 1, length);
    }
    //////////////////////////////////////////////////////////////////////////
    //获取绝对路径
    static std::string getFullPath(const std::string& path);

    //创建路径（自动创建多级路径）
    //@pathName:路径名
    //@return:true创建成功
    static bool makePath(const std::string& pathName)
    {
        std::string dirname = formatPathSeparator(pathName);

        if (*dirname.rbegin() != getPathSeparator()) {
            dirname.push_back(getPathSeparator());
        }

        size_t len = dirname.size();
        size_t i = 0;

        for (i = 1;  i < len;   i++) {
            if (dirname[i] == getPathSeparator()) {
                dirname[i] = '\0';
                if (access(dirname.c_str(), F_OK) != 0) {
                    if (_makePath(dirname) == false) {
                        return false;
                    }
                }
                dirname[i] = getPathSeparator();
            }
        }
        return true;
    }

    //判断目录是否存在
    static bool exists(const std::string& dirname)
    {
        struct stat fileStat;
        if ((stat(dirname.c_str(), &fileStat) == 0) && S_ISDIR(fileStat.st_mode)) {
            return true;
        }
        return false;
    }

private:
    //////////////////////////////////////////////////////////////////////////
    //路径中包含非windows格式间隔符
    static bool haswinFormatSeparator(const std::string& path)
    {
        return (std::string::npos != path.find('\\'));
    }
    //////////////////////////////////////////////////////////////////////////
    //替换windows格式间隔符
    static std::string formatPathSeparator(const std::string& path)
    {
        std::stringstream cache;
        char ch = 0;
        for (size_t offset = 0; offset < path.size(); offset++) {
            ch = path.at(offset);
            if (ch == '\\') {
                //替换为windows间隔符
                cache << getPathSeparator();
            } else {
                cache << ch;
            }
        }
        return cache.str();
    }

    //////////////////////////////////////////////////////////////////////////
    //获取路径间隔符
    static const char& getPathSeparator()
    {
        static const char pathSep = '/';
        return pathSep;
    }
    //////////////////////////////////////////////////////////////////////////
    //创建路径（不可自动创建多级路径）
    //@path:路径名
    //@return:true创建成功
    static bool _makePath(const std::string& path)
    {
        return (0 == mkdir(path.c_str(), 0777));
    }

    //返回exe或dll所在目录
    static std::string getExePath()
    {
        char szPath[MAX_PATH];
        ssize_t n = readlink("/proc/self/exe", buffer, *size - 1);
        if (n == -1) {
            return "";
        }
        szPath[n] = '\0';
        return szPath;
    }
};
#endif
