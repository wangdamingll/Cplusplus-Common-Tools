#ifndef __VECTOR2I__
#define __VECTOR2I__

//9宫格左下角坐标
class Vector2i{
public:
    Vector2i() = default;
    Vector2i(int x,int y):m_x(x),m_y(y){};
public:
    bool operator ==(const Vector2i& other){
        return ((m_x == other.m_x) &&(m_y==other.m_y));
    }
//private:示例不做权限控制了
    int m_x{0};
    int m_y{0};
};

#endif