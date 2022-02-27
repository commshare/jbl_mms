#include "room_server.hpp"
using namespace mms;
RoomServer::RoomServer(ThreadWorker *worker) : worker_(worker) {

}

RoomServer::~RoomServer() {

}

bool RoomServer::start(uint16_t port) {
    if (!worker_) {
        return false;
    }

    worker_->post([port, this]() {
        try {
            // Set logging settings
            set_access_channels(websocketpp::log::alevel::all);
            clear_access_channels(websocketpp::log::alevel::frame_payload);

            // Initialize Asio
            init_asio();
            // Register our message handler
            set_open_handler(std::bind(&RoomServer::onOpen, this, websocketpp::lib::placeholders::_1));
            set_close_handler(std::bind(&RoomServer::onClose, this, websocketpp::lib::placeholders::_1));
            set_message_handler(std::bind(&RoomServer::onMessage, this, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

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

    return 0;
}

void RoomServer::onOpen(websocketpp::connection_hdl hdl) {

}

void RoomServer::onClose(websocketpp::connection_hdl hdl) {

}

void RoomServer::onMessage(websocketpp::server<websocketpp::config::asio>* server, websocketpp::connection_hdl hdl, message_ptr msg) {

}

void RoomServer::stop() {

}