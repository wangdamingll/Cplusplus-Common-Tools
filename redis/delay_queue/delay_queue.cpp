#include <iostream>
#include<string>
#include <ctime>
#include <sstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis.h>

/* delay queue*/

//lua script: http://www.redis.cn/commands/eval.html

//g++ delay_queue.cpp  -I/usr/include/hiredis/ -L /usr/lib/x86_64-linux-gnu/ -lhiredis -ggdb  

void Delay(redisReply *reply,redisContext *c ,const string& msg){
    //序列化工具序列化成字符串
    //这里省略了
    int32_t delayTime = time(NULL) + 5;
    /* add sort set */
    stringstream iss;
    iss << "zadd delay-queue " << delayTime<<" " << msg<<"-"<<delayTime;
    std::cout<<iss.str()<<std::endl;
    reply = (redisReply*)redisCommand(c,iss.str().c_str());
    if(reply==nullptr){
        printf("zadd: %s\n", c->errstr);
    }
    freeReplyObject(reply);
}

void GetMsg(redisReply *reply,redisContext *c){
    /* lua script*/
    std::string lua =   "local zset_key = KEYS[1]   \
                        local min_score = 0   \
                        local max_score = ARGV[1]  \
                        local offset = 0  \
                        local limit = 1   \
                        \
                        local status, type = next(redis.call('TYPE', zset_key)) \
                        if status ~= nil and status == 'ok' then    \
                            if type == 'zset' then                  \
                                local list = redis.call('ZRANGEBYSCORE', zset_key, min_score, max_score, 'LIMIT', offset, limit)    \
                                if list ~= nil and #list > 0 then   \
                                    local value = list[1]   \
                                    redis.call('ZREM', zset_key, value) \
                                    return value    \
                                end \
                            end \
                        end \
                        return nil";

    reply = (redisReply*)redisCommand(c,"eval %s 1 %s %d",lua.c_str(),"delay-queue",time(NULL));
    if(reply==nullptr){
        printf("zget: %s\n", c->errstr);
    }else{
        /* get msg */
        printf("zget delay-queue: %s\n", reply->str);
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


    /* delay queue */
    //one thread push msg
    Delay(reply,c,"test");

    //another thread get msg
    GetMsg(reply,c);

    /* Disconnects and frees the context */
    redisFree(c);

    return 0;
}
