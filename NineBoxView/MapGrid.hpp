#ifndef __MAP_GRID__H_
#define __MAP_GRID__H_

#include <unordered_map>

//地图格子类 容纳角色和NPC等等
struct ObjBase{
public:
    ObjBase() = default;
    virtual ~ObjBase() = default;

public:
    uint64_t GetUid()const{
        return uid;
    }
private:
    uint64_t uid {0};
};

class MapGrid{
public:
    MapGrid() = default;

public:
    void AddObj(const ObjBase& obj){
        m_Obj.emplace(obj.GetUid(),obj); //举个例子
    }

    //.... 其他函数

private:
    std::unordered_map<uint64_t,ObjBase> m_Obj; //简单举个例子 可以用内存池等等
};

#endif //__MAP_GRID__H_