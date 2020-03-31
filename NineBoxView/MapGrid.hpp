#ifndef __MAP_GRID__H_
#define __MAP_GRID__H_

#include <unordered_map>

//角色对象
struct ObjBase{
public:
    ObjBase() = default;
    virtual ~ObjBase() = default;

public:
    inline void SetPosX(int posX){
        m_posX = posX;
    }
    inline void SetPosY(int posY){
        m_posY = posY;
    }

    inline int GetPosX() const{
        return m_posX;
    }
    inline int GetPosY() const{
        return m_posY;
    }

    uint64_t GetUid()const{
        return uid;
    }
private:
    int m_posX {0};
    int m_posY {0};
    uint64_t uid {0};
};

//地图格子类 容纳角色和NPC等等
class MapGrid{
public:
    MapGrid() = default;

public:
    void AddObj(const ObjBase& obj){
        m_Obj.emplace(obj.GetUid(),obj); //举个例子
    }

    void RemoveObj(const uint64_t& uid){
        auto it = m_Obj.find(uid);
        if(it == m_Obj.end()){
            std::cout<<"ERROR"<<std::endl;
            return;
        }
        m_Obj.erase(it);
    }

    //.... 其他函数

private:
    std::unordered_map<uint64_t,ObjBase> m_Obj; //简单举个例子 可以用内存池等等
};

#endif //__MAP_GRID__H_