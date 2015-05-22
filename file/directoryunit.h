/***************************************
* @file     directoryunit.h
* @brief    ��Ŀ¼��صĲ�����������
* @details	Directory�ࣺ�����¼�����̬����
				combine--�������·��
				getDirectoryName--��ȡĿ¼��
				changeExtension--�޸�·����׺
				getExtension--��ȡ��׺
				getFileName--��ȡ�ļ���
				getFileNameWithoutExtension--��ȡ������׺���ļ���
				getFullPath--��ȡ����·��
				makePath--����·�����Զ������༶·����
				getSubDirs--��ȡָ��Ŀ¼�µ�������Ŀ¼
				removePath--ɾ��ָ����Ŀ¼
				exists --�ж�Ŀ¼�Ƿ����
				getExePath--����exe��dll����Ŀ¼,\��β
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-4
* @mod      2014-11-13 phata ���exists����
            2015-03-11 phata ���getExePath����
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


#if defined __linux__ || defined __APPLE__
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
/////////////////////////////////////////////////////////��ƽ̨ͷ�ļ�����-����////////////////////////
#include <stdarg.h>
#include <direct.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <sstream>

//////////////////////////////////////////////////////////////////////////
//·�����ƽ���
#if defined WIN32 || defined _WIN32

//·�����ƽ���
class Directory
{
    class CFileInfo
    {
    public:
        //std::string m_strName;
        std::string m_strFile;  //�ļ�����������·����
        //std::string m_strPath;
        int m_iType;
    };
public:
    //////////////////////////////////////////////////////////////////////////
    //�������·��
    static std::string combine(const std::string& path1, const std::string& path2)
    {
        std::stringstream cache;
        cache << path1 << getPathSeparator() << path2;
        return cache.str();
    }
    //////////////////////////////////////////////////////////////////////////
    //��ȡĿ¼��
    static std::string getDirectoryName(const std::string& path)
    {
        //�ݹ���ã�ȥ����windows��ʽ�ָ���
        if (hasNonwinFormatSeparator(path)) {
            return getDirectoryName(formatPathSeparator(path));
        }
        std::string::size_type offset = getPathSeparatorOffset(path);
        if (std::string::npos == offset) {
            //û��·����
            return "";
        }
        std::string::size_type drive = path.find(":\\");
        if (std::string::npos == drive) {
            return path.substr(0, offset);
        } else {
            if (drive == (offset - 1)) {
                //ֻ���̷���û��·��
                return "";
            }
            //���̷�
            std::string::size_type length = offset - drive - 2;
            return path.substr(drive + 2, length);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //�޸�·����׺
    static std::string changeExtension(const std::string& path, const std::string& ext)
    {
        std::string filename = getFileNameWithoutExtension(path);
        if (0 == filename.size()) {
            //���ļ���
            return path;
        }
        std::stringstream cache;
        cache << getFullPath(path);
        cache << filename;
        cache << getFileSeparator() << ext;
        return cache.str();
    }
    //////////////////////////////////////////////////////////////////////////
    //��ȡ��׺
    static std::string getExtension(const std::string& path)
    {
        std::string::size_type offset = getFileSeparatorOffset(path);
        if (std::string::npos == offset) {
            //û�м����
            return "";
        }
        return path.substr(offset + 1, path.length() - offset - 1);
    }
    //////////////////////////////////////////////////////////////////////////
    //��ȡ�ļ���
    static std::string getFileName(const std::string& path)
    {
        //�ݹ���ã�ȥ����windows��ʽ�ָ���
        if (hasNonwinFormatSeparator(path)) {
            return getFileName(formatPathSeparator(path));
        }
        std::string::size_type offset = getPathSeparatorOffset(path);
        if (std::string::npos == offset) {
            //û��·����
            return path;
        }
        return path.substr(offset + 1, path.length() - offset - 1);
    }
    //////////////////////////////////////////////////////////////////////////
    //��ȡ������׺���ļ���
    static std::string getFileNameWithoutExtension(const std::string& path)
    {
        //�ݹ���ã�ȥ����windows��ʽ�ָ���
        if (hasNonwinFormatSeparator(path)) {
            return getFileNameWithoutExtension(formatPathSeparator(path));
        }
        size_t extOffset = getFileSeparatorOffset(path);
        if (std::string::npos == extOffset) {
            //û����չ�������ļ�����ͬ
            return getFileName(path);
        }
        size_t pathOffset = getPathSeparatorOffset(path);
        if (std::string::npos == pathOffset) {
            //û��·����
            return path.substr(0, extOffset);
        }
        size_t length = extOffset - pathOffset - 1;
        return path.substr(pathOffset + 1, length);
    }
    //////////////////////////////////////////////////////////////////////////
    //��ȡ����·��
    static std::string getFullPath(const std::string& path)
    {
        //�ݹ���ã�ȥ����windows��ʽ�ָ���
        if (hasNonwinFormatSeparator(path)) {
            return getFullPath(formatPathSeparator(path));
        }
        std::string::size_type offset = getPathSeparatorOffset(path);
        if (std::string::npos == offset) {
            //û��·����
            return "";
        }
        return path.substr(0, getPathSeparatorOffset(path) + 1);
    }
    //����·�����Զ������༶·����
    //@pathName:·����
    //@return:true�����ɹ�
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
    //��ȡָ��Ŀ¼�µ�������Ŀ¼
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
    //ɾ��ָ����Ŀ¼������Ŀ¼�µ��ļ���
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

    //�ж�Ŀ¼�Ƿ����
    static bool exists(const std::string& dirname)
    {
        struct _stat fileStat;
        if ((_stat(dirname.c_str(), &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
            return true;
        }
        return false;
    }

    //����exe��dll����Ŀ¼,\��β
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
    //��ȡ·�������
    static const char& getPathSeparator()
    {
        static const char pathSep = '\\';
        return pathSep;
    }
    //////////////////////////////////////////////////////////////////////////
    //��ȡ�ļ������
    static const char& getFileSeparator()
    {
        static const char fileSep = '.';
        return fileSep;
    }
    //////////////////////////////////////////////////////////////////////////
    //��ȡ�ļ������ƫ��λ��
    static std::string::size_type getFileSeparatorOffset(const std::string& path)
    {
        return path.find_last_of(getFileSeparator());
    }
    //////////////////////////////////////////////////////////////////////////
    //��ȡ·�������ƫ��λ�ã����Ҳ�·���������
    static std::string::size_type getPathSeparatorOffset(const std::string& path)
    {
        return path.find_last_of(getPathSeparator());
    }
    //////////////////////////////////////////////////////////////////////////
    //·���а�����windows��ʽ�����
    static bool hasNonwinFormatSeparator(const std::string& path)
    {
        return (std::string::npos != path.find('/'));
    }
    //////////////////////////////////////////////////////////////////////////
    //�滻��windows��ʽ�����
    static std::string formatPathSeparator(const std::string& path)
    {
        std::stringstream cache;
        char ch = 0;
        for (size_t offset = 0; offset < path.size(); offset++) {
            ch = path.at(offset);
            if (ch == '/') {
                //�滻Ϊwindows�����
                cache << getPathSeparator();
            } else {
                cache << ch;
            }
        }
        return cache.str();
    }

    //////////////////////////////////////////////////////////////////////////
    //����·���������Զ������༶·����
    //@path:·����
    //@return:true�����ɹ�
    static bool _makePath(const std::string& path)
    {
        return (0 == _mkdir(path.c_str()));
    }

    //ɾ��·��
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
            //û��ƥ���ļ�
            return 0;
        }
        do {
            //���������ļ�
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
    //�������·��
    static std::string combine(const std::string& path1, const std::string& path2);
    //////////////////////////////////////////////////////////////////////////
    //��ȡĿ¼��
    static std::string getDirectoryName(const std::string& path)
    {
        std::string::size_type pos = path.find_last_of("/");
        if (pos == std::string::npos) {
            return path;
        }

        return path.substr(0, pos);
    }
    //////////////////////////////////////////////////////////////////////////
    //�޸�·����׺
    static std::string changeExtension(const std::string& path, const std::string& ext);
    //////////////////////////////////////////////////////////////////////////
    //��ȡ��׺
    static std::string getExtension(const std::string& path);
    //////////////////////////////////////////////////////////////////////////
    //��ȡ�ļ���
    static std::string getFileName(const std::string& path)
    {
        std::string::size_type pos = path.find_last_of("/");
        if (pos == std::string::npos) {
            return path;
        }
        return path.substr(pos + 1);
    }
    //////////////////////////////////////////////////////////////////////////
    //��ȡ������׺���ļ���
    static std::string getFileNameWithoutExtension(const std::string& path)
    {
        std::string::size_type ext_offset = path.find_last_of('.');
        if (std::string::npos == ext_offset) {
            //û�к�׺��
            return getFileName(path);
        }
        std::string::size_type path_offset = path.find_last_of('/');
        if (std::string::npos == path_offset) {
            //û�д�·��
            return path.substr(0, ext_offset);
        }
        size_t length = ext_offset - path_offset - 1;
        return path.substr(path_offset + 1, length);
    }
    //////////////////////////////////////////////////////////////////////////
    //��ȡ����·��
    static std::string getFullPath(const std::string& path);

    //����·�����Զ������༶·����
    //@pathName:·����
    //@return:true�����ɹ�
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

    //�ж�Ŀ¼�Ƿ����
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
    //·���а�����windows��ʽ�����
    static bool haswinFormatSeparator(const std::string& path)
    {
        return (std::string::npos != path.find('\\'));
    }
    //////////////////////////////////////////////////////////////////////////
    //�滻windows��ʽ�����
    static std::string formatPathSeparator(const std::string& path)
    {
        std::stringstream cache;
        char ch = 0;
        for (size_t offset = 0; offset < path.size(); offset++) {
            ch = path.at(offset);
            if (ch == '\\') {
                //�滻Ϊwindows�����
                cache << getPathSeparator();
            } else {
                cache << ch;
            }
        }
        return cache.str();
    }

    //////////////////////////////////////////////////////////////////////////
    //��ȡ·�������
    static const char& getPathSeparator()
    {
        static const char pathSep = '/';
        return pathSep;
    }
    //////////////////////////////////////////////////////////////////////////
    //����·���������Զ������༶·����
    //@path:·����
    //@return:true�����ɹ�
    static bool _makePath(const std::string& path)
    {
        return (0 == mkdir(path.c_str(), 0777));
    }

    //����exe��dll����Ŀ¼
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
