#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <zmq.hpp>

#define PORT 12345

int main(int argc, char const* argv[])
{

    zmq::context_t context (1);
    zmq::socket_t socket (context, zmq::socket_type::rep);    

    zmq_bind(socket, "tcp://*:12345");
    std::cout << ("Server running...\n");

    std::ofstream file("./database/data.json", std::ios::app);

    while (true)
    {
        zmq::message_t request;

        auto result = socket.recv(request, zmq::recv_flags::none);
        assert(result.value_or(0) != 0);
        std::cout << "Received data from client...\n";

        std::string received_data(static_cast<char*>(request.data()), request.size());

        if (file.is_open()) {
            std::cout << "Write in file...\n"; 
            file << received_data << std::endl;
            file.flush();
        }   

        std::time_t recv_time = std::time(nullptr);
        std::string date_time = std::asctime(std::localtime(&recv_time));
        std::string kReplyString = "Location received: " + date_time;
        zmq::message_t reply (kReplyString.length());
        std::cout << "Send reply to client...\n"; 
        memcpy (reply.data(), kReplyString.data(), kReplyString.length());
        socket.send (reply, zmq::send_flags::none);

    }
    
    file.close();
    return 0;
}
