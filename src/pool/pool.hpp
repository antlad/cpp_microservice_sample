//
// Created by Vladislav Troinich on 05.06.2020.
//
#pragma once

#include "../defs.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <vector>
#include <thread>

namespace sample {

class async_pool: private noncopyable {
public:
    virtual ~async_pool();

    async_pool(io_ctx& io, size_t size);

    async_task_ptr make_task(exec_fn_result&& f);

private:
    void work();

    io_ctx& m_io;
    std::mutex m_lock;
    std::condition_variable m_cv;
    std::atomic_bool m_working;
    std::vector<std::thread> m_workers;
    std::list<async_task_ptr> m_tasks;
};

typedef std::shared_ptr<async_pool> async_pool_ptr;

}

