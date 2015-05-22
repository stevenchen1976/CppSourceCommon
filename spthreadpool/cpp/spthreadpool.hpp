/***************************************
* @file     spthreadpool.hpp
* @brief    线程池C++实现。作者：http://iunknown.iteye.com/。
　　　　　　位于项目http://code.google.com/p/spserver/中，有修改
	        作者博客介绍http://iunknown.iteye.com/blog/59804
* @details  version 0.9.5 (12.13.2009)
* @author   phata, wqvbjhc@gmail.com
* @date     2013-11-8
****************************************/
/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */


#ifndef __spthreadpool_hpp__
#define __spthreadpool_hpp__

#include "spthread.hpp"

typedef struct tagSP_Thread SP_Thread_t;

class SP_ThreadPool
{
public:
    typedef void ( * DispatchFunc_t )( void * );

    SP_ThreadPool( int maxThreads, const char * tag = 0 );
    ~SP_ThreadPool();

    /// @return 0 : OK, -1 : cannot create thread
    int dispatch( DispatchFunc_t dispatchFunc, void *arg );

    int getMaxThreads();

private:
    char * mTag;

    int mMaxThreads;
    int mIndex;
    int mTotal;
    int mIsShutdown;

    sp_thread_mutex_t mMainMutex;
    sp_thread_cond_t mIdleCond;
    sp_thread_cond_t mFullCond;//<mThreadList full
    sp_thread_cond_t mEmptyCond;

    SP_Thread_t ** mThreadList;//<空闲的线程

    static sp_thread_result_t SP_THREAD_CALL wrapperFunc( void * );
    int saveThread( SP_Thread_t * thread );
};

#endif

