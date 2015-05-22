/***************************************
* @file     guardclass.h
* @brief    守护类--负责初始化与释放2种函数
* @details  调用方法
			定义类型: typedef int(*INITFUN)();
					  typedef CGuardClass<INITFUN,INITFUN> Init_Clean_Fun;
			构造函数中调用 static Init_Clean_Fun  ICFun(InitSDK,ClearUp);
			一调用就会触发InitSDK函数，静态变量在程序退出时才退出，所以程序退出时才触发ClearUp函数。

			一种比较奇异的设计方式，我会使用比较正规的，程序开始时初始化，结尾时释放。较少使用这种技术。

* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-9
****************************************/
#ifndef GUARD_CLASS_H
#define GUARD_CLASS_H
#include "mutex.h"
template <class FUN_INIT,class FUN_CLEAN>
class CGuardClass
{
public:
    CGuardClass(FUN_INIT initFun,FUN_CLEAN cleanFun):clen_fun_(cleanFun) {
        AutoLock lock(mut_);
        initFun();
    }
    ~CGuardClass() {
        clen_fun_();
    }
private:
	CGuardClass(const CGuardClass&);//不实现
	CGuardClass& operator =(const CGuardClass&);//不实现
    FUN_CLEAN clen_fun_;
    Mutex mut_;
};
#endif//GUARD_CLASS_H
