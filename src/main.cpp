#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "implot.h"

#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <ctime>
#include <zmq.hpp>

struct cellInfoLteData {
    int ci;  
    int pci;
    bool isReg;
    int bandwidth;
    int earfcn;
    std::string mcc;
    std::string mnc;
    int tac;
    int asuLevel;
    int cqi;
    int rsrp;
    int rsrq;
    int rssi;
    int rssnr; 
    int dbm;
    int timingAdvance;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(cellInfoLteData, ci, pci, isReg, bandwidth, earfcn, mcc, mnc, tac, asuLevel, cqi, rsrp, rsrq, rssi, rssnr, dbm, timingAdvance)
struct cellInfoGSMData {
    int cid;  
    int bsic;
    int arfcn;
    int lac;
    std::string mcc;
    std::string mnc;
    std::string psc;
    int rssi; 
    int dbm;
    int timingAdvance;
};

struct location {
    std::string imei;
    float latitude;
    float longitude;
    float altitude;
    float accuracy;
    long long ms;
    cellInfoGSMData cellGSM;
    std::vector<cellInfoLteData> cellLTE;
    std::string date;
};

void run_server(location *loc) {

    zmq::context_t context (1);
    zmq::socket_t socket (context, zmq::socket_type::rep);    

    zmq_bind(socket, "tcp://*:12345");
    std::cout << ("Server running...\n");
    std::ofstream file("../src/database/data.json", std::ios::app);

    while (true)
    {
        zmq::message_t request;

        auto result = socket.recv(request, zmq::recv_flags::none);
        assert(result.value_or(0) != 0);
        std::cout << "Received data from client: " << result.value() << std::endl;

        std::string received_data(static_cast<char*>(request.data()), request.size());

        try {
            nlohmann::json json_data = nlohmann::json::parse(received_data);
            std::cout << "Parsed data: " << json_data["imei"] << json_data["latitude"] << json_data["longitude"] << json_data["date"] << std::endl;
            loc->imei = json_data["imei"];
            loc->ms = json_data["timeMS"];
            loc->date = json_data["date"];
            loc->latitude = json_data["latitude"];
            loc->longitude = json_data["longitude"];
            loc->altitude = json_data["altitude"];
            loc->accuracy = json_data["accuracy"];
 

           // loc->cellGSM = json_data["cellGSM"];
            // if (json_data.contains("cellLte") && json_data["cellLte"].is_array()) {
            //     loc->cellLTE = json_data["cellLte"].get<std::vector<cellInfoLteData>>();
            // }


            if (file.is_open()) {
                std::cout << "Write in file parsed data...\n"; 
                file << json_data.dump(4) << std::endl;
                file.flush();
            } 
        }
        catch(const std::exception& e) {
            std::cerr << "Error while parsing json: " << e.what() << '\n';
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
}

void run_gui(location *loc){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Window* window = SDL_CreateWindow(
        "Backend start", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Включить Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Включить Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Включить Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Включить Multi-Viewport / Platform Windows. Позволяет работать "окнам" вне основного окна. 
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool running = true;
    // auto last_frame_time = std::chrono::steady_clock::now();
    while (running) {

        // Обработка event'ов (inputs, window resize, mouse moving, etc.)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            //std::cout << "Processing some event: "<< event.type << " timestamp: " << event.motion.timestamp << std::endl;
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_None);

        ImGui::Begin("Data from phone:"); 
        ImGui::Text("Imei = %s", loc->imei.c_str());
        ImGui::Text("Latitude = %.5f", loc->latitude);
        ImGui::Text("Longitude = %.5f", loc->longitude);
        ImGui::Text("Altitude = %.5f", loc->altitude);
        ImGui::Text("Accuracy = %.10f", loc->accuracy);
        //ImGui::Text("Massiv of data = %d", loc->cellLTE);
        ImGui::Text("Date = %s", loc->date.c_str());
        ImGui::End();

        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    static location locationInfo;

    std::thread server_thread(run_server, &locationInfo);
    std::thread gui_thread(run_gui, &locationInfo);

    server_thread.join();
    gui_thread.join();

    return 0;
}