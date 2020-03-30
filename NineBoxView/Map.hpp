#ifndef __MAP_HPP_H__
#define __MAP_HPP_H__

#include <vector>
#include <set>
#include <algorithm>
#include "Vector2i.hpp"
#include "MapGrid.hpp"

using namespace std;

class Map{
public:
    Map() = default;

public:
    void InitMap(int mapWidth,int mapHeight,int gridSize){
        m_ColCount = mapWidth / gridSize + 1;
        m_RowCount = mapHeight / gridSize + 1;
        m_GridSize = gridSize;
        m_MapWidth = mapWidth;
        m_MapHeight = mapHeight;

        m_MaxIndex = m_ColCount * m_RowCount;
        m_MapGridVec.resize(m_MaxIndex);
    }

    Vector2i ComputerGrid(int posX,int posY){
        if(posX<0) posX = 0;
        if(posY<0) posY = 0;
        if(posX>m_MapWidth) posX = m_MapWidth;
        if(posY>m_MapHeight) posY = m_MapHeight;
        return {posX/m_GridSize,posY/m_GridSize};
    }

    int ComputerGridIndex(int posX, int posY){
        if(posX<0) posX = 0;
        if(posY<0) posY = 0;
        if(posX>m_MapWidth) posX = m_MapWidth;
        if(posY>m_MapHeight) posY = m_MapHeight;
        return (posY/m_GridSize * m_ColCount + posX/m_GridSize);
    }

    void InterestArea(const Vector2i& from,const Vector2i& dst,set<int>& deleteArea, set<int>& addArea){
        set<int> fromArea = GetAll9Grid(from);
        set<int> dstArea = GetAll9Grid(dst);

        //求set交集
        set<int> intersection;
        std::set_intersection(fromArea.begin(),fromArea.end(),dstArea.begin(),dstArea.end(),std::inserter(intersection,intersection.begin()));//C++11

        //deleteArea =  fromArea - 交集
        std::set_difference(fromArea.begin(),fromArea.end(),intersection.begin(),intersection.end(),std::inserter(deleteArea,deleteArea.begin()));
        //addArea = dstArea - 交集
        std::set_difference(dstArea.begin(),dstArea.end(),intersection.begin(),intersection.end(),std::inserter(addArea,addArea.begin()));

        //DEBUG LOG
        std::cout<<"fromArea:"<<std::endl;
        Print(fromArea);
        std::cout<<"dstArea:"<<std::endl;
        Print(dstArea);
        std::cout<<"intersection:"<<std::endl;
        Print(intersection);
        std::cout<<"deleteArea:"<<std::endl;
        Print(deleteArea);
        std::cout<<"addArea:"<<std::endl;
        Print(addArea);
    }

    void InterestArea(const int& fromPosX, const int& fromPosY,const int& dstPosX, const int& dstPosY,set<int>& deleteArea, set<int>& addArea){
        Vector2i from = ComputerGrid(fromPosX,fromPosY);
        Vector2i dst = ComputerGrid(dstPosX,dstPosY);
        return InterestArea(from,dst,deleteArea,addArea);
    }

private:
    set<int> GetAll9Grid(const Vector2i& pos){
        int curIndex = pos.m_y * m_ColCount + pos.m_x;      //当前索引值
        int curColIndexMax = (pos.m_y+1) * m_ColCount -1;   //当前行最大索引值
        int curColIndexMin = pos.m_y * m_ColCount ;       //当前行最小索引值

        set<int> setIndex;

        setIndex.emplace(curIndex); //中间格子

        int leftIndex = curIndex-1 ; //左边格子索引
        if(leftIndex>=0 && leftIndex >=curColIndexMin){
            setIndex.emplace(leftIndex);
        }
        int rightIndex = (curIndex+1)<=curColIndexMax?(curIndex+1):curColIndexMax;//右边格子索引
        if(rightIndex<=m_MaxIndex-1){
            setIndex.emplace(rightIndex);
        }

        int midUpIndex = curIndex+m_ColCount;//中上格子索引
        if(midUpIndex<=m_MaxIndex-1){
            setIndex.emplace(midUpIndex);
        }

        int midDownIndex = curIndex-m_ColCount; //中下格子索引
        if(midDownIndex>=0){
            setIndex.emplace(midDownIndex);
        }

        int leftUpIndex = midUpIndex-1; //左上格子索引
        if(leftUpIndex>=0  && (leftUpIndex>= curColIndexMin + m_ColCount && leftUpIndex<=m_MaxIndex-1)){
            setIndex.emplace(leftUpIndex);
        }
        int rightUpIndex = (midUpIndex+1)<=(curColIndexMax+m_ColCount)?(midUpIndex+1):(curColIndexMax+m_ColCount);//右上格子索引
        if(rightUpIndex<=m_MaxIndex-1){
            setIndex.emplace(rightUpIndex);
        }


        int leftDownIndex = midDownIndex-1; //左下格子索引
        if(leftDownIndex>=0 && leftDownIndex>=curColIndexMin-m_ColCount){
            setIndex.emplace(leftDownIndex);
        }
        int rightDownIndex = (midDownIndex+1)<=(curColIndexMax-m_ColCount)?(midDownIndex+1):(curColIndexMax-m_ColCount);//右下格子索引
        if(rightDownIndex>=0){
            setIndex.emplace(rightDownIndex);
        }

        return setIndex;
    }

private:
    void Print(const set<int>& set){
        std::cout<<"(";
        for(auto it : set){
            std::cout<<it<<" ";
        }
        std::cout<<")"<<std::endl;
    }

private:
    int m_GridSize {0};
    int m_MapWidth {0};
    int m_MapHeight {0};
    int m_ColCount {0};
    int m_RowCount {0};
    int m_MaxIndex {0};
    std::vector<MapGrid> m_MapGridVec;
};


#endif //__MAP_HPP_H__