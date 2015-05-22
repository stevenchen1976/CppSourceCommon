#include "mutex.h"



/////////////////////////Mutex::Impl定义开始/////////////////////////////////////
#if defined WIN32 || defined _WIN32 || defined WINCE

struct Mutex::Impl {
    Impl() {
        InitializeCriticalSection(&cs);
        refcount = 1;
    }
    ~Impl() {
        DeleteCriticalSection(&cs);
    }

    void lock() {
        EnterCriticalSection(&cs);
    }
    bool trylock() {
        return TryEnterCriticalSection(&cs) != 0;
    }
    void unlock() {
        LeaveCriticalSection(&cs);
    }

    CRITICAL_SECTION cs;
    int refcount;
};

#ifndef __GNUC__
int _interlockedExchangeAdd(int* addr, int delta)
{
#if defined _MSC_VER && _MSC_VER >= 1500
    return (int)_InterlockedExchangeAdd((long volatile*)addr, delta);
#else
    return (int)InterlockedExchangeAdd((long volatile*)addr, delta);
#endif
}
#endif // __GNUC__

#elif defined __APPLE__

#include <libkern/OSAtomic.h>

struct Mutex::Impl {
    Impl() {
        sl = OS_SPINLOCK_INIT;
        refcount = 1;
    }
    ~Impl() {}

    void lock() {
        OSSpinLockLock(&sl);
    }
    bool trylock() {
        return OSSpinLockTry(&sl);
    }
    void unlock() {
        OSSpinLockUnlock(&sl);
    }

    OSSpinLock sl;
    int refcount;
};

#elif defined __linux__ && !defined ANDROID

struct Mutex::Impl {
    Impl() {
        pthread_spin_init(&sl, 0);
        refcount = 1;
    }
    ~Impl() {
        pthread_spin_destroy(&sl);
    }

    void lock() {
        pthread_spin_lock(&sl);
    }
    bool trylock() {
        return pthread_spin_trylock(&sl) == 0;
    }
    void unlock() {
        pthread_spin_unlock(&sl);
    }

    pthread_spinlock_t sl;
    int refcount;
};

#else

struct Mutex::Impl {
    Impl() {
        pthread_mutex_init(&sl, 0);
        refcount = 1;
    }
    ~Impl() {
        pthread_mutex_destroy(&sl);
    }

    void lock() {
        pthread_mutex_lock(&sl);
    }
    bool trylock() {
        return pthread_mutex_trylock(&sl) == 0;
    }
    void unlock() {
        pthread_mutex_unlock(&sl);
    }

    pthread_mutex_t sl;
    int refcount;
};

#endif

/////////////////////////Mutex实现开始/////////////////////////////////////
Mutex::Mutex()
{
    impl = new Mutex::Impl;
}

Mutex::~Mutex()
{
    if( CV_XADD(&impl->refcount, -1) == 1 ) {
        delete impl;
    }
    impl = 0;
}

Mutex::Mutex(const Mutex& m)
{
    impl = m.impl;
    CV_XADD(&impl->refcount, 1);
}

Mutex& Mutex::operator = (const Mutex& m)
{
    CV_XADD(&m.impl->refcount, 1);
    if( CV_XADD(&impl->refcount, -1) == 1 ) {
        delete impl;
    }
    impl = m.impl;
    return *this;
}

void Mutex::lock()
{
    impl->lock();
}
void Mutex::unlock()
{
    impl->unlock();
}
bool Mutex::trylock()
{
    return impl->trylock();
}
