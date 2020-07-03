//
// Created by Vladislav Troinich on 12.05.2020.
//

#pragma once

#include "common_defs.hpp"
#include "defs.hpp"

#include <string>

namespace sample {
void rest_run(io_ctx& ctx, const std::string& address, int port, const log_ptr& log);
}
