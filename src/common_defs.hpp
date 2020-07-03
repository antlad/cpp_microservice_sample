#pragma once

#include <boost/asio/spawn.hpp>
#include <boost/beast/core/span.hpp>
#include <boost/core/noncopyable.hpp>

#include <any>
#include <functional>
#include <memory>
#include <stdexcept>

#if __cplusplus >= 201703L
#include <optional>
#include <string_view>
#else
#include <boost/optional.hpp>
#include <boost/utility/string_view.hpp>
#endif

namespace spdlog {
class logger;
}

namespace sample {

#if __cplusplus >= 201703L
using std::optional;
using std::string_view;
#else
using boost::optional;
using boost::string_view;
#endif

using boost::beast::span;

typedef std::shared_ptr<spdlog::logger> log_ptr;

typedef boost::asio::io_context io_ctx;

typedef boost::asio::yield_context ctx;

class async_task;

using boost::noncopyable;

typedef std::shared_ptr<async_task> async_task_ptr;

typedef std::function<std::any()> exec_fn_result;

} // namespace sample
