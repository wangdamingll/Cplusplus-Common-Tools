#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#if __WIN32
#include <windows.h>
#endif
#include "Map.hpp"
using namespace std;

constexpr int MAP_WIDTH = 256;
constexpr int MAP_HEIGHT = 256;
constexpr int MAP_GRID_SIZE = 10;

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<> dis(0, MAP_WIDTH);

float GetPos(){
    return dis(gen);
}

int main() {
    Map map;
    map.InitMap(MAP_WIDTH,MAP_HEIGHT,MAP_GRID_SIZE);
    while (1){
        //模拟玩家坐标
        float fromPosX = GetPos();
        float fromPosY = GetPos();
        std::cout<<"fromPost:("<<fromPosX<<","<<fromPosY<<")"<<std::endl;

        float dstPosX = GetPos();
        float dstPosY = GetPos();
        std::cout<<"dstPost:("<<dstPosX<<","<<dstPosY<<")"<<std::endl;

        Vector2i from = map.ComputerGrid(fromPosX,fromPosY);
        Vector2i dst = map.ComputerGrid(dstPosX, dstPosY);

        int fromIndex = map.ComputerGridIndex(fromPosX,fromPosY);
        int dstIndex = map.ComputerGridIndex(dstPosX,dstPosY);
        std::cout<<"fromGridIndex:"<<fromIndex<<" dstGridIndex:"<<dstIndex<<std::endl;

        if(from == dst){
            std::cout<<"same grid"<<std::endl;
            return 0;
        }

        //different grid
        set<int> deleteArea,addArea;
        map.InterestArea(from,dst,deleteArea,addArea);

        //具体更新玩家视野 简单举个例子
        ObjBase * obj = new ObjBase();
        map.AddObj(dst,*obj);

#if __WIN32
        Sleep(1000 * 5);
#endif
        //std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
