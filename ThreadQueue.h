/***************************************
* @file     ThreadQueue.h
* @brief    线程安全的队列//出处：http://bbs.chinaunix.net/thread-1748687-3-1.html
* @details  自己实现的线程安全队列
* @author   phata, wqvbjhc@gmail.com
* @date     2013-7-27
* @modify
  2013-9-10    修改为使用mutex.h，去掉thread/thread.h中的mutex		phata
****************************************/
#ifndef THREAD_QUEUE_H
#define THREAD_QUEUE_H
#include <queue>
#include "sys/mutex.h"

template <class Object> class ThreadQueue
{
private:
    std::queue <Object> que;
    Mutex mutex;
public:
    ThreadQueue() :mutex(){
    }
    ~ThreadQueue() {
    }

public:
    bool empty()  {
        AutoLock m(mutex);
        return (que.size() == 0);
    }

    void push(const Object &obj) {
        AutoLock m(mutex);
        que.push(obj);
    }

    void pop() {
        AutoLock m(mutex);
        que.pop();
    }

    int size()  {
        AutoLock m(mutex);
        return(que.size());
    }

    Object& front() {//为空时调用此方法会产生runtime error
        AutoLock m(mutex);
        Object &obj = que.front();
        return obj;
    }

    const Object& front() const {//为空时调用此方法会产生runtime error
        AutoLock m(mutex);
        Object &obj = que.front();
        return obj;
    }

    Object& back() {//为空时调用此方法会产生runtime error
        AutoLock m(mutex);
        Object &obj = que.back();
        return obj;
    }

    const Object& back() const {//为空时调用此方法会产生runtime error
        AutoLock m(mutex);
        Object &obj = que.back();
        return obj;
    }

    Object getTopAndPop() { //得到队头元素并出队，//为空时调用此方法会产生runtime error
        AutoLock m(mutex);
        if (que.empty()) {
            return Object();
        }
        Object obj=que.front();
        que.pop();
        return std::move(obj);
    }

	void assign(ThreadQueue& obj){//赋值函数，把obj的que赋值给this
		AutoLock m(mutex);
		AutoLock objm(obj.mutex);
		this->que=obj.que;
	}

    void clear() {
        AutoLock m(mutex);
        while (!que.empty()) {
            que.pop();
        }
    }
};

#endif//THREAD_QUEUE_H