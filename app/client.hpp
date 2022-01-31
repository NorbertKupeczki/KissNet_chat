#ifndef NETTUTORIAL_CLIENT_HPP
#define NETTUTORIAL_CLIENT_HPP

#include "atomic"
#include "iostream"
#include "thread"
#include "kissnet.hpp"
#include "MyPacket.hpp"

class Client {
 public:
    Client();

    kissnet::tcp_socket Connect(const std::string &server_address, kissnet::port_t port);
    void Input(kissnet::tcp_socket &socket, const std::string& user_name);
    void Run(kissnet::tcp_socket&& socket, std::string user_name);

 private:
    std::atomic<bool> connected;
    std::atomic<bool> running;
};

#endif //NETTUTORIAL_CLIENT_HPP
