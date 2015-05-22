// ShareMemory.cpp: implementation of the ShareMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "sharememory.h"
#include <cstdio>
#include <cstdlib>
#ifndef WIN32
#include <sys/time.h>
#endif

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ShareMemory::ShareMemory(const char *key, unsigned long size, const char *path)
{
    Init();
    if ( NULL != key ) {
        m_shareName = key;
    }
    if ( 0 < size ) {
        m_dwSize = size;
    }
    if ( NULL != path ) {
        m_mapFilePath = path;
    }
    //FILE_MAP_ALL_ACCESS = FILE_MAP_WRITE|FILE_MAP_READ
    if ( !Open(FILE_MAP_ALL_ACCESS) ) {
        Create();
        Open(FILE_MAP_ALL_ACCESS);
    }

}

ShareMemory::ShareMemory(const int key, unsigned long size, const char *path)
{
    Init();
    m_digName = key;
    if ( 0 < m_digName ) {
        char strnum[256];
        sprintf( strnum, "%u", m_digName );
        m_shareName = strnum;
    }
    if ( 0 < size ) {
        m_dwSize = size;
    }
    if ( NULL != path ) {
        m_mapFilePath = path;
    }
    if ( !Open(FILE_MAP_ALL_ACCESS) ) {
#ifndef WIN32
        if ( "" != m_mapFilePath ) {
            return;
        }
#endif
        if ( !Create() ) {
            return;
        }
        Open(FILE_MAP_ALL_ACCESS);
    }
}

ShareMemory::~ShareMemory()
{
    Close();
}

void ShareMemory::Init()
{
#if defined(_WIN32) || defined(WIN32)
    m_hFile = NULL;
    m_hFileMap = NULL;
#else
    m_shmid = -1;
#endif
    m_lpFileMapBuffer = NULL;
    m_dwSize = 8;
    m_shareName = "_DEFAULT_SMB_";
    m_mapFilePath = "";
    m_digName = -1;
}

bool ShareMemory::Create()
{
#if defined(_WIN32) || defined(WIN32)
    m_hFile = (HANDLE)0xFFFFFFFF;// system
    if ("" != m_mapFilePath) {
        string strFile = m_mapFilePath + "/" + m_shareName;
        // file
        m_hFile = CreateFile(
                      strFile.c_str(),
                      GENERIC_READ|GENERIC_WRITE,
                      FILE_SHARE_READ|FILE_SHARE_WRITE,
                      NULL,
                      OPEN_ALWAYS,//OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL
                  );
        if ( INVALID_HANDLE_VALUE == m_hFile ) {
            m_lastError = "open file error:" + strFile;
            return false;
        }
    }
    m_hFileMap = CreateFileMapping( m_hFile, NULL, PAGE_READWRITE,
                                    0, m_dwSize, m_shareName.c_str() );
    if (NULL == m_hFileMap) {
        m_lastError = "create file mapping error";
        return false;
    }
#else
    if ( -1 == m_digName ) {
        if ( "" == m_shareName ) {
            m_lastError = "key is null";
            return false;
        }
        m_digName = atoi( m_shareName.c_str() );
        if ( 0 >= m_digName ) {
            m_lastError = "key havo to bigger than 0";
            return false;
        }
    }
    m_shmid = shmget( m_digName, m_dwSize, IPC_CREAT|IPC_EXCL );
    if ( -1 == m_shmid ) {
        if ( EEXIST == errno ) {
            m_shmid = shmget( m_digName, m_dwSize, 0666 );
        }
        if ( -1 == m_shmid ) {
            m_lastError = "open error:";
            if ( EINVAL == errno ) {
                m_lastError += "size < SHMMIN or size > SHM-MAX";
                m_lastError += ", or segment is exist, but size is small than the new size";
            }
            if ( EIDRM == errno ) {
                m_lastError += "segment is delete";
            }
            if ( ENOSPC == errno ) {
                m_lastError += "requested size would cause the  system  to exceed the system-wide limit on shared memory (SHMALL)";
            }
            if ( ENOENT == errno ) {
                m_lastError += "No segment exists";
            }
            if ( EACCES == errno ) {
                m_lastError += "not have permission to access";
            }
            if ( ENOMEM == errno ) {
                m_lastError += "No memory could be allocated for segment overhead";
            }
            if ( ENFILE == errno ) {
                m_lastError += "open too many file";
            }
            return false;
        }
    }
#endif
    return true;
}

bool ShareMemory::Open(unsigned long dwAccess)
{
#if defined(_WIN32) || defined(WIN32)
    if (NULL == m_hFileMap) {
        m_hFileMap = OpenFileMapping( dwAccess, TRUE, m_shareName.c_str() );
    }
    if (NULL == m_hFileMap) {
        return false;
    }
    m_lpFileMapBuffer = MapViewOfFile(m_hFileMap, dwAccess, 0, 0, 0);
    if ( NULL == m_lpFileMapBuffer ) {
        Close();
        return false;
    }
    MEMORY_BASIC_INFORMATION buffer;
    int nRet = VirtualQuery( m_lpFileMapBuffer, &buffer, sizeof(buffer));
    if (0 == nRet) {
        Close();
        return false;
    }
    m_dwSize = buffer.RegionSize;
#else
    if ( "" != m_mapFilePath ) {
        return OpenFileMap();
    }
    if ( -1 == m_digName ) {
        if ( "" == m_shareName ) {
            return false;
        }
        m_digName = atoi( m_shareName.c_str() );
        if ( 0 >= m_digName ) {
            return false;
        }
    }
    m_shmid = shmget( m_digName, 0, 0666 );
    if ( -1 == m_shmid ) {
        return false;
    }
    m_lpFileMapBuffer = shmat( m_shmid, 0, 0 );
    shmctl( m_shmid, IPC_STAT, &m_ds );
    m_dwSize = m_ds.shm_segsz;
#endif

    return true;
}

bool ShareMemory::CheckDir( const char *strDir )
{
#if defined(_WIN32) || defined(WIN32)
    return false;
#else
    if (-1 == access( strDir, 0 )) {
        umask(0);
        if( 0 != mkdir(strDir, 0777) ) {
            return false;
        }
    }
    umask(0);
    chmod(strDir,0777);
    return true;
#endif
}

bool ShareMemory::CheckFile( const char *strFile )
{
#if defined(_WIN32) || defined(WIN32)
    return false;
#else
    umask(0);
    chmod(strFile,0777);
    return true;
#endif
}

bool ShareMemory::OpenFileMap()
{
#if defined(_WIN32) || defined(WIN32)
    return false;
#else
    CheckDir( m_mapFilePath.c_str() );
    string strFile = m_mapFilePath + "/" + m_shareName;
    CheckFile( strFile.c_str() );
    int fd;
    fd = open(strFile.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if ( -1 == fd ) {
        m_lastError = "open file error:" + strFile;
        return false;
    }
    struct stat sb;
    fstat(fd,&sb);
    if ( sb.st_size < (long)m_dwSize ) {
        lseek(fd, m_dwSize, SEEK_SET);
        write(fd, "\0", 1);
        lseek(fd, 0, SEEK_SET);
        fstat(fd,&sb);
        m_dwSize = sb.st_size;
    }

    m_lpFileMapBuffer = mmap(0,
                             m_dwSize,
                             PROT_WRITE|PROT_READ, //允许写入
                             MAP_SHARED,//写入内容被立即写入到文件
                             fd,
                             0);
    close(fd);
    if ( NULL == m_lpFileMapBuffer ) {
        m_lastError = "create file map error";
        return false;
    }
    return true;
#endif
}

void ShareMemory::Close()
{
#if defined(_WIN32) || defined(WIN32)
    if (NULL != m_lpFileMapBuffer) {
        UnmapViewOfFile(m_lpFileMapBuffer);
    }
    if (NULL != m_hFileMap) {
        CloseHandle(m_hFileMap);
    }
    if (NULL != m_hFile && INVALID_HANDLE_VALUE != m_hFile) {
        CloseHandle(m_hFile);
    }
#else
    if ( "" != m_mapFilePath && NULL != m_lpFileMapBuffer ) {
        munmap(m_lpFileMapBuffer, m_dwSize);
    } else if (NULL != m_lpFileMapBuffer) {
        shmdt(m_lpFileMapBuffer);
    }
#endif
    Init();
}


void ShareMemory::Destory()
{
#if defined(_WIN32) || defined(WIN32)
    Close();
#else
    if ( -1 != m_shmid ) {
        Close();
        shmctl( m_shmid, IPC_RMID, 0 );
    } else if ( "" != m_mapFilePath ) {
        string strFile = m_mapFilePath + "/" + m_shareName;
        Close();
        remove(strFile.c_str());
    }
#endif
}

void* ShareMemory::GetBuffer()
{
    return m_lpFileMapBuffer;
}

unsigned long ShareMemory::GetSize()
{
    return NULL == m_lpFileMapBuffer?0:m_dwSize;
}
