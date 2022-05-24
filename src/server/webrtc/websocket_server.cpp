#include <boost/asio/spawn.hpp>
#include "websocket_server.hpp"
using namespace mms;

WebsocketServer::~WebsocketServer() {

}

bool WebsocketServer::start(uint16_t port) {
    work_thread_ = std::make_shared<std::thread>([this, port]() {
        try {
            // Set logging settings
            set_access_channels(websocketpp::log::alevel::all);
            clear_access_channels(websocketpp::log::alevel::frame_payload);

            // Initialize Asio
            init_asio();
            set_reuse_addr(true);
            // Register our message handler
            set_open_handler(std::bind(&WebsocketServer::onWebsocketOpen, this, websocketpp::lib::placeholders::_1));
            set_close_handler(std::bind(&WebsocketServer::onWebsocketClose, this, websocketpp::lib::placeholders::_1));
            set_message_handler(std::bind(&WebsocketServer::onMessage, this, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

            // Listen on port
            listen(port);
            
            // Start the server accept loop
            start_accept();

            // Start the ASIO io_service run loop
            run();
        } catch (websocketpp::exception const & e) {
            std::cout << e.what() << std::endl;
        } catch (...) {
            std::cout << "other exception" << std::endl;
        }
    });
    return true;

    // boost::asio::spawn(worker_->getIOContext(), [this, port](boost::asio::yield_context yield) {
    //     try {
    //         // Set logging settings
    //         set_access_channels(websocketpp::log::alevel::all);
    //         clear_access_channels(websocketpp::log::alevel::frame_payload);

    //         // Initialize Asio
    //         init_asio();
    //         // Register our message handler
    //         set_open_handler(std::bind(&WebsocketServer::onWebsocketOpen, this, websocketpp::lib::placeholders::_1));
    //         set_close_handler(std::bind(&WebsocketServer::onWebsocketClose, this, websocketpp::lib::placeholders::_1));
    //         set_message_handler(std::bind(&WebsocketServer::onMessage, this, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

    //         // Listen on port
    //         listen(port);

    //         // Start the server accept loop
    //         start_accept();

    //         // Start the ASIO io_service run loop
    //         run();
    //     } catch (websocketpp::exception const & e) {
    //         std::cout << e.what() << std::endl;
    //     } catch (...) {
    //         std::cout << "other exception" << std::endl;
    //     }
    // });
    // worker_->post([port, this]() {
    //     try {
    //         // Set logging settings
    //         set_access_channels(websocketpp::log::alevel::all);
    //         clear_access_channels(websocketpp::log::alevel::frame_payload);

    //         // Initialize Asio
    //         init_asio();
    //         // Register our message handler
    //         set_open_handler(std::bind(&WebsocketServer::onWebsocketOpen, this, websocketpp::lib::placeholders::_1));
    //         set_close_handler(std::bind(&WebsocketServer::onWebsocketClose, this, websocketpp::lib::placeholders::_1));
    //         set_message_handler(std::bind(&WebsocketServer::onMessage, this, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

    //         // Listen on port
    //         listen(port);

    //         // Start the server accept loop
    //         start_accept();

    //         // Start the ASIO io_service run loop
    //         run();
    //     } catch (websocketpp::exception const & e) {
    //         std::cout << e.what() << std::endl;
    //     } catch (...) {
    //         std::cout << "other exception" << std::endl;
    //     }
    // });

    return true;
}

void WebsocketServer::onMessage(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl, message_ptr msg) {

}

void WebsocketServer::stop() {
    stop_listening();
    if (work_thread_ && work_thread_->joinable()) {
        work_thread_->join();
    }
}