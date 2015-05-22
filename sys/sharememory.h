/***************************************
* @file     sharememory.h
* @brief    跨平台共享内存文件映射类,出处:http://www.oschina.net/code/snippet_732357_15901
* @details
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-4
****************************************/

#ifndef SHAREMEMORY_H
#define SHAREMEMORY_H

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/shm.h>// for void *shmat(int shmid, const void *shmaddr, int shmflg);
#include <sys/ipc.h>// for int shmget(key_t key, size_t size, int shmflg);
#include <unistd.h>

#include<sys/stat.h>
#include<fcntl.h>
#include <sys/mman.h>// for void *mmap(void*start,size_t length,int prot,int flags,int fd,off_toffsize);

#include <errno.h> //for errno
#include <error.h>

#define FILE_MAP_READ 1
#define FILE_MAP_WRITE 2
#define FILE_MAP_ALL_ACCESS 3
#endif
#include <string>

class ShareMemory
{
private:
#if defined(_WIN32) || defined(WIN32)
    HANDLE		m_hFile;
    HANDLE		m_hFileMap;
#else
    int m_shmid;
    shmid_ds m_ds;
#endif
    void*			m_lpFileMapBuffer;
    std::string		m_shareName;
    int				m_digName;
    unsigned long	m_dwSize;
    std::string		m_mapFilePath;
    std::string		m_lastError;
public:
    /*
     *	创建/打开共享内存
     *	参数
     *		key	全局标识，linux下只接收数字id
     *		size 共享大小
     *			if ( 是创建 )
     *				则创建size大小的共享内存，
     *			else //是打开已存在的共享内存
     *				if ( 是文件映射 && 是linux系统 )
     *	 				则使用当前共享内存大小与size中较大的一个,作为共享内存的大小
     *				else 无效
     *			else 无效
     *		path 使用文件映射的共享内存，文件路径，key为文件名
     */
    ShareMemory(const char *key, unsigned long size, const char *path);
    /*
     *	创建/打开共享内存
     *	参数
     *		key	全局标识
     *		size 共享大小
     *			if ( 是创建 )
     *				则创建size大小的共享内存，
     *			else //是打开已存在的共享内存
     *				if ( 是文件映射 && 是linux系统 )
     *	 				则使用当前共享内存大小与size中较大的一个,作为共享内存的大小
     *				else 无效
     *			else 无效
     *		path 使用文件映射的共享内存，文件路径，key为文件名
    */
    ShareMemory(const int key, unsigned long size, const char *path);
    virtual ~ShareMemory();

private:
public:
    void* GetBuffer();
    unsigned long GetSize();
    void Destory();


private:
    void Init();
    bool Create();
    bool Open(unsigned long dwAccess);
    void Close();

    bool OpenFileMap();
    bool CheckDir( const char *strIPCDir );
    bool CheckFile( const char *strIPCFile );
};
/************************************测试案例

//	使用系统内存创建共享内存测试
//	VMware xin
//		409600byte数据
//		1w次read耗时953~1344毫秒
//		1w次write耗时1015~1297毫秒
//	VMware linux
//		40960byte数据
//		10w次read耗时554~690毫秒
//		10w次write耗时527~636毫秒
static void TestFile()
{
#ifdef WIN32
    ShareMemory a("12", 409600, "E:\\Lib\\HuoYu\\HYLib\\part_test\\smtest");
    char sss[409600];
    SYSTEMTIME t1,t2;
    char *p = (char*)a.GetBuffer();

    memset(p,1,409600);
    memset(sss,100,409600);
    while ( true ) {
        int i = 0;
        GetSystemTime(&t1);
        int j = 0;
        for ( j = 0; j < 10000; j++) {
            memcpy( p, sss, 409600 );//写
//			memcpy( sss, p, 409600 );//读
        }
        GetSystemTime(&t2);
        int h = (t2.wHour - t1.wHour)*3600;
        int m = (t2.wMinute - t1.wMinute) * 60;
        int s = t2.wSecond - t1.wSecond;
        s = h + m + s;
        int ns = t2.wMilliseconds - t1.wMilliseconds;
        s = s * 1000 + ns;
        float ft = s / 1000.0;
        printf( "%f\n", ft );
    }
#else
    printf( "TestFile\n" );
    ShareMemory a("999", 40960, "./");
    char sss[409600];
    char *p = (char*)a.GetBuffer();
    struct timeval tEnd,tStart;
    struct timezone tz;
    memset(p,'a',40960);
    memset(sss,'a',40960);
    while ( true ) {
        int j = 0;
        gettimeofday (&tStart , &tz);
        for ( j = 0; j < 100000; j++) {
            memcpy( p, sss, 40960 );//写
//			memcpy( sss, p, 40960 );//读
        }
        gettimeofday (&tEnd , &tz);
        printf("time:%ld\n", (tEnd.tv_sec-tStart.tv_sec)*1000+(tEnd.tv_usec-tStart.tv_usec)/1000);
    }
#endif
}


//	使用映射文件创建共享内存测试
//	VMware xin
//		409600byte数据
//		1w次read耗时953~1344毫秒
//		1w次write耗时1015~1391毫秒
//	VMware linux
//		40960byte数据
//		10w次read耗时582~691毫秒
//		10w次write耗时560~652毫秒

static void TestSystem()
{
#ifdef WIN32
    ShareMemory a(NULL, 409600, NULL);
    char *p = (char*)a.GetBuffer();
    char sss[409600];
    SYSTEMTIME t1,t2;

    memset(p,1,409600);
    memset(sss,100,409600);
    while ( true ) {
        int i = 0;
        GetSystemTime(&t1);
        int j = 0;
        for ( j = 0; j < 10000; j++) {
            memcpy( p, sss, 409600 );//写
//			memcpy( sss, p, 409600 );//读
        }
        GetSystemTime(&t2);
        int h = (t2.wHour - t1.wHour)*3600;
        int m = (t2.wMinute - t1.wMinute) * 60;
        int s = t2.wSecond - t1.wSecond;
        s = h + m + s;
        int ns = t2.wMilliseconds - t1.wMilliseconds;
        s = s * 1000 + ns;
        float ft = s / 1000.0;
        printf( "%f\n", ft );
    }
#else
    ShareMemory a("1234", 40960, NULL);
    char *p = (char*)a.GetBuffer();
    char sss[409600];

    struct timeval tEnd,tStart;
    struct timezone tz;
    memset(p,'d',40960);
    memset(sss,'d',40960);
    while ( true ) {
        int j = 0;
        gettimeofday (&tStart , &tz);
        for ( j = 0; j < 100000; j++) {
//			memcpy( p, sss, 40960 );//写
            memcpy( sss, p, 40960 );//读
        }
        gettimeofday (&tEnd , &tz);
        printf("time:%ld\n", (tEnd.tv_sec-tStart.tv_sec)*1000+(tEnd.tv_usec-tStart.tv_usec)/1000);
    }

#endif
}



//	内存复制测试
//	VMware xin
//		409600byte数据
//		1w次read耗时968~1422毫秒
//		1w次write耗时969~1766毫秒
//	VMware linux
//		40960byte数据
//		10w次read耗时1021~1170毫秒
//		10w次write耗时990~1090毫秒

static void TestMemory()
{
#ifdef WIN32
    ShareMemory a(NULL, 409600, NULL);
    char *p = new char[409600];
    char sss[409600];
    SYSTEMTIME t1,t2;
    memset(p,1,409600);
    memset(sss,100,409600);
    while ( true ) {
        int i = 0;
        GetSystemTime(&t1);
        int j = 0;
        for ( j = 0; j < 10000; j++) {
            memcpy( p, sss, 409600 );//写
//			memcpy( sss, p, 409600 );//读
        }
        GetSystemTime(&t2);
        int h = (t2.wHour - t1.wHour)*3600;
        int m = (t2.wMinute - t1.wMinute) * 60;
        int s = t2.wSecond - t1.wSecond;
        s = h + m + s;
        int ns = t2.wMilliseconds - t1.wMilliseconds;
        s = s * 1000 + ns;
        float ft = s / 1000.0;
        printf( "%f\n", ft );
    }
#else
    printf( "TestMemory\n" );
    char *p = new char[40960];
    char sss[409600];
    struct timeval tEnd,tStart;
    struct timezone tz;
    memset(p,'s',40960);
    memset(sss,'s',40960);

    while ( true ) {
        int j = 0;
        gettimeofday (&tStart , &tz);
        for ( j = 0; j < 100000; j++) {
            memcpy( p, sss, 40960 );//写
//			memcpy( sss, p, 40960 );//读
        }
        gettimeofday (&tEnd , &tz);
        printf("time:%ld\n", (tEnd.tv_sec-tStart.tv_sec)*1000+(tEnd.tv_usec-tStart.tv_usec)/1000);
    }
#endif
}
********************************/

#endif // !defined SHAREMEMORY_H
