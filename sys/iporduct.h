/***************************************
* @file     IPorduct.h
* @brief    抽象工厂模式中的抽象产品类，一个模板。不需要单例模式。可拷贝添加用于不同工程
* @details  
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-9
****************************************/
#ifndef I_PRODUCT_H
#define I_PRODUCT_H
class IProduct{
public:
	IProduct(){};
	~IProduct(){};
	//其他方法抽象成员函数
private:
	IProduct(const IProduct&);
	IProduct& operator =(const IProduct&);
};
#endif//I_PRODUCT_H