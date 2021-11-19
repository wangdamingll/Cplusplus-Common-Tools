//=====================================================================
//
// itimer.h - Application Level Implementation of Linux Kernel Timer 
//
// NOTE:
// for more information, please see the readme file.
//
//=====================================================================
#ifndef __ITIMERMGR_H__
#define __ITIMERMGR_H__

#include <stddef.h>
#include <functional>

//=====================================================================
// 32BIT INTEGER DEFINITION 
//=====================================================================
#ifndef __INTEGER_32_BITS__
#define __INTEGER_32_BITS__
#if defined(__UINT32_TYPE__) && defined(__UINT32_TYPE__) && defined(__UINT64_TYPE__) && defined(__UINT64_TYPE__)
	typedef __UINT32_TYPE__ ISTDUINT32;
	typedef __INT32_TYPE__ ISTDINT32;
    typedef __UINT64_TYPE__ ISTDUINT64;
    typedef __INT64_TYPE__ ISTDINT64;
#elif defined(__UINT_FAST32_TYPE__) && defined(__INT_FAST32_TYPE__)
	typedef __UINT_FAST32_TYPE__ ISTDUINT32;
	typedef __INT_FAST32_TYPE__ ISTDINT32;
#elif defined(_WIN64) || defined(WIN64) || defined(__amd64__) || \
	defined(__x86_64) || defined(__x86_64__) || defined(_M_IA64) || \
	defined(_M_AMD64)
	typedef unsigned int ISTDUINT32;
	typedef int ISTDINT32;
#elif defined(_WIN32) || defined(WIN32) || defined(__i386__) || \
	defined(__i386) || defined(_M_X86)
	typedef unsigned long ISTDUINT32;
	typedef long ISTDINT32;
#elif defined(__MACOS__)
	typedef UInt32 ISTDUINT32;
	typedef SInt32 ISTDINT32;
#elif defined(__APPLE__) && defined(__MACH__)
	#include <sys/types.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif defined(__BEOS__)
	#include <sys/inttypes.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && (!defined(__MSDOS__))
	typedef unsigned __int32 ISTDUINT32;
	typedef __int32 ISTDINT32;
#elif defined(__GNUC__) && (__GNUC__ > 3)
	#include <stdint.h>
	typedef uint32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
	typedef uint64_t ISTDUINT64;
	typedef int64_t ISTDINT64;
#else 
	typedef unsigned long ISTDUINT32; 
	typedef long ISTDINT32;
#endif
#endif


//=====================================================================
// Integer Definition
//=====================================================================
#ifndef __IINT8_DEFINED
#define __IINT8_DEFINED
typedef char IINT8;
#endif

#ifndef __IUINT8_DEFINED
#define __IUINT8_DEFINED
typedef unsigned char IUINT8;
#endif

#ifndef __IUINT16_DEFINED
#define __IUINT16_DEFINED
typedef unsigned short IUINT16;
#endif

#ifndef __IINT16_DEFINED
#define __IINT16_DEFINED
typedef short IINT16;
#endif

#ifndef __IINT32_DEFINED
#define __IINT32_DEFINED
typedef ISTDINT32 IINT32;
#endif

#ifndef __IUINT32_DEFINED
#define __IUINT32_DEFINED
typedef ISTDUINT32 IUINT32;
#endif

#ifndef __IUINT64_DEFINED
#define __IUINT64_DEFINED
typedef ISTDUINT64 IUINT64;
#endif


/*====================================================================*/
/* LIST DEFINITION                                                   */
/*====================================================================*/
#ifndef __ILIST_DEF__
#define __ILIST_DEF__

struct ILISTHEAD {
	struct ILISTHEAD *next, *prev;
};

typedef struct ILISTHEAD ilist_head;


/*--------------------------------------------------------------------*/
/* list init                                                         */
/*--------------------------------------------------------------------*/
#define ILIST_HEAD_INIT(name) { &(name), &(name) }
#define ILIST_HEAD(name) \
	struct ILISTHEAD name = ILIST_HEAD_INIT(name)

#define ILIST_INIT(ptr) ( \
	(ptr)->next = (ptr), (ptr)->prev = (ptr))

#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define ICONTAINEROF(ptr, type, member) ( \
		(type*)( ((char*)((type*)ptr)) - IOFFSETOF(type, member)) )

#define ILIST_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)


/*--------------------------------------------------------------------*/
/* list operation                                                    */
/*--------------------------------------------------------------------*/
#define ILIST_ADD(node, head) ( \
	(node)->prev = (head), (node)->next = (head)->next, \
	(head)->next->prev = (node), (head)->next = (node))

#define ILIST_ADD_TAIL(node, head) ( \
	(node)->prev = (head)->prev, (node)->next = (head), \
	(head)->prev->next = (node), (head)->prev = (node))

#define ILIST_DEL_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n))

#define ILIST_DEL(entry) (\
	(entry)->next->prev = (entry)->prev, \
	(entry)->prev->next = (entry)->next, \
	(entry)->next = 0, (entry)->prev = 0)

#define ILIST_DEL_INIT(entry) do { \
	ILIST_DEL(entry); ILIST_INIT(entry); } while (0)

#define ILIST_IS_EMPTY(entry) ((entry) == (entry)->next)

#define ilist_init		ILIST_INIT
#define ilist_entry		ILIST_ENTRY
#define ilist_add		ILIST_ADD
#define ilist_add_tail	ILIST_ADD_TAIL
#define ilist_del		ILIST_DEL
#define ilist_del_init	ILIST_DEL_INIT
#define ilist_is_empty	ILIST_IS_EMPTY

#define ILIST_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = ilist_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = ilist_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define ilist_foreach(iterator, head, TYPE, MEMBER) \
	ILIST_FOREACH(iterator, head, TYPE, MEMBER)

#define ilist_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )
	

#define __ilist_splice(list, head) do {	\
		ilist_head *first = (list)->next, *last = (list)->prev; \
		ilist_head *at = (head)->next; \
		(first)->prev = (head), (head)->next = (first);		\
		(last)->next = (at), (at)->prev = (last); }	while (0)

#define ilist_splice(list, head) do { \
	if (!ilist_is_empty(list)) __ilist_splice(list, head); } while (0)

#define ilist_splice_init(list, head) do {	\
	ilist_splice(list, head);	ilist_init(list); } while (0)


#ifdef _MSC_VER
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)
#endif

#endif


//=====================================================================
// Timer Vector
//=====================================================================
#define ITVN_BITS		6
#define ITVR_BITS		8
#define ITVN_SIZE		(1 << ITVN_BITS)
#define ITVR_SIZE		(1 << ITVR_BITS)
#define ITVN_MASK		(ITVN_SIZE - 1)
#define ITVR_MASK		(ITVR_SIZE - 1)

struct itimer_vec {
	ilist_head vec[ITVN_SIZE];
};

struct itimer_vec_root {
	ilist_head vec[ITVR_SIZE];
};

struct itimer_core {
	IUINT32 timer_jiffies;
	struct itimer_vec *tvecs[6];
	struct itimer_vec_root tv1;
	struct itimer_vec tv2;
	struct itimer_vec tv3;
	struct itimer_vec tv4;
	struct itimer_vec tv5;
};

struct itimer_node {
	ilist_head head;
	IUINT32 expires;
	IUINT32 state;
	void *data;
    void (*callback)(void *data);
	struct itimer_core *core;
};


//=====================================================================
// global definition
//=====================================================================
typedef struct itimer_core itimer_core;
typedef struct itimer_node itimer_node;

#define itimer_core_jiffies(core) ((core)->jiffies)
#define itimer_node_pending(node) (!iqueue_is_empty(&(node)->head))


#ifdef __cplusplus
extern "C" {
#endif

//=====================================================================
// Core Timer
//=====================================================================

// initialize timer core
void itimer_core_init(itimer_core *core, IUINT32 jiffies);

// destroy timer core
void itimer_core_destroy(itimer_core *core);

// run timer core 
void itimer_core_run(itimer_core *core, IUINT32 jiffies);


// initialize node
void itimer_node_init(itimer_node *node, void (*fn)(void*), void *data);

// destroy node
void itimer_node_destroy(itimer_node *node);

// add node to core
void itimer_node_add(itimer_core *core, itimer_node *node, IUINT32 expires);

// remove node from core
int itimer_node_del(itimer_core *core, itimer_node *node);

// modify node
int itimer_node_mod(itimer_core *core, itimer_node *node, IUINT32 expires);



//=====================================================================
// Timer Manager
//=====================================================================
struct itimer_mgr
{
	IUINT32 interval;
	IUINT32 current;
	IUINT32 millisec;
	IUINT32 jiffies;
	itimer_core core;
};

struct itimer_evt
{
    IUINT64 evtId;                      //自己添加的 用于C++的简单封装
    std::function<void(void*,void*)> fn; //自己添加的 用于C++的简单封装
	IUINT32 period;
	IUINT32 slap;
	int repeat;
	int running;
	void *data;
	void *user;
	struct itimer_mgr *mgr;
	itimer_node node;
};

// type defines
typedef struct itimer_mgr itimer_mgr;
typedef struct itimer_evt itimer_evt;

// initialize timer manager
// millisec - current time stamp
// interval - internal working interval
void itimer_mgr_init(itimer_mgr *mgr, IUINT32 millisec, IUINT32 interval);

// destroy timer manager
void itimer_mgr_destroy(itimer_mgr *mgr);

// run timer events: 
// millisec - current time stamp
void itimer_mgr_run(itimer_mgr *mgr, IUINT32 millisec);


// initialize timer event
void itimer_evt_init(itimer_evt *evt, std::function<void(void*,void*)>&& fn,
	void *data, void *user);

// destroy timer event
void itimer_evt_destroy(itimer_evt *evt);

// start timer: repeat <= 0 (infinite repeat)
void itimer_evt_start(itimer_mgr *mgr, itimer_evt *evt, 
	IUINT32 period, int repeat, IUINT32 firstInterval);

// stop timer
void itimer_evt_stop(itimer_mgr *mgr, itimer_evt *evt);



#ifdef __cplusplus
}
#endif

//下面采用C++简单封装一下便于使用
#include <unordered_map>
#include <functional>
using namespace std;

class TimerMgr
{
public:
    using EvtCallBack = std::function<void(void*,void*)>;
public:
    /**
     * @param curMillSec:ms
     * @param interval:ms
     */
    TimerMgr(uint64_t curMillSec, uint32_t interval);
    ~TimerMgr();

    /**
     *
     * @param callBack
     * @param data
     * @param user
     * @return evt_id
     */
    uint64_t TimerEvtInit(EvtCallBack&& callBack, void* data, void* user);

    /**
     * @param evtId
     * @param period
     * @param repeat
     * @param firstInterval
     */
    void TimerEvtStart(uint64_t evtId, uint32_t period, uint32_t repeat, uint32_t firstInterval = 0);

    /**
     * @param evtId
     */
    void TimerEvtStop(uint64_t evtId);

    /**
     * @param evtId
     */
    void TimerEvtDestroy(uint64_t evtId);

    void TimerMgrDestroy();

    /**
     * @param curMillSec:ms
     */
    void Run(uint64_t curMillSec);

private:
    /**
     * @param evt
     */
    void AddEvt(itimer_evt* evt);

    /**
     * @param evtId
     * @return
     */
    itimer_evt* FindEvt(uint64_t evtId);

    /**
     * @param evtId
     */
    void RemoveEvt(uint64_t evtId);
private:
    uint64_t mIndex = 0;
    itimer_mgr* mMgr = nullptr;
    std::unordered_map<uint32_t/*evtid*/,itimer_evt*> mEvt;
};

#define INIT_TIMERMGR(timerMgr, now, interval)  \
TimerMgr timerMgr(now, interval);

#define REG_TIMER(timerMgr, func, data, user ,period, repeat, timerId)   \
{   \
    timerId = timerMgr.TimerEvtInit(std::move(func), (void*)data, (void*)user);    \
    timerMgr.TimerEvtStart(timerId, period, repeat); \
}

#define REG_TIMER_FIRST(timerMgr, func, data, user ,period, repeat, firstInterval, timerId)   \
{   \
    timerId = timerMgr.TimerEvtInit(std::move(func), (void*)data, (void*)user);    \
    timerMgr.TimerEvtStart(timerId, period, repeat, firstInterval); \
}

#define REG_CLASS_TIMER(timerMgr, func, ins, user, period, repeat, timerId)   \
{   \
    auto callBack = std::bind(&func, ins, std::placeholders::_1, std::placeholders::_2);   \
    timerId = timerMgr.TimerEvtInit(std::move(callBack), (void*)ins, (void*)user);    \
    timerMgr.TimerEvtStart(timerId, period, repeat); \
}

#define REG_CLASS_TIMER_FIRST(timerMgr, func, ins, user, period, repeat, firstInterval, timerId)   \
{   \
    auto callBack = std::bind(&func, ins, std::placeholders::_1, std::placeholders::_2);   \
    timerId = timerMgr.TimerEvtInit(std::move(callBack), (void*)ins, (void*)user);    \
    timerMgr.TimerEvtStart(timerId, period, repeat, firstInterval); \
}

#define START_TIMER(timerMgr, timerId, period, repeat)  \
{   \
    timerMgr.TimerEvtStart(timerId, period, repeat);  \
}

#define STOP_TIMER(timerMgr, timerId)  \
{   \
    timerMgr.TimerEvtStop(timerId);  \
}

#define UNREG_TIMER(timerMgr, timerId)  \
{   \
    timerMgr.TimerEvtDestroy(timerId);  \
}

#define RUN_TIMERMGR(timerMgr, now)  \
{   \
    timerMgr.Run(now);  \
}

#define UNINIT_TIMERMGR(timerMgr)  \
{   \
    timerMgr.TimerMgrDestroy(); \
}

#endif


