/***************************************
* @file     mutex.cpp
* @brief    ��ƽ̨������
* @details  ���̲߳�����Ҫ���⣬�û����Ǵ�opencv2.4.6(modules\core\src\system.cpp)�и��������ġ�ʵ����
			CV_XADD -- ԭ�Ӳ�����add
			Mutex::Impl--�������ʵ�ʲ��֣�winʹ��CRITICAL_SECTION
										   appleʹ��OSSpinLock
										   linuxʹ��pthread_spinlock_t
										   android&����ƽ̨ʹ��pthread_mutex_t
		    Mutex--ʹ�����ü����Ļ������࣬���໥��ֵ,��void lock();bool trylock();void unlock();������Ա����
			AutoLock--����һmutex,������ʱlock,����ʱunlock.��ʵ���Զ�������֧�ֿ����븳ֵ
			CriticalSection--(opencv2.4.6 modules\core\src\alloc.cpp),û���ü����Ļ������ࡣ
			AutoLockCriticalSection--(opencv2.4.6 modules\core\src\alloc.cpp),����һCriticalSection,������ʱlock,����ʱunlock.��ʵ���Զ�������֧�ֿ����븳ֵ
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-2
****************************************/
#ifndef MUTEX_H
#define MUTEX_H
#include <string.h>
#include <limits.h>
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

/////////////////////////CV_XADD�������忪ʼ/////////////////////////////////////
/////// exchange-add operation for atomic operations on reference counters ///////
#if defined __INTEL_COMPILER && !(defined WIN32 || defined _WIN32)   // atomic increment on the linux version of the Intel(tm) compiler
#define CV_XADD(addr,delta) _InterlockedExchangeAdd(const_cast<void*>(reinterpret_cast<volatile void*>(addr)), delta)
#elif defined __GNUC__

#if defined __clang__ && __clang_major__ >= 3 && !defined __ANDROID__
#ifdef __ATOMIC_SEQ_CST
#define CV_XADD(addr, delta) __c11_atomic_fetch_add((_Atomic(int)*)(addr), (delta), __ATOMIC_SEQ_CST)
#else
#define CV_XADD(addr, delta) __atomic_fetch_add((_Atomic(int)*)(addr), (delta), 5)
#endif
#elif __GNUC__*10 + __GNUC_MINOR__ >= 42

#if !(defined WIN32 || defined _WIN32) && (defined __i486__ || defined __i586__ || \
	defined __i686__ || defined __MMX__ || defined __SSE__  || defined __ppc__) || \
	(defined __GNUC__ && defined _STLPORT_MAJOR)
#define CV_XADD __sync_fetch_and_add
#else
#include <ext/atomicity.h>
#define CV_XADD __gnu_cxx::__exchange_and_add
#endif

#else
#include <bits/atomicity.h>
#if __GNUC__*10 + __GNUC_MINOR__ >= 34
#define CV_XADD __gnu_cxx::__exchange_and_add
#else
#define CV_XADD __exchange_and_add
#endif
#endif

#elif defined WIN32 || defined _WIN32 || defined WINCE
int _interlockedExchangeAdd(int* addr, int delta);
#define CV_XADD _interlockedExchangeAdd

#else
static inline int CV_XADD(int* addr, int delta)
{
    int tmp = *addr;
    *addr += delta;
    return tmp;
}
#endif

/////////////////////////Mutex���忪ʼ/////////////////////////////////////
class Mutex
{
public:
    Mutex();
    ~Mutex();
    Mutex(const Mutex& m);
    Mutex& operator = (const Mutex& m);

    void lock();
    bool trylock();
    void unlock();

    struct Impl;
protected:
    Impl* impl;
};

/////////////////////////AutoLock���忪ʼ/////////////////////////////////////
class AutoLock
{
public:
    explicit AutoLock(Mutex& m) : mutex(&m) {
        mutex->lock();
    }
    ~AutoLock() {
        mutex->unlock();
    }
protected:
    Mutex* mutex;
private:
    AutoLock(const AutoLock&);
    AutoLock& operator = (const AutoLock&);
};

////////////////////�޼�����CriticalSection���忪ʼ/////////
#ifdef WIN32
class CriticalSection
{
public:
    CriticalSection() {
        InitializeCriticalSection(&cs);
    }
    ~CriticalSection() {
        DeleteCriticalSection(&cs);
    }
    void lock() {
        EnterCriticalSection(&cs);
    }
    void unlock() {
        LeaveCriticalSection(&cs);
    }
    bool trylock() {
        return TryEnterCriticalSection(&cs) != 0;
    }
private:
    CRITICAL_SECTION cs;
};
#elif defined __APPLE__
#include <libkern/OSAtomic.h>
class CriticalSection
{
public:
    CriticalSection() {
        sl = OS_SPINLOCK_INIT;
    }
    ~CriticalSection() {
        pthread_mutex_destroy(&mutex);
    }
    void lock() {
        OSSpinLockLock(&sl);
    }
    void unlock() {
        OSSpinLockUnlock(&sl);
    }
    bool trylock() {
        return OSSpinLockTry(&sl);
    }
private:
    OSSpinLock sl;
};
#elif defined __linux__ && !defined ANDROID
class CriticalSection
{
public:
    CriticalSection() {
        pthread_spin_init(&sl, 0);
    }
    ~CriticalSection() {
        pthread_spin_destroy(&sl);
    }
    void lock() {
        pthread_spin_lock(&sl);
    }
    void unlock() {
        pthread_spin_unlock(&sl);
    }
    bool trylock() {
        return pthread_spin_trylock(&sl) == 0;
    }
private:
    pthread_spinlock_t sl;
};
#else //WIN32

class CriticalSection
{
public:
    CriticalSection() {
        pthread_mutex_init(&mutex, 0);
    }
    ~CriticalSection() {
        pthread_mutex_destroy(&mutex);
    }
    void lock() {
        pthread_mutex_lock(&mutex);
    }
    void unlock() {
        pthread_mutex_unlock(&mutex);
    }
    bool trylock() {
        return pthread_mutex_trylock(&mutex) == 0;
    }
private:
    pthread_mutex_t mutex;
};
#endif //WIN32

////////////////////AutoLockCriticalSection���忪ʼ/////////
class AutoLockCriticalSection
{
public:
    AutoLockCriticalSection(CriticalSection& _cs) : cs(&_cs) {
        cs->lock();
    }
    ~AutoLockCriticalSection() {
        cs->unlock();
    }
private:
    CriticalSection* cs;
private:
    AutoLockCriticalSection(const AutoLockCriticalSection&);
    AutoLockCriticalSection& operator = (const AutoLockCriticalSection&);
};

#endif  //MUTEX_H
