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
    double dbm;
    int timingAdvance;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(cellInfoLteData, ci, pci, bandwidth, earfcn, mcc, mnc, tac, asuLevel, cqi, rsrp, rsrq, rssi, rssnr, dbm, timingAdvance)
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
    double ms;
    int cidIsReg;
    bool isReg;
    // cellInfoGSMData cellGSM;
    std::vector<cellInfoLteData> cellLTE;
    std::string date;
};

struct dataPlot {
    std::map<int, std::vector<double>> msMap;
    std::map<int, std::vector<double>> cellMap;
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
            //std::cout << "Parsed data: " << json_data["imei"] << json_data["latitude"] << json_data["longitude"] << json_data["date"] << std::endl;
            nlohmann::json data1 = json_data["locationInfo"];
            nlohmann::json data2 = json_data["cellGSM"];
            nlohmann::json data3 = json_data["cellLte"];

            loc->imei = data1["imei"];
            loc->ms = data1["timeMS"];
            loc->date = data1["date"];
            loc->latitude = data1["latitude"];
            loc->longitude = data1["longitude"];
            loc->altitude = data1["altitude"];
            loc->accuracy = data1["accuracy"];
            loc->cidIsReg = data1["cidIsReg"];
            loc->isReg = data1["IsReg"];
            
            if (data3.is_array()) {
                loc->cellLTE = data3.get<std::vector<cellInfoLteData>>();
            }

            std::cout <<  json_data["cellLte"].is_array() << std::endl;

            std::cout << "SIZE: " << (loc->cellLTE).size() << "ETO ARRAY? : " << json_data["cellLte"].is_array() << std::endl;

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

void Demo_LinePlots(location *loc, dataPlot *data) {
    if (loc) {
        for (const auto& cell : loc->cellLTE) {
            data->msMap[cell.ci].push_back(loc->ms);
            data->cellMap[cell.ci].push_back(cell.dbm);
        }
    }
    if (ImPlot::BeginPlot("Line Plots", ImVec2(-1, 400))) {
        ImPlot::SetupAxes("Time", "dBm", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
        for (const auto& [ci, y] : data->cellMap) {
            const auto& x = data->msMap[ci];
            ImPlot::PlotLine(std::to_string(ci).c_str(), x.data(), y.data(), (int)y.size());
        }
        ImPlot::EndPlot();
    }
}

void run_gui(location *loc, dataPlot *data){

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

        Demo_LinePlots(loc, data);

        ImGui::Begin("Data from phone:"); 
        ImGui::Text("Imei = %s", loc->imei.c_str());
        ImGui::Text("Latitude = %.5f", loc->latitude);
        ImGui::Text("Longitude = %.5f", loc->longitude);
        ImGui::Text("Altitude = %.5f", loc->altitude);
        ImGui::Text("Accuracy = %.10f", loc->accuracy);
        ImGui::Text("Connected to cell: %d", loc->cidIsReg);

        for (int i = 0; i < (int)loc->cellLTE.size(); i++) {
            ImGui::Text("%d. CI = %d dmb = %f", i + 1, loc->cellLTE[i].ci, loc->cellLTE[i].dbm);
        }

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
    static dataPlot data;

    std::thread server_thread(run_server, &locationInfo);
    std::thread gui_thread(run_gui, &locationInfo, &data);

    server_thread.join();
    gui_thread.join();

    return 0;
}