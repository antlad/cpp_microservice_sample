//
// Created by Vladislav Troinich on 06.06.2020.
//

#pragma once


#include "../defs.hpp"

#include <boost/asio/deadline_timer.hpp>

#include <any>
#include <atomic>
#include <functional>

namespace sample {

struct async_task: private noncopyable, std::enable_shared_from_this<async_task>
{
    async_task(io_ctx& io, exec_fn_result&& f);

    bool is_done() const;

    template <class T> T wait(ctx c) {
        if (done.load()) {
            return std::any_cast<T>(result);
        }
        timer.expires_from_now(boost::posix_time::pos_infin);
        boost::system::error_code err;
        timer.async_wait(c[err]);
        // TODO: do somthing with error
        return std::any_cast<T>(result);
    }

    void execute();

    void notify_done();

private:
    boost::asio::deadline_timer timer;
    exec_fn_result call;
    std::atomic_bool done;
    std::any result;
};


}


