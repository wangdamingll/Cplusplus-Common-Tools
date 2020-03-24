#ifndef __CPP17
    #if __cplusplus > 201402L
    #       define __CPP17 (1)
    #else
    #       define __CPP17 (0)
    #endif
#endif

#ifndef __CPP14
    #if __cplusplus > 201103L
    #       define __CPP14 (1)
    #else
    #       define __CPP14 (0)
    #endif
#endif

#ifndef __CPP11
    #if __cplusplus > 199711L
    #       define __CPP11 (1)
    #else
    #       define __CPP11 (0)
    #endif
#endif

#if __CPP17
#include "Ioc_17.hpp"
#elif __CPP11
#include "Ioc_11.hpp"
#endif

#include <iostream>
using namespace std;

/*test code*/
//测试继承对象
struct Base
{
	virtual void Func(){}
	virtual ~Base(){}
};

struct DerivedB : public Base
{
	DerivedB(int a, double b):m_a(a),m_b(b)
	{
	}
	void Func()override
	{
		cout<<"DerivedB::Fun()::"<<m_a+m_b<<endl;
	}
private:
	int m_a;
	double m_b;
};

struct DerivedC : public Base
{
    void Func()override
    {
        cout<<"DerivedC::Fun()"<<endl;
    }
};

struct A
{
	A(Base * ptr) :m_ptr(ptr)
	{
	}

	void Func()
	{
		m_ptr->Func();
	}

	~A()
	{
		if(m_ptr!=nullptr)
		{
			delete m_ptr;
			m_ptr = nullptr;
		}
	}

private:
Base * m_ptr;
};

//测试普通对象
struct Bus
{
    void Func() const { std::cout <<"Bus::Func()"<<std::endl; }
};
struct Car
{
    void Func() const { std::cout <<"Car::Func()"<<std::endl; }
};


void TestIoc(){
	IocContainer ioc;
	//注册继承对象
	ioc.RegisterType<A, DerivedC>("C");      //配置依赖关系
    ioc.RegisterType<A, DerivedB, int, double>("B");   //注册时要注意DerivedB的参数int和double

    //注册普通对象
    ioc.RegisterSimple<Bus>("Bus");
    ioc.RegisterSimple<Car>("Car");

	auto c = ioc.ResolveShared<A>("C");
    c->Func();
	auto b = ioc.ResolveShared<A>("B", 1, 2.0); //还要传入参数
	b->Func();

    auto bus = ioc.ResolveShared<Bus>("Bus");
    bus->Func();
    auto car = ioc.ResolveShared<Car>("Car");
    car->Func();

}

int main() {
    TestIoc();

    return 0;
}
