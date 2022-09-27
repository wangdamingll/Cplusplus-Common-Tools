#include <iostream>
#include <vector>
#include <memory>
#include "OrderedMap.h"

struct Role
{
    uint64_t roleId = 0;
    uint32_t time = 0;
};

struct RoleKey : public std::unary_function<Role*, uint64_t>
{
    uint64_t operator()(Role* value) const
    {
        return value->roleId;
    }
};

struct Compare : public std::binary_function<Role*, Role*, bool>
{
    bool operator()(Role* lval , Role* rval) const
    {
        if(lval->time != rval->time)
        {
            return lval->time < rval->time;
        }
        return lval->roleId < rval->roleId;
    }
};

int main()
{
    OrderedHashMap<Role*, RoleKey, Compare> pRoles;

    for(uint32_t index = 1; index <= 5; index++)
    {
        Role* pRole = new Role();
        pRole->roleId = index;
        pRole->time = index;
        pRoles.Insert(pRole);
    }

    pRoles.Erase(1);

    std::vector<std::unique_ptr<Role>> removeRoles;
    pRoles.ForeachRemove([&](Role* pRole){
        if(pRole->time >= 4)
        {
            return false;
        }
        removeRoles.push_back(std::unique_ptr<Role>(pRole));
        return true;
    });

    //4 5
    pRoles.Foreach([&](Role* pRole){
        std::cout<<" roleId:"<<pRole->roleId<<" time:"<<pRole->time<<std::endl;
    });

    std::cout<<"---------------------------"<<std::endl;

    Role* pRole = new Role();
    pRole->roleId = 2;
    pRole->time = 2;
    pRoles.Insert(pRole);

    //2 4 5
    pRoles.Foreach([&](Role* pRole){
       std::cout<<" roleId:"<<pRole->roleId<<" time:"<<pRole->time<<std::endl;
    });

    return 0;
}
