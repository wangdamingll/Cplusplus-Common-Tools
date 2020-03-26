#include <iostream>
using namespace std;

#include "MessageBus.hpp"

MessageBus g_bus;
const string Topic = "Drive";
const string CallBackTopic = "DriveOk";

//TEST CODE
struct Subject{
    Subject(){
        g_bus.Attach([this]{DriveOk();},CallBackTopic);
    }
    void SendReq(const string& topic){
        g_bus.SendReq<void, int>(50, topic);
    }
    void DriveOk(){
        cout<<"drive ok"<<endl;
    }
};
struct Car{
    Car(){
        g_bus.Attach([this](int speed){Drive(speed);}, Topic);
    }
    void Drive(int speed){
        cout << "Car drive " << speed << endl;
        g_bus.SendReq<void>(CallBackTopic);
    }
};
struct Bus{
    Bus(){
        g_bus.Attach([this](int speed){Drive(speed);});
    }
    void Drive(int speed){
        cout << "Bus drive " << speed << endl;
        g_bus.SendReq<void>(CallBackTopic);
    }
};
struct Truck{
    Truck(){
        g_bus.Attach([this](int speed){Drive(speed);});
    }
    void Drive(int speed){
        cout << "Truck drive " << speed << endl;
        g_bus.SendReq<void>(CallBackTopic);
    }
};

void TestBus(){
    Subject subject;
    Car car;
    Bus bus;
    Truck truck;
    subject.SendReq(Topic);
    subject.SendReq("");
}

//TEST CODE
void TestMsgBus(){
    MessageBus bus;
    // 注册消息
    bus.Attach([](int a){cout << "no reference" << a << endl; });
    bus.Attach([](int& a){cout << "lvalue reference" << a << endl; });
    bus.Attach([](int&& a){cout << "rvalue reference" << a << endl; });
    bus.Attach([](const int& a){cout << "const lvalue reference" << a << endl; });
    bus.Attach([](int a){cout << "no reference has return value and key" << a << endl;return a;}, "a");
    int i = 2;

    // 发送消息
    bus.SendReq<void, int>(2);
    bus.SendReq<int, int>(2, "a");
    bus.SendReq<void, int&>( i);
    bus.SendReq<void, const int&>(2);
    bus.SendReq<void, int&&>(2);

    // 移除消息
    bus.Remove<void, int>();
    bus.Remove<int, int>("a");
    bus.Remove<void, int&>();
    bus.Remove<void, const int&>();
    bus.Remove<void, int&&>();

    // 发送消息
    bus.SendReq<void, int>(2);
    bus.SendReq<int, int>(2, "a");
    bus.SendReq<void, int&>(i);
    bus.SendReq<void, const int&>(2);
    bus.SendReq<void, int&&>(2);
}

int main() {
    TestBus();
    std::cout<<"----------------------------"<<std::endl;
    TestMsgBus();
    return 0;
}
