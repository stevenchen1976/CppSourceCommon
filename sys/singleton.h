/***************************************
* @file     singleton.h
* @brief    单例模式
* @details
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-2
* @modify   2013-9-17    phata    去掉无用的头文件
****************************************/
#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class Singleton
{
//private://private不能作为父类
protected://protected可作为父类
    Singleton() {
    };
    ~Singleton() {
    };
private://private中，禁止复制和赋值
    Singleton(const Singleton&);//不实现
    Singleton& operator =(const Singleton&);//不实现
public:
    static T* GetInstance() {
        //此需要加锁以保证线程安全，但C++11标准规则static 变量是线程安全的，所以对于支持C++11标准的编译器，不需要加锁。
        static T instance;
        return &instance;
    }
};

#endif//SINGLETON_H
/**********************************
mexo提出的方案的确不错，但是好象也并不完美（我不是指多线程解决方案），因为他把模板类的构造函数放在私有段里了，如果放在protected段里就好得多，因为你的类可以从模板类继承，这样就不再需要你的那个 typedef Singleton<ServiceManger> SSManger;定义了。示例如下：
	template <class T>
class Singleton {
public:
	static T& instance() {
		static T _instance;
		return _instance;
	}
protected:
	Singleton(void) {}
	virtual ~Singleton(void) {}
	Singleton(const Singleton<T>&); //不实现
	Singleton<T>& operator= (const Singleton<T> &); //不实现
};
--------------------------------
下面是一个需要做为单例的类，只需从Singleton继承即可
class Test : public Singleton<Test> {
public:
	void foo(){};
private:
	Test(){};
	~Test(){};
	friend class Singleton<Test>;
};
----------------------
	这样，别人在使用的时候，只需要写
	Test::GetInstance()->foo();
而再也不需要写：
	Singleton<Test>::GetInstance()->foo();
*****************************************************/