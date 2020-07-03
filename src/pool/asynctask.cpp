//
// Created by Vladislav Troinich on 06.06.2020.
//

#include "asynctask.hpp"

namespace sample {

bool async_task::is_done() const { return done.load(); }

async_task::async_task(io_ctx& io, exec_fn_result&& f)
    : timer(io), call(std::forward<exec_fn_result>(f)), done(false) {}

void async_task::execute() {
    result = call();
    done.store(true);
}

void async_task::notify_done() {
    timer.cancel();
}

}

