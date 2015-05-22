/***************************************
* @file     IFactory.h
* @brief    抽象工厂类, 一个模板。不需要单例模式。可拷贝添加用于不同工程
* @details  具体的工厂从它这里派生
			由于C/C++没有反射机制，所以对抽象工厂支持不是很好。
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-9
****************************************/
#ifndef I_FACTORY_H
#define I_FACTORY_H
class IProduct;//产品类

class IFactory{
public:
	virtual IProduct* CreateProduct()=0;
private:
	IFactory(){};
	~IFactory(){};
	IFactory(const IFactory&);
	IFactory& operator=(const IFactory&);
};

#endif//I_FACTORY_H