#include <iostream>
#include<string>
#include <ctime>
#include <sstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis.h>

/* single redis instance distribute lock of example*/

//lua script: http://www.redis.cn/commands/eval.html

//g++ distribute_lock.cpp  -I/usr/include/hiredis/ -L /usr/lib/x86_64-linux-gnu/ -lhiredis -ggdb  


void Lock(redisReply *reply,redisContext *c , uint32_t& uniqueKey){
    //生成唯一的锁标识符,这里为了简单 就用时间戳代替了
    uniqueKey = time(NULL);

    //set lock:books 1 ex 10 nx
    stringstream iss;
    iss << "set lock:books " << uniqueKey<<" ex 10 nx";
    std::cout<<iss.str()<<std::endl;
    reply = (redisReply*)redisCommand(c,iss.str().c_str());
    if(reply==nullptr){
        printf("set lock: %s\n", c->errstr);
    }else{
        printf("set lock: %s\n", reply->str);
    }
    freeReplyObject(reply);
}


void Unlock(redisReply *reply,redisContext *c , uint32_t uniqueKey){
    /* lua script*/
    std::string lua =  "local del_key = KEYS[1] \
                        local del_arg = ARGV[1] \
                        if redis.call('get',del_key) == del_arg then \
                            return redis.call('del',del_key)    \
                        else    \
                            return 0    \
                        end";

    reply = (redisReply*)redisCommand(c,"eval %s 1 %s %d",lua.c_str(),"lock:books",uniqueKey);
    if(reply==nullptr){
        printf("del lock: %s\n", c->errstr);
    }else{
        printf("del lock: %s\n", reply->str);
    }

    freeReplyObject(reply);

}

int main(int argc, char **argv) {
    unsigned int j, isunix = 0;
    redisContext *c;
    redisReply *reply;
    const char *hostname = (argc > 1) ? argv[1] : "127.0.0.1";

    if (argc > 2) {
        if (*argv[2] == 'u' || *argv[2] == 'U') {
            isunix = 1;
            /* in this case, host is the path to the unix socket */
            printf("Will connect to unix socket @%s\n", hostname);
        }
    }

    int port = (argc > 2) ? atoi(argv[2]) : 6379;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    if (isunix) {
        c = redisConnectUnixWithTimeout(hostname, timeout);
    } else {
        c = redisConnectWithTimeout(hostname, port, timeout);
    }
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }

    /* lock fun in thread */
    uint32_t uniqueKey = 0;
    Lock(reply,c,uniqueKey);

    /* unlock fun in up thread*/
    /* this is test fun*/
    Unlock(reply,c,uniqueKey+1);
    /* this is real fun*/
    Unlock(reply,c,uniqueKey);


    /* Disconnects and frees the context */
    redisFree(c);

    return 0;
}
