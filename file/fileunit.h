/***************************************
* @file     fileunit.h
* @brief    与文件相关的操作函数与类
* @details  tempfile--得到一个可用临时文件的名称（得到后可创建）:来自opencv
		    getfilesize--得到文件的大小:来自pugixml,原名get_file_size
			File类：有如下几个静态函数
					canRead
					canWrite
					copy
					exists
					getSize--功能与getfilesize一样
					move
					remove
					serach
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-3
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

#include <stdarg.h>

#if defined __linux__ || defined __APPLE__
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
/////////////////////////////////////////////////////////多平台头文件定义-结束////////////////////////

#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <io.h>

//******************************
//* @brief   //返回系统tmp目录一个可用的临时文件名字(创建,得到名字，删除)(opencv中的函数)
//* @param   const char* suffix--临时文件的后续，NULL表示没后缀
//* @return  string--返回创建的文件名
//******************************/
//返回系统tmp目录一个可用的临时文件名字(创建,得到名字，删除)。opencv 2.4.6中的函数
inline std::string tempfile( const char* suffix )
{
    const char *temp_dir = getenv("OPENCV_TEMP_PATH");
    std::string fname;

#if defined WIN32 || defined _WIN32
    char temp_dir2[MAX_PATH + 1] = { 0 };
    char temp_file[MAX_PATH + 1] = { 0 };

    if (temp_dir == 0 || temp_dir[0] == 0) {
        ::GetTempPathA(sizeof(temp_dir2), temp_dir2);
        temp_dir = temp_dir2;
    }
    if(0 == ::GetTempFileNameA(temp_dir, "ocv", 0, temp_file)) {
        return std::string();
    }

    DeleteFileA(temp_file);

    fname = temp_file;
# else
#  ifdef ANDROID
    //char defaultTemplate[] = "/mnt/sdcard/__opencv_temp.XXXXXX";
    char defaultTemplate[] = "/data/local/tmp/__opencv_temp.XXXXXX";
#  else
    char defaultTemplate[] = "/tmp/__opencv_temp.XXXXXX";
#  endif

    if (temp_dir == 0 || temp_dir[0] == 0) {
        fname = defaultTemplate;
    } else {
        fname = temp_dir;
        char ech = fname[fname.size() - 1];
        if(ech != '/' && ech != '\\') {
            fname += "/";
        }
        fname += "__opencv_temp.XXXXXX";
    }

    const int fd = mkstemp((char*)fname.c_str());
    if (fd == -1) {
        return std::string();
    }

    close(fd);
    remove(fname.c_str());
# endif

    if (suffix) {
        if (suffix[0] != '.') {
            return fname + "." + suffix;
        } else {
            return fname + suffix;
        }
    }
    return fname;
}

//******************************
//* @brief   得到文件的大小(pugixml中的函数)
//* @param   FILE* file--文件句柄
//*			 size_t& out_result--返回的文件大小
//* @return  int--0表示成功，2表io出错，3表内存不足(文件大小超过size_t范围)
//******************************/
//得到文件的大小
// we need to get length of entire file to load it in memory; the only (relatively) sane way to do it is via seek/tell trick
inline int getfilesize(FILE* file, size_t& out_result)
{
#if defined(PUGI__MSVC_CRT_VERSION) && PUGI__MSVC_CRT_VERSION >= 1400 && !defined(_WIN32_WCE)
    // there are 64-bit versions of fseek/ftell, let's use them
    typedef __int64 length_type;

    _fseeki64(file, 0, SEEK_END);
    length_type length = _ftelli64(file);
    _fseeki64(file, 0, SEEK_SET);
#elif defined(__MINGW32__) && !defined(__NO_MINGW_LFS) && !defined(__STRICT_ANSI__)
    // there are 64-bit versions of fseek/ftell, let's use them
    typedef off64_t length_type;

    fseeko64(file, 0, SEEK_END);
    length_type length = ftello64(file);
    fseeko64(file, 0, SEEK_SET);
#else
    // if this is a 32-bit OS, long is enough; if this is a unix system, long is 64-bit, which is enough; otherwise we can't do anything anyway.
    typedef long length_type;

    fseek(file, 0, SEEK_END);
    length_type length = ftell(file);
    fseek(file, 0, SEEK_SET);
#endif

    // check for I/O errors
    if (length < 0) {
        return 2;    //status_io_error;
    }

    // check for overflow
    size_t result = static_cast<size_t>(length);

    if (static_cast<length_type>(result) != length) {
        return 3;    //status_out_of_memory;
    }

    // finalize
    out_result = result;

    return 0;//status_ok;
}


//////////////////////////////////////////////////////////////////////////
//文件访问类
#if defined WIN32 || defined _WIN32
class File
{
public:
    enum FileMode {
        ReadOnly = 0x02, //只读
        WriteOnly = 0x04,//只写
        Both = 0x06//读写
    };
    //////////////////////////////////////////////////////////////////////////
    //修改文件读写模式
    //@filename:文件名
    //@mode:模式
    //@return:true 修改成功
    static bool changeMode(const std::string& filename, FileMode mode) {
        switch(mode) {
        case ReadOnly:
            return (0 == _chmod(filename.c_str(), _S_IREAD));
        case WriteOnly:
            return (0 == _chmod(filename.c_str(), _S_IWRITE));
        case Both:
            return (0 == _chmod(filename.c_str(), _S_IREAD|_S_IWRITE));
        default:
            return false;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //复制文件
    //@sourceFile:源文件
    //@destFile:目标文件
    //@overwrite:是否允许覆盖已存在文件
    static bool copy(const std::string& sourceFile, const std::string& destFile, bool overwrite = false) {
        if (!canRead(sourceFile)) {
            //源文件不存在或不可读
            return false;
        }
        if (overwrite && exists(destFile)) {
            //目标文件存在
            if (!canWrite(destFile)) {
                //不可写
                return false;
            }
        }
        //读文件流
        std::fstream readStream;
        //写文件流
        std::fstream writeStream;
        readStream.open(sourceFile.c_str(), std::ios::in| std::ios::binary);
        writeStream.open(destFile.c_str(), std::ios::out| std::ios::binary);
        char currentChar = 0;
        while (!readStream.eof()) {
            readStream.get(currentChar);
            if (!readStream.eof()) {
                writeStream << currentChar;
            }
        }
        readStream.close();
        writeStream.close();
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    //是否存在指定文件
    //@filename:文件名
    //@return:true 存在
    static bool exists(const std::string& filename) {
        return (0 == _access(filename.c_str(), 0));
    }

    //////////////////////////////////////////////////////////////////////////
    //删除指定文件
    //@filename:文件名
    static bool remove(const std::string& filename) {
        if (!exists(filename)) {
            //不存在
            return false;
        }
        return (0 == _unlink(filename.c_str()));
    }

    //////////////////////////////////////////////////////////////////////////
    //移动文件
    //@sourceFile:源文件
    //@destFile:目标文件
    static bool move(const std::string& sourceFile, const std::string& destFile) {
        //仅可移动文件
        return (0 == rename(sourceFile.c_str(), destFile.c_str()));
    }

    //////////////////////////////////////////////////////////////////////////
    //是否可读
    //@path:路径
    //@return:true 可读
    static bool canRead(const std::string& path) {
        if (!exists(path)) {
            //文件不存在
            return false;
        }
        if (0 == _access(path.c_str(), 4)) {
            //read only
            return true;
        }
        if (0 == _access(path.c_str(), 6)) {
            //read write
            return true;
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //是否可写
    //@path:路径
    //@return:true 可写
    static bool canWrite(const std::string& path) {
        if (!exists(path)) {
            //文件不存在
            return false;
        }
        if (0 == _access(path.c_str(), 2)) {
            //read only
            return true;
        }
        if (0 == _access(path.c_str(), 6)) {
            //read write
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    //获取文件大小
    //@path:路径
    //@return:0 = 不存在
    static size_t getSize(const std::string& path) {
        if (!exists(path)) {
            return 0;
        }
        struct _stat status;
        if (0 == _stat(path.c_str(), &status)) {
            //获取成功
            return status.st_size;
        } else {
            //获取失败
            return 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //搜索符合条件的文件
    //@condition:搜索条件，如"c:/windows/*.*"
    //@files:符合条件的文件名集合，不带路径
    //@return:true 有符合条件的文件
    static bool search(const std::string& condition, std::vector< std::string >& files) {
        struct _finddata_t file_data;
        intptr_t hFile;
        hFile = _findfirst(condition.c_str(), &file_data);
        if ( -1L == hFile) {
            //没有匹配文件
            return false;
        }
        do {
            //遍历所有文件
            files.push_back(file_data.name);
        } while (0 == _findnext(hFile, &file_data));
        _findclose(hFile);
        if (0 != files.size()) {
            return true;
        } else {
            return false;
        }
    }
};
#elif defined __linux__ && !defined ANDROID
class File
{
public:
    //////////////////////////////////////////////////////////////////////////
    //复制文件
    //@sourceFile:源文件
    //@destFile:目标文件
    //@overwrite:是否允许覆盖已存在文件
    static bool copy(const std::string& sourceFile, const std::string& destFile, bool overwrite = false) {
        if (!canRead(sourceFile)) {
            //源文件不存在或不可读
            return false;
        }
        if (overwrite && exists(destFile)) {
            //目标文件存在
            if (!canWrite(destFile)) {
                //不可写
                return false;
            }
        }
        //读文件流
        std::fstream readStream;
        //写文件流
        std::fstream writeStream;
        readStream.open(sourceFile.c_str(), std::ios::in|std::ios::binary);
        writeStream.open(destFile.c_str(), std::ios::out|std::ios::binary);
        char currentChar = 0;
        while (!readStream.eof()) {
            readStream.get(currentChar);
            if (!readStream.eof()) {
                writeStream << currentChar;
            }
        }
        readStream.close();
        writeStream.close();
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    //是否存在指定文件
    //@filename:文件名
    //@return:true 存在
    static bool exists(const std::string& filename) {
        return (0 == access(filename.c_str(), 0));
    }

    //////////////////////////////////////////////////////////////////////////
    //删除指定文件
    //@filename:文件名
    static bool remove(const std::string& filename) {
        if (!exists(filename)) {
            //不存在
            return false;
        }
        return (0 == unlink(filename.c_str()));
    }

    //////////////////////////////////////////////////////////////////////////
    //移动文件
    //@sourceFile:源文件
    //@destFile:目标文件
    static bool move(const std::string& sourceFile, const std::string& destFile) {
        return (0 == rename(sourceFile.c_str(), destFile.c_str()));
    }

    //////////////////////////////////////////////////////////////////////////
    //是否可读
    //@path:路径
    //@return:true 可读
    static bool canRead(const std::string& path) {
        if (!exists(path)) {
            //文件不存在
            return false;
        }
        if (0 == access(path.c_str(), 4)) {
            //read only
            return true;
        }
        if (0 == access(path.c_str(), 6)) {
            //read write
            return true;
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //是否可写
    //@path:路径
    //@return:true 可写
    static bool canWrite(const std::string& path) {
        if (!exists(path)) {
            //文件不存在
            return false;
        }
        if (0 == access(path.c_str(), 2)) {
            //read only
            return true;
        }
        if (0 == access(path.c_str(), 6)) {
            //read write
            return true;
        }
        return false;
    }
    //获取文件大小
    //@path:路径
    //@return:0 = 不存在
    static size_t getSize(const std::string& path) {
        if (!exists(path)) {
            return 0;
        }
        struct stat status;
        if (0 == stat(path.c_str(), &status)) {
            //获取成功
            return status.st_size;
        } else {
            //获取失败
            return 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////
    //搜索符合条件的文件
    //@condition:搜索条件，如"c:/windows/*.*"
    //@files:符合条件的文件名集合，不带路径
    //@return:true 有符合条件的文件
    static bool search(const std::string& condition, std::vector< std::string >& files) {
        files.clear();
        DIR *dir;
        bool hasS = false;
        do {
            if(!(dir = opendir(condition.c_str()))) { //有 /  *  *.*  *.c  a.*  a 结尾的情况
                // 可能是有"*"的情况,取出最后一个'/'以后的东东,再试一次
                // 可能是文件
                std::string strLast = condition.substr(condition.find_last_of("/")+1);
                if ((int)strLast.find("*") >= 0) {
                    //再试一次
                    strLast = condition.substr(0, condition.find_last_of("/"));
                    if (!(dir=opendir(strLast.c_str()))) {
                        //失败了
                        return false;
                    } else {
                        hasS = true;
                        break;//跳出do while(0);
                    }
                } else if((int)strLast.size()>0) { //原来是没有*的,并且是/a的形式,看看是不是文件
                    if (access(condition.c_str(),0) != 0) { //连文件也不是
                        return false;
                    } else { //是文件来的
                        files.push_back(condition.substr(condition.find_last_of("/")+1));
                        return true;
                    }

                } else {	//  /
                    return false;
                }
            }
        } while(0);
        //是文件夹来的
        closedir(dir);

        //cout<<"OK"<<endl;
        struct dirent ** namelist;
        int n;
        std::string tmpstr = condition.substr(condition.find_last_of("/")+1);//取出目录名
        if (!hasS || (hasS && (tmpstr=="*" || tmpstr=="*.*"))) {	// /a  /a/ *  *.*把目录下的东西全部显示出来
            std::string dentry ;
            //有*的话去掉
            if ( hasS ) {
                dentry = condition.substr(0, condition.find_last_of("/"));
            } else {
                dentry = condition;
            }
            n = scandir(dentry.c_str(), &namelist, select_no_dot, alphasort);
            while(n--) {
                files.push_back(std::string(namelist[n]->d_name));
                delete namelist[n];
            }
            delete namelist;
            return true;
        } else if(hasS && tmpstr.find("*") >= 0) {	// *.c  a.*
            struct dirent *ptr;
            dir = opendir(condition.substr(0, condition.find_last_of("/")).c_str());

            if ((int)tmpstr.find("*") == 0) { //*.c
                std::string strEnd = tmpstr.substr(tmpstr.find_last_of(".")+1);//c
                while((ptr = readdir(dir))!=NULL) {
                    std::string strFN(ptr->d_name);
                    int pos = strFN.find_last_of(".");
                    if ( pos >= 0  &&  strFN.substr(pos + 1) == strEnd) {
                        files.push_back(strFN);
                    }
                    //delete ptr;
                }
            } else if((int)tmpstr.find("*") > 0) { //a.*
                std::string strBegin = tmpstr.substr(0, tmpstr.find_first_of("."));//a
                while((ptr = readdir(dir))!=NULL) {
                    std::string strFN(ptr->d_name);
                    if (strFN.substr(0, strFN.find_first_of(".")) == strBegin) {
                        files.push_back(strFN);
                    }
                    //delete ptr;
                }

            }
            closedir(dir);
            return true;
        }
        return false;
    }

private:
    static int select_no_dot(const struct dirent * file) { //a.*
        if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, "..")) {
            return false;
        }
        return true;
    }
};

#endif
