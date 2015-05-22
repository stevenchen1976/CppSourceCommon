/*****************************************
* @file     thread.h
* @brief    出处：作者未定义版权 http://blog.chinaunix.net/uid-24103300-id-125475.html
* @details  与多任务（多线程）有关的平台无关的接口
* @author   phata,wqvbjhc@gmail.com
* @date     2013-4-27
* @mod      2013-6-23:添加CMutex类
            2013-6-24:头文件的函数必须为内联
			          WaitMultiTreadToFinish添加获取线程的返回值
******************************************/
//定义了与多任务（多线程）有关的平台无关的接口
//适合平台：WindowsXP, WindowsCE, Linux, Android
#ifndef COMMON_THREAD_H
#define COMMON_THREAD_H
//启动新任务的方法
//bool CreateNewThread(TypeTask&, TypeTaskDec (*TaskFun)(TypeTaskArg), TypeTaskArg&); //成功返回true
//等待任务结束的方法
//bool WaitThreadToFinish(TypeTask&); //成功返回true
//等待多任务结束的方法
//win下milliseconds为负表永不超时. linux永远是永不超时
//int WaitMultiTreadToFinish(int ncount, TypeTask* taskarry, bool waitall, long milliseconds);//返回第一个结束的任务索引
//任务睡眠一段时间的方法
//void ThreadSleep(unsigned long ms); //睡眠ms毫秒
//得到线程句柄
//TypeTask GetCurThread();

//创建(初始化)互斥量的方法
//bool MutexInit(TypeMutex&); //成功返回true
//销毁互斥量的方法
//bool MutexDestroy(TypeMutex&); //成功返回true
//对互斥量上锁的方法
//bool MutexLock(TypeMutex&); //成功返回true
//对互斥量解锁的方法
//bool MutexUnlock(TypeMutex&); //成功返回true


//对Android平台，也认为是linux
#ifdef ANDROID
#define __linux__ ANDROID
#endif
//包含头文件
#if defined (WIN32) || defined(_WIN32)
#include <windows.h>
#endif
#ifdef __linux__
#include <pthread.h>
#include <unistd.h>
#endif

#if defined (WIN32) || defined(_WIN32)
typedef LPVOID TypeTaskArg;//任务(线程)函数参数类型：TypeTaskArg
#define TypeTaskDec DWORD WINAPI//任务(线程)函数声明类型：TypeTaskDec
typedef DWORD  TypeTaskReturn;//任务(线程)函数返回值类型：TypeTaskReturn
typedef HANDLE  TypeTask;//任务句柄类型：TypeTask

typedef HANDLE   TypeMutex;

#elif defined(__linux__)
typedef void* TypeTaskArg;
#define TypeTaskDec void*
typedef void*  TypeTaskReturn;
typedef pthread_t TypeTask;

typedef pthread_mutex_t TypeMutex;

#else
#error "no supported os"
#endif


//函数
#if defined (WIN32) || defined(_WIN32)
#define CreateNewThread(t, fun, arg) (((t) = CreateThread(NULL, 0, fun, arg, 0, NULL)) != NULL)
#define CloseThread(t) (CloseHandle(t))
#define WaitThreadToFinish(t) ((WaitForSingleObject(t, INFINITE) == WAIT_OBJECT_0) && CloseHandle(t))
#define ThreadSleep(ms) Sleep(ms);//睡眠ms毫秒
#define GetCurThread GetCurrentThread;//得到当前线程句柄
inline int WaitMultiTreadToFinish(int ncount, TypeTask* taskarry, bool waitall, long milliseconds, unsigned long* retcode)//返回第一个结束的任务索引,ret为返回值
{
    int ret=WaitForMultipleObjects(ncount,taskarry,waitall,milliseconds);
    if (retcode) {
        for (int i=0; i< ncount; ++i) {
            GetExitCodeThread(taskarry[i],&retcode[i]);
            CloseHandle(taskarry[i]);
        }
    } else {
        for (int i=0; i< ncount; ++i) {
            CloseHandle(taskarry[i]);
        }
    }
    return ret;
}

#define MutexInit(m) (((m) = CreateMutex(NULL, FALSE, NULL)) != NULL)
#define MutexDestroy(m) CloseHandle(m)
#define MutexLock(m) (WaitForSingleObject(m, INFINITE) == WAIT_OBJECT_0)
#define MutexUnlock(m) ReleaseMutex(m)
#elif defined(__linux__)
#define CreateNewThread(t, fun, arg) (!pthread_create(&(t), NULL, fun, arg))
#define CloseThread(t) ()
#define WaitThreadToFinish(t) (!pthread_join(t, NULL))
#define ThreadSleep(ms) usleep((ms) * 1000)
#define GetCurThread pthread_self;//得到当前线程句柄
inline int WaitMultiTreadToFinish(int ncount, TypeTask* taskarry, bool waitall, long milliseconds,unsigned long* retcode)//返回第一个结束的任务索引
{
    if (retcode) {
        for (int i=0; i< ncount; ++i) {
			void *tmp=&retcode[i];
            pthread_join(taskarry[i],&tmp);
            if(!waitall) {
                break;
            }
        }
    } else {
        for (int i=0; i< ncount; ++i) {
            pthread_join(taskarry[i],NULL);
            if(!waitall) {
                break;
            }
        }
    }
    return ncount;
}

#define MutexInit(m) (!pthread_mutex_init(&(m), NULL))
#define MutexDestroy(m) (!pthread_mutex_destroy(&(m)))
#define MutexLock(m) (!pthread_mutex_lock(&(m)))
#define MutexUnlock(m) (!pthread_mutex_unlock(&(m)))

#else
#error "no supported os"
#endif

//但可以使用CThreadMutex把锁写的更简便一点：
//需要先调用MutexInit
class CThreadMutex
{
public:
    explicit CThreadMutex(TypeMutex &m):mut_(m) {
        MutexLock(mut_);
    }
    ~CThreadMutex(void) {
        MutexUnlock(mut_);
    }
private:
    TypeMutex& mut_;
};
//这样在每个函数开始CMutex g(mutex);

#endif //COMMON_THREAD_H
