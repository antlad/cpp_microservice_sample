#include "src/rest.hpp"

#include <cxxopts.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/spawn.hpp>

#include <filesystem>
#include <memory>
#include <signal.h>
#include <string>
#include <thread>


std::unique_ptr<boost::asio::io_context> ioc;

void signal_handler(int signal) {
    if ((signal == SIGINT || signal == SIGTERM)) {
        //        if (core != nullptr)
        //            core->stop();
        if (ioc != nullptr)
            ioc->stop();
    }
}

int main(int argc, char* argv[]) {
    try {
        setenv("TZ", "", 0);
        tzset();
        auto console = spdlog::stdout_color_mt("console");
        console->set_level(spdlog::level::info);

        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        std::string rest_address = "0.0.0.0";
        int rest_port = 8083;
        cxxopts::Options options(argv[0], " - filters command line options");
        /* clang-format off */
        options.add_options()
                ("h,help", "Print help")
                ("d,debug","Enable debugging")
                ("rest_address", "rest address", cxxopts::value<std::string>(rest_address))
                ("rest_port", "rest port", cxxopts::value<int>(rest_port))
                ;
        /* clang-format on */
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (result.count("debug")) {
            console->set_level(spdlog::level::trace);
        }

        ioc = std::make_unique<boost::asio::io_context>();
        std::thread asio_thread([&]() {
            sample::rest_run(*ioc, rest_address, rest_port, console);
            console->info("asio thread exit");
        });

        asio_thread.join();
        console->info("global exit");

        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown exception" << std::endl;
        return 1;
    }
}
