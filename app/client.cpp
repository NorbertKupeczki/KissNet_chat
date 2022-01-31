#include "client.hpp"

Client::Client() : connected(false) , running(false) { }

kissnet::tcp_socket Client::Connect(const std::string &server_address, kissnet::port_t port)
{
    // attempt connection to server and return the connected socket
    kissnet::tcp_socket client_socket(kissnet::endpoint{server_address, port});
    client_socket.connect(0);
    connected = true;
    return client_socket;
}

void Client::Input(kissnet::tcp_socket &socket, const std::string& user_name) {
    while (running) {
        // grab the user input
        std::string str;
        std::getline(std::cin, str);
        std::cin.clear();

        // is the client still connected?
        if (connected) {
            MyPacket packet;
            packet << MyPacket::PacketID::STRING;
            packet << user_name << str;
            socket.send(&packet.data()[0], packet.length());

            MyPacket packet2;
            packet2 << MyPacket::PacketID::FLOAT;
            packet2 << user_name <<  5.52352F;
            socket.send(&packet2.data()[0], packet2.length());
        }
    }
}

void Client::Run(kissnet::tcp_socket&& socket, std::string user_name) {
    // 1. start input thread
    std::thread input_thread ([&] {
        Input(socket, user_name);
    });

    running = true;
    input_thread.detach();

    // 2. receive messages
    while(running && connected)
    {
// same size as the server buffer
        kissnet::buffer<4096> static_buffer;
        while (connected)
        {
            // structured bindings in play again
            if (auto[size, valid] = socket.recv(static_buffer); valid)
            {
                if (valid.value == kissnet::socket_status::cleanly_disconnected)
                {
                    connected = false;
                    std::cout << "clean disconnection" << std::endl;
                    socket.close();
                    break;
                }

                // treat all data as string and append null terminator
                if (size < static_buffer.size()) static_buffer[size] = std::byte{0};
//                std::cout << reinterpret_cast <char *>(static_buffer.data()) << '\n';

                MyPacket packet;
                auto bytes_received = static_buffer.size();
                packet.data().resize(bytes_received);
                std::copy(static_buffer.begin(), static_buffer.begin() + static_cast<ptrdiff_t>(bytes_received),
                          &packet.data()[0]);

                MyPacket::PacketID id;
                packet >> id;
                switch(id)
                {
                    case MyPacket::PacketID::STRING:
                    {
                        std::string data, user;
                        packet >> user >> data;
                        std::cout << user << ": " << data << "\n";
                    } break;
                    case MyPacket::PacketID::INT:
                    {
                        int data = 0;
                        packet >> data;
                        std::cout << data << "\n";
                    } break;
                    case MyPacket::PacketID::FLOAT:
                    {
                        std::string user;
                        float data;
                        packet >> user >> data;
                        std::cout << user << ": " << data << "\n";
                    } break;
                    case MyPacket::PacketID::PLAYER:
                    {
                        std::cout << "Player Data";
                    } break;

//                    default:
//                    {
//                        std::cout << "error \n";
//                    } break;
                }
            } else
            {
                connected = false;
                std::cout << "disconnected" << std::endl;
                socket.close();
            }
        }
    }
}

// program entrypoint
int main(int /*argc*/, char* /*argv*/ [])
{
    Client client;
    std::string user;
    std::cout << "Pick a username: ";
    std::getline(std::cin, user);
    std::cin.clear();
    std::cout << "---- Messenger ---- \n";

    auto socket = client.Connect("127.0.0.1", 12321);
    // create a string to send
    // std::string str {"hello server!"};
    // using the strings data and its length cast to a std::byte and send it
    // socket.send(reinterpret_cast<std::byte*>(str.data()), str.length());
    client.Run(std::move(socket), user);

    return EXIT_SUCCESS;
}
