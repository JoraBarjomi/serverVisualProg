#include "../../include/server/server.h"
#include "../../include/database/db_queries.h"

std::mutex loc_mutex;
int cnt = 0;

void run_server(location *loc) {

    zmq::context_t context (1);
    zmq::socket_t socket (context, zmq::socket_type::rep);    

    zmq_bind(socket, "tcp://*:12345");
    std::cout << ("Server running...\n");

    while (true)
    {
        zmq::message_t request;

        auto result = socket.recv(request, zmq::recv_flags::none);
        assert(result.value_or(0) != 0);
        std::cout << cnt++ << ": Received data from client: " << result.value() << std::endl;

        std::string received_data(static_cast<char*>(request.data()), request.size());

        try {
            nlohmann::json json_data = nlohmann::json::parse(received_data);
            nlohmann::json data1 = json_data["locationInfo"];
            nlohmann::json data2 = json_data["cellGSM"];
            nlohmann::json data3 = json_data["cellLte"];

            std::lock_guard<std::mutex> lock(loc_mutex);
            loc->imei = data1["imei"];
            loc->latitude = data1["latitude"];
            loc->longitude = data1["longitude"];
            loc->altitude = data1["altitude"];
            loc->accuracy = data1["accuracy"];
            loc->cidIsReg = data1["cidIsReg"];
            loc->isReg = data1["IsReg"];
            loc->ms = data1["timeMS"];
            loc->date = data1["date"];
            
            loc->isNew = true;

            int fk = insertIntoAllLocations(loc->con, loc->imei, loc->latitude, loc->longitude, loc->altitude, loc->accuracy, loc->date, loc->ms, loc->isReg, loc->cidIsReg);

            if (data3.is_array()) {
                loc->cellLTE = data3.get<std::vector<cellInfoLteData>>();
            }

            insertIntoCellLte(loc->con, fk, loc->cellLTE);
        }
        catch(const std::exception& e) {
            std::cerr << "Error while parsing json: " << e.what() << '\n';
        }                            

        std::time_t recv_time = std::time(nullptr);
        std::string date_time = std::asctime(std::localtime(&recv_time));
        std::string kReplyString = std::to_string(cnt) + ": " + date_time;
        zmq::message_t reply (kReplyString.data(), kReplyString.size());
        socket.send (reply, zmq::send_flags::none);
    }
}