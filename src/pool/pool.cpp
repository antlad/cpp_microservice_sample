//
// Created by Vladislav Troinich on 05.06.2020.
//

#include "pool.hpp"
#include "asynctask.hpp"

namespace sample {

void async_pool::work() {
    async_task_ptr current;
    while (m_working.load()) {
        {
            std::unique_lock<std::mutex> guard(m_lock);
            m_cv.wait(guard, [this]() { return !m_tasks.empty(); });
            current = m_tasks.front();
            m_tasks.pop_front();
        }
        current->execute();
        m_io.post([current]() { current->notify_done(); });
    }
}

async_task_ptr async_pool::make_task(exec_fn_result&& f) {
    auto t = std::make_shared<async_task>(m_io, std::forward<exec_fn_result>(f));
    {
        std::lock_guard<std::mutex> lk(m_lock);
        m_tasks.emplace_back(t);
    }
    m_cv.notify_one();
    return t;
}

async_pool::async_pool(io_ctx& io, size_t size) : m_io(io), m_working(true) {
    for (size_t i = 0; i < size; i++) {
        m_workers.emplace_back([this]() { work(); });
    }
}

async_pool::~async_pool() {
    m_working.store(false);
    for (auto& e : m_workers) {
        if (e.joinable())
            e.join();
    }
}

}

