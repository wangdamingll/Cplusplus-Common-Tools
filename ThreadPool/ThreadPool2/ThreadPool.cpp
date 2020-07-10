#include "ThreadPool.h"

thread_local work_stealing_queue* ThreadPool::local_work_queue = nullptr;
thread_local unsigned ThreadPool::my_index = 0;