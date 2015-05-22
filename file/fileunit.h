/***************************************
* @file     fileunit.h
* @brief    ���ļ���صĲ�����������
* @details  tempfile--�õ�һ��������ʱ�ļ������ƣ��õ���ɴ�����:����opencv
		    getfilesize--�õ��ļ��Ĵ�С:����pugixml,ԭ��get_file_size
			File�ࣺ�����¼�����̬����
					canRead
					canWrite
					copy
					exists
					getSize--������getfilesizeһ��
					move
					remove
					serach
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-3
****************************************/
/////////////////////////////////////////////////////////��ƽ̨ͷ�ļ�����////////////////////////
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
/////////////////////////////////////////////////////////��ƽ̨ͷ�ļ�����-����////////////////////////

#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <io.h>

//******************************
//* @brief   //����ϵͳtmpĿ¼һ�����õ���ʱ�ļ�����(����,�õ����֣�ɾ��)(opencv�еĺ���)
//* @param   const char* suffix--��ʱ�ļ��ĺ�����NULL��ʾû��׺
//* @return  string--���ش������ļ���
//******************************/
//����ϵͳtmpĿ¼һ�����õ���ʱ�ļ�����(����,�õ����֣�ɾ��)��opencv 2.4.6�еĺ���
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
//* @brief   �õ��ļ��Ĵ�С(pugixml�еĺ���)
//* @param   FILE* file--�ļ����
//*			 size_t& out_result--���ص��ļ���С
//* @return  int--0��ʾ�ɹ���2��io����3���ڴ治��(�ļ���С����size_t��Χ)
//******************************/
//�õ��ļ��Ĵ�С
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
//�ļ�������
#if defined WIN32 || defined _WIN32
class File
{
public:
    enum FileMode {
        ReadOnly = 0x02, //ֻ��
        WriteOnly = 0x04,//ֻд
        Both = 0x06//��д
    };
    //////////////////////////////////////////////////////////////////////////
    //�޸��ļ���дģʽ
    //@filename:�ļ���
    //@mode:ģʽ
    //@return:true �޸ĳɹ�
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
    //�����ļ�
    //@sourceFile:Դ�ļ�
    //@destFile:Ŀ���ļ�
    //@overwrite:�Ƿ��������Ѵ����ļ�
    static bool copy(const std::string& sourceFile, const std::string& destFile, bool overwrite = false) {
        if (!canRead(sourceFile)) {
            //Դ�ļ������ڻ򲻿ɶ�
            return false;
        }
        if (overwrite && exists(destFile)) {
            //Ŀ���ļ�����
            if (!canWrite(destFile)) {
                //����д
                return false;
            }
        }
        //���ļ���
        std::fstream readStream;
        //д�ļ���
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
    //�Ƿ����ָ���ļ�
    //@filename:�ļ���
    //@return:true ����
    static bool exists(const std::string& filename) {
        return (0 == _access(filename.c_str(), 0));
    }

    //////////////////////////////////////////////////////////////////////////
    //ɾ��ָ���ļ�
    //@filename:�ļ���
    static bool remove(const std::string& filename) {
        if (!exists(filename)) {
            //������
            return false;
        }
        return (0 == _unlink(filename.c_str()));
    }

    //////////////////////////////////////////////////////////////////////////
    //�ƶ��ļ�
    //@sourceFile:Դ�ļ�
    //@destFile:Ŀ���ļ�
    static bool move(const std::string& sourceFile, const std::string& destFile) {
        //�����ƶ��ļ�
        return (0 == rename(sourceFile.c_str(), destFile.c_str()));
    }

    //////////////////////////////////////////////////////////////////////////
    //�Ƿ�ɶ�
    //@path:·��
    //@return:true �ɶ�
    static bool canRead(const std::string& path) {
        if (!exists(path)) {
            //�ļ�������
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
    //�Ƿ��д
    //@path:·��
    //@return:true ��д
    static bool canWrite(const std::string& path) {
        if (!exists(path)) {
            //�ļ�������
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
    //��ȡ�ļ���С
    //@path:·��
    //@return:0 = ������
    static size_t getSize(const std::string& path) {
        if (!exists(path)) {
            return 0;
        }
        struct _stat status;
        if (0 == _stat(path.c_str(), &status)) {
            //��ȡ�ɹ�
            return status.st_size;
        } else {
            //��ȡʧ��
            return 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //���������������ļ�
    //@condition:������������"c:/windows/*.*"
    //@files:�����������ļ������ϣ�����·��
    //@return:true �з����������ļ�
    static bool search(const std::string& condition, std::vector< std::string >& files) {
        struct _finddata_t file_data;
        intptr_t hFile;
        hFile = _findfirst(condition.c_str(), &file_data);
        if ( -1L == hFile) {
            //û��ƥ���ļ�
            return false;
        }
        do {
            //���������ļ�
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
    //�����ļ�
    //@sourceFile:Դ�ļ�
    //@destFile:Ŀ���ļ�
    //@overwrite:�Ƿ��������Ѵ����ļ�
    static bool copy(const std::string& sourceFile, const std::string& destFile, bool overwrite = false) {
        if (!canRead(sourceFile)) {
            //Դ�ļ������ڻ򲻿ɶ�
            return false;
        }
        if (overwrite && exists(destFile)) {
            //Ŀ���ļ�����
            if (!canWrite(destFile)) {
                //����д
                return false;
            }
        }
        //���ļ���
        std::fstream readStream;
        //д�ļ���
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
    //�Ƿ����ָ���ļ�
    //@filename:�ļ���
    //@return:true ����
    static bool exists(const std::string& filename) {
        return (0 == access(filename.c_str(), 0));
    }

    //////////////////////////////////////////////////////////////////////////
    //ɾ��ָ���ļ�
    //@filename:�ļ���
    static bool remove(const std::string& filename) {
        if (!exists(filename)) {
            //������
            return false;
        }
        return (0 == unlink(filename.c_str()));
    }

    //////////////////////////////////////////////////////////////////////////
    //�ƶ��ļ�
    //@sourceFile:Դ�ļ�
    //@destFile:Ŀ���ļ�
    static bool move(const std::string& sourceFile, const std::string& destFile) {
        return (0 == rename(sourceFile.c_str(), destFile.c_str()));
    }

    //////////////////////////////////////////////////////////////////////////
    //�Ƿ�ɶ�
    //@path:·��
    //@return:true �ɶ�
    static bool canRead(const std::string& path) {
        if (!exists(path)) {
            //�ļ�������
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
    //�Ƿ��д
    //@path:·��
    //@return:true ��д
    static bool canWrite(const std::string& path) {
        if (!exists(path)) {
            //�ļ�������
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
    //��ȡ�ļ���С
    //@path:·��
    //@return:0 = ������
    static size_t getSize(const std::string& path) {
        if (!exists(path)) {
            return 0;
        }
        struct stat status;
        if (0 == stat(path.c_str(), &status)) {
            //��ȡ�ɹ�
            return status.st_size;
        } else {
            //��ȡʧ��
            return 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////
    //���������������ļ�
    //@condition:������������"c:/windows/*.*"
    //@files:�����������ļ������ϣ�����·��
    //@return:true �з����������ļ�
    static bool search(const std::string& condition, std::vector< std::string >& files) {
        files.clear();
        DIR *dir;
        bool hasS = false;
        do {
            if(!(dir = opendir(condition.c_str()))) { //�� /  *  *.*  *.c  a.*  a ��β�����
                // ��������"*"�����,ȡ�����һ��'/'�Ժ�Ķ���,����һ��
                // �������ļ�
                std::string strLast = condition.substr(condition.find_last_of("/")+1);
                if ((int)strLast.find("*") >= 0) {
                    //����һ��
                    strLast = condition.substr(0, condition.find_last_of("/"));
                    if (!(dir=opendir(strLast.c_str()))) {
                        //ʧ����
                        return false;
                    } else {
                        hasS = true;
                        break;//����do while(0);
                    }
                } else if((int)strLast.size()>0) { //ԭ����û��*��,������/a����ʽ,�����ǲ����ļ�
                    if (access(condition.c_str(),0) != 0) { //���ļ�Ҳ����
                        return false;
                    } else { //���ļ�����
                        files.push_back(condition.substr(condition.find_last_of("/")+1));
                        return true;
                    }

                } else {	//  /
                    return false;
                }
            }
        } while(0);
        //���ļ�������
        closedir(dir);

        //cout<<"OK"<<endl;
        struct dirent ** namelist;
        int n;
        std::string tmpstr = condition.substr(condition.find_last_of("/")+1);//ȡ��Ŀ¼��
        if (!hasS || (hasS && (tmpstr=="*" || tmpstr=="*.*"))) {	// /a  /a/ *  *.*��Ŀ¼�µĶ���ȫ����ʾ����
            std::string dentry ;
            //��*�Ļ�ȥ��
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
