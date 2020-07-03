//
// Created by Vladislav Troinich on 12.05.2020.
//
#include "rest.hpp"
#include "logger.hpp"

#include <restinio/all.hpp>
#include <spdlog/spdlog.h>
#include "pool/pool.hpp"
#include "pool/asynctask.hpp"

#include <nlohmann/json.hpp>

#include <memory>

namespace sample {

using json = nlohmann::json;
using namespace restinio;
namespace rr = restinio::router;
using router_t = rr::express_router_t<>;

class service_handler_t {
public:
    explicit service_handler_t(io_ctx& ctx, const log_ptr& log, const async_pool_ptr& pool) : m_ctx(ctx), m_log(log), m_pool(pool) {}

    service_handler_t(const service_handler_t&) = delete;

    service_handler_t(service_handler_t&&) = delete;

    auto on_delay(const restinio::request_handle_t& req, rr::route_params_t t) {
        int k = i;

        boost::asio::spawn(m_ctx, [this, req, k](ctx c){

          m_log->info("-------------------------");
          auto t = m_pool->make_task([this, k]() {
            auto id = std::this_thread::get_id();
            m_log->info("pool: hello from thread {}, wait started", id);
            std::this_thread::sleep_for(std::chrono::seconds(3));
            m_log->info("pool: hello from thread {}, wait done", id);
            return int(42 + k);
          });
          auto id = std::this_thread::get_id();
          m_log->info("async: put task done, hello async thread {}, wait started", id);
          auto v = t->wait<int>(c);
          m_log->info("async: hello async thread {}, wait done, {} ", id, v);
          m_log->info("finalize http req");

          req->create_response().set_body(fmt::format("Hello world! {}", k)).done();
        });

        i++;

        return restinio::request_accepted();
    }

    auto on_sub_del_by_id(const restinio::request_handle_t& req, rr::route_params_t params) {
        auto id = cast_to<uint64_t>(params["id"]);
        // some delete here
        auto resp = init_resp(req->create_response());

        return resp.done();
    }

    auto on_sub_get_by_id(const restinio::request_handle_t& req, rr::route_params_t params) {
        auto resp = init_resp(req->create_response());
        uint64_t id = 0;
        try {
            id = cast_to<uint64_t>(params["id"]);
        } catch (const std::exception& e) {
            bad_request(resp, e.what());
            return resp.done();
        }

        json jj = {{"id", id}};
        resp.set_body(jj.dump());
        return resp.done();
    }

    auto on_subs_list(const restinio::request_handle_t& req, rr::route_params_t) const {
        auto resp = init_resp(req->create_response());
        auto msg = fmt::format("plain response done {}", 42);
        resp.set_body(msg);
        return resp.done();
    }

private:
    int i = 0;
    io_ctx& m_ctx;
    log_ptr m_log;
    async_pool_ptr m_pool;

    template <typename RESP> void bad_request(RESP& resp, string_view s) {
        json j = {{"bad_request", s}};
        resp.set_body(j.dump());
        resp.header().status_line(restinio::status_bad_request());
    }

    template <typename RESP> static RESP init_resp(RESP resp) {
        resp.append_header("Server", "sample")
            .append_header_date_field()
            .append_header("Content-Type", "text/plain; charset=utf-8");

        return resp;
    }
};

auto make_router(io_ctx& io, const log_ptr& log) {
    using rrouter_t = restinio::router::express_router_t<>;

    auto router = std::make_unique<rrouter_t>();
    auto pool = std::make_shared<async_pool>(io, 10);

    auto handler = std::make_shared<service_handler_t>(io, log, pool);

    auto by = [&](auto method) {
        using namespace std::placeholders;
        return std::bind(method, handler, _1, _2);
    };

    router->http_get(R"(/api/v1/gtw/subs)", by(&service_handler_t::on_subs_list));
    router->http_get(R"(/api/v1/gtw/delay)", by(&service_handler_t::on_delay));
    router->http_delete(R"(/api/v1/gtw/subs/:id)", by(&service_handler_t::on_sub_del_by_id));
    router->http_get(R"(/api/v1/gtw/subs/:id)", by(&service_handler_t::on_sub_get_by_id));

    router->non_matched_request_handler([](auto req) {
        return req->create_response(restinio::status_not_found()).append_header_date_field().connection_close().done();
    });

    return router;
}

void rest_run(io_ctx& ctx, const std::string& address, int port, const log_ptr& log) {
    namespace asio_ns = boost::asio;

    using traits_t = restinio::traits_t<restinio::asio_timer_manager_t, spdlogger_helper, router_t>;
    restinio::run(
        ctx, restinio::on_this_thread<traits_t>().address(address).logger(log).port(port).request_handler(make_router(ctx, log)));
}
} // namespace sample
