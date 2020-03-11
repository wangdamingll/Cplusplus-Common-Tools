#include <iostream>
#include <functional>
#include "Aspect.hpp"

using namespace std;


struct AA{
    void Before(int i){
        cout <<"Before from AA"<<i<< endl;
    }
    void After(int i){
        cout <<"After from AA"<<i<< endl;
    }
};
struct BB{
    void Before(int i){
        cout <<"Before from BB"<<i<< endl;
    }
    void After(int i){
        cout <<"After from BB"<<i<< endl;
    }
};
struct CC{
    void Before(){
        cout <<"Before from CC"<< endl;
    }
    void After(){
        cout <<"After from CC"<< endl;
    }
};
struct DD{
    void Before(){
        cout <<"Before from DD"<< endl;
    }
    void After(){
        cout <<"After from DD"<< endl;
    }
};
void GT(){
    cout <<"real GT function"<< endl;
}
void HT(int a){
    cout <<"real HT function: "<<a<< endl;
}
void TestAop1(){
    // 织入普通函数
    std::function<void(int)> f = std::bind(&HT, std::placeholders::_1);
    Invoke<AA, BB>(std::function<void(int)>(std::bind(&HT, std::placeholders::_1)), 1);
    // 组合了两个切面AA BB
    Invoke<AA, BB>(f, 1);

    // 织入普通函数
    Invoke<CC, DD>(&GT);
    Invoke<AA, BB>(&HT, 1);

    // 织入lambda表达式
    Invoke<AA, BB>([](int i){}, 1);
    Invoke<CC, DD>([]{});
}



/*------------------------------------*/
struct TimeElapsedAspect{
	void Before(int i){
        cout <<"time start"<< endl;
	}
	void After(int i){
		cout <<"time end"<< endl;
	}
};
struct LoggingAspect{
	void Before(int i){
		std::cout <<"entering"<< std::endl;
	}
	void After(int i){
		std::cout <<"leaving"<< std::endl;
	}
};
void foo(int a){
	cout <<"real HT function: "<<a<< endl;
}

void TestAop2() {
    Invoke<LoggingAspect, TimeElapsedAspect>(&foo, 1); //织入方法
    cout <<"-----------------------"<< endl;
    Invoke<TimeElapsedAspect, LoggingAspect>(&foo, 1);
}

int main() {
    TestAop1();
    TestAop2();
    return 0;
}
