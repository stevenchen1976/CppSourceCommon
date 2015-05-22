/***************************************
* @file     threadpool.h
* @brief    线程池C实现。作者：http://iunknown.iteye.com/。
　　　　　　项目http://code.google.com/p/spserver/，位于DownLoad,项目中使用的是C++版本的实现
	        作者博客介绍http://iunknown.iteye.com/blog/38544
* @details  Ver0.2
            适应以下场景：1.需要大量的线程来完成任务，且完成任务的时间比较短。
			不适应以下场景：可能会长时间运行的任务。
* @author   phata, wqvbjhc@gmail.com
* @date     2013-11-8
****************************************/
/**
 * threadpool.h
 *
 * This file declares the functionality associated with
 * your implementation of a threadpool.
 */

#ifndef __threadpool_h__
#define __threadpool_h__

#ifdef __cplusplus
extern "C" {
#endif

// maximum number of threads allowed in a pool
#define MAXT_IN_POOL 200

// You must hide the internal details of the threadpool
// structure from callers, thus declare threadpool of type "void".
// In threadpool.c, you will use type conversion to coerce
// variables of type "threadpool" back and forth to a
// richer, internal type.  (See threadpool.c for details.)

typedef void *threadpool;

// "dispatch_fn" declares a typed function pointer.  A
// variable of type "dispatch_fn" points to a function
// with the following signature:
//
//     void dispatch_function(void *arg);

typedef void (*dispatch_fn)(void *);

/**
 * create_threadpool creates a fixed-sized thread
 * pool.  If the function succeeds, it returns a (non-NULL)
 * "threadpool", else it returns NULL.
 */
threadpool create_threadpool(int num_threads_in_pool);


/**
 * dispatch sends a thread off to do some work.  If
 * all threads in the pool are busy, dispatch will
 * block until a thread becomes free and is dispatched.
 *
 * Once a thread is dispatched, this function returns
 * immediately.
 *
 * The dispatched thread calls into the function
 * "dispatch_to_here" with argument "arg".
 */
int dispatch_threadpool(threadpool from_me, dispatch_fn dispatch_to_here,
                        void *arg);

/**
 * destroy_threadpool kills the threadpool, causing
 * all threads in it to commit suicide, and then
 * frees all the memory associated with the threadpool.
 */
void destroy_threadpool(threadpool destroyme);

#ifdef __cplusplus
}
#endif

#endif

