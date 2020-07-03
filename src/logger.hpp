#pragma once

#include "defs.hpp"

#include <spdlog/spdlog.h>

class spdlogger_helper {
public:
    spdlogger_helper(const spdlogger_helper&) = delete;
    spdlogger_helper& operator=(const spdlogger_helper&) = delete;

    explicit spdlogger_helper(const sample::log_ptr& log) : m_log(log) {}

    template <typename Message_Builder> constexpr void trace(Message_Builder&& mb) const noexcept {
        m_log->trace(mb());
    }

    template <typename Message_Builder> constexpr void info(Message_Builder&& mb) const noexcept {
        m_log->info(mb());
        //
    }

    template <typename Message_Builder> constexpr void warn(Message_Builder&& mb) const noexcept {
        m_log->warn(mb());
        //
    }

    template <typename Message_Builder> constexpr void error(Message_Builder&& mb) const noexcept {
        m_log->error(mb());
    }

private:
    sample::log_ptr m_log;
};