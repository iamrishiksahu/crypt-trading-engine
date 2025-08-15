#include "../common/config_parser/parser.h"
#include <boost/asio/ssl/context.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <signal.h>
#include <string>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
namespace ws_lib = websocketpp::lib;
namespace ssl    = boost::asio::ssl;

using json = nlohmann::json;
using websocketpp::connection_hdl;

// TLS init handler
static ws_lib::shared_ptr<ssl::context> on_tls_init()
{
    auto ctx = ws_lib::make_shared<ssl::context>(ssl::context::tlsv12);
    ctx->set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 | ssl::context::no_sslv3 | ssl::context::single_dh_use);
    return ctx;
}

void              signalHandler(int);
std::atomic<bool> is_running = true;

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        std::cout << "Too less arguments provided\n";
        return 1;
    }

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    hft_ns::ConfigParser config_parser;
    using ErrorCode = hft_ns::ConfigParser::ErrorCode;
    ErrorCode ec    = config_parser.Parse(argv[1]);
    if (ec != ErrorCode::NoError)
    {
        std::cout << "Error reading the config " << ec << std::endl;
        return 1;
    }

    auto uri_opt = config_parser.GetValue("SOCKET_CONNECTION_URI", "COINBASE");
    if (!uri_opt.has_value())
    {
        std::cout << "Error loading the socket connection uri: " << ec << std::endl;
        return 1;
    }

    const std::string uri = uri_opt.value();

    client c;

    try
    {
        c.set_access_channels(websocketpp::log::alevel::none);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        c.init_asio();
        c.set_tls_init_handler([](connection_hdl) { return on_tls_init(); });

        c.set_open_handler([&](connection_hdl hdl) {
            std::cout << "Connected to Coinbase sandbox WS\n";
            json subscribe_msg = {{"type", "subscribe"},
                                  {"channels", json::array({json{{"name", "ticker"}, {"product_ids", json::array({"BTC-USD"})}}})}};
            c.send(hdl, subscribe_msg.dump(), websocketpp::frame::opcode::text);
        });

        c.set_message_handler([&](connection_hdl, client::message_ptr msg) {
            try
            {
                auto parsed = json::parse(msg->get_payload());
                std::cout << "Handling Message: " << parsed.dump() << "\n";
            }
            catch (const std::exception &e)
            {
                std::cerr << "Parse error: " << e.what() << "\n";
            }
        });

        websocketpp::lib::error_code ec;
        client::connection_ptr       con = c.get_connection(uri, ec);
        if (ec)
        {
            std::cerr << "Connection init error: " << ec.message() << "\n";
            return 1;
        }

        c.connect(con);
        c.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }

    // Additional main thread awaking
    while (is_running.load(std::memory_order_acquire))
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Shutdown completed!\n";
}

void signalHandler(int signal)
{
    std::cout << "Terminate signal " << signal << "received! Shutting down\n";
    is_running.store(false, std::memory_order_release);
}