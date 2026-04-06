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
#include <libpq-fe.h>

#include <string>
#include <nlohmann/json.hpp>
#include "dotenv.h"
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <zmq.hpp>
#include <mutex>

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
    PGconn *con;
    std::string imei;
    float latitude;
    float longitude;
    float altitude;
    float accuracy;
    double ms;
    int cidIsReg;
    bool isReg;
    bool isNew = false;
    // cellInfoGSMData cellGSM;
    std::vector<cellInfoLteData> cellLTE;
    std::string date;
};

struct dataPlot {
    std::map<int, std::vector<double>> msMap;
    std::map<int, std::vector<double>> dbmMap;
    std::map<int, std::vector<double>> rssnrMap;
    std::map<int, std::vector<double>> rsrpMap;
    std::map<int, std::vector<double>> rsrqMap;
    std::map<int, std::vector<double>> rssiMap;
};

static double start_ms = -1;
static double t = 0;
static int cnt = 1;
std::mutex loc_mutex;

int insertIntoAllLocations(PGconn *con, std::string imei, float latitude, float longitude, float altitude, float accuracy, std::string date, double ms, bool is_reg, int cid_is_reg) {
    std::string query = std::string("INSERT INTO ") + std::string("all_locations") + "(imei, lat, lon, alt, accuracy, date, ms, is_reg, cid_is_reg)" + "VALUES($1, $2, $3, $4, $5, to_timestamp($6, 'HH24:MI DD.MM.YYYY'), $7, $8, $9) RETURNING id";
   
    std::string lat = std::to_string(latitude);
    std::string lon = std::to_string(longitude);
    std::string alt = std::to_string(altitude);
    std::string acc = std::to_string(accuracy);
    std::string ms_s = std::to_string(static_cast<long long>(ms));
    std::string reg = is_reg ? "true" : "false";
    std::string cid = std::to_string(cid_is_reg);
    const char* data_to_insert[] = {imei.c_str(), lat.c_str(), lon.c_str(), alt.c_str(), acc.c_str(), date.c_str(), ms_s.c_str(), reg.c_str(), cid.c_str(), };
   
    PGresult* insert_res = PQexecParams(
        con, 
        query.c_str(),
        9,
        NULL,
        data_to_insert,
        NULL,
        NULL,
        0
    );

    int new_id = -1;
    if (PQresultStatus(insert_res) != PGRES_TUPLES_OK) {
        std::cerr << "Insert \033[31mFAILED\033[0m:" << PQresultErrorMessage(insert_res) << std::endl;
    } else {
        std::cout << "Insert \033[32mSUCCESSFULL!\033[0m\n";
        new_id = std::stoi(PQgetvalue(insert_res, 0, 0));
    }
    
    PQclear(insert_res);
    return new_id;
}

void insertIntoCellLte(PGconn *con, int fk, std::vector<cellInfoLteData> cells) {
    std::string query = std::string("INSERT INTO ") + std::string("cells_lte") + "(all_locations_id, ci, pci, bandwidth, earfcn, mcc, mnc, tac, asu_level, cqi, rsrp, rsrq, rssi, rssnr, dbm, timing_advance)" 
    + "VALUES($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16)";
   
    std::string fk_s = std::to_string(fk);
    for (auto cell : cells) {
        std::string ci = std::to_string(cell.ci);
        std::string pci = std::to_string(cell.pci); 
        std::string bandwidth = std::to_string(cell.bandwidth); 
        std::string earfcn = std::to_string(cell.earfcn); 
        std::string tac = std::to_string(cell.tac); 
        std::string asuLevel = std::to_string(cell.asuLevel); 
        std::string cqi = std::to_string(cell.cqi); 
        std::string rsrp = std::to_string(cell.rsrp); 
        std::string rsrq = std::to_string(cell.rsrq); 
        std::string rssi = std::to_string(cell.rssi);
        std::string rssnr = std::to_string(cell.rssnr); 
        std::string dbm = std::to_string(static_cast<long long>(cell.dbm)); 
        std::string timingAdvance = std::to_string(cell.timingAdvance); 
        const char* data_to_insert[] = {fk_s.c_str(), ci.c_str(), pci.c_str(), bandwidth.c_str(), 
            earfcn.c_str(), cell.mcc.c_str(), cell.mnc.c_str(), tac.c_str(), asuLevel.c_str(), 
            cqi.c_str(), rsrp.c_str(), rsrq.c_str(), rssi.c_str(), rssnr.c_str(), dbm.c_str(), timingAdvance.c_str()};
   
        PGresult* insert_res = PQexecParams(
            con, 
            query.c_str(),
            16,
            NULL,
            data_to_insert,
            NULL,
            NULL,
            0
        );

        if (PQresultStatus(insert_res) != PGRES_COMMAND_OK) {
            std::cerr << "Insert \033[31mFAILED\033[0m:" << PQresultErrorMessage(insert_res) << std::endl;
        }        
        PQclear(insert_res);
    }
}

void selectAllLocations(PGconn *con) {

    PGresult* res = PQexec(con, "SELECT * FROM all_locations");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "\033[31mERROR\033[0m:" << PQresultErrorMessage(res) << std::endl;
        PQfinish(con);
        exit(1);
    }

    int nFields = PQnfields(res);

    for (int i = 0; i < nFields; i++) {
    printf("%-15s", PQfname(res, i));
    }
    printf("\n-----------------------------------\n");

    for (int i = 0; i < PQntuples(res); i++) {
    for (int j = 0; j < nFields; j++) {
        if (PQgetisnull(res, i, j)) {
            printf("%-15s", "NULL");
        } else {
            printf("%-15s", PQgetvalue(res, i, j));
        }
    }
    printf("\n");
    }

}

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
        std::cout << cnt++ << ": Received data from client: " << result.value() << std::endl;

        std::string received_data(static_cast<char*>(request.data()), request.size());

        try {
            nlohmann::json json_data = nlohmann::json::parse(received_data);
            //std::cout << "Parsed data: " << json_data["imei"] << json_data["latitude"] << json_data["longitude"] << json_data["date"] << std::endl;
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

            //std::cout <<  json_data["cellLte"].is_array() << std::endl;
            //std::cout << "SIZE: " << (loc->cellLTE).size() << "ETO ARRAY? : " << json_data["cellLte"].is_array() << std::endl;

            if (file.is_open()) {
                //std::cout << "Write in file parsed data...\n"; 
                file << json_data.dump(4) << std::endl;
                file.flush();
            } 
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
    file.close();
}

void LinePlots(location *loc, dataPlot *data) {
    if (loc->isNew) {
        std::lock_guard<std::mutex> lock(loc_mutex);
        if (start_ms < 0) start_ms = loc->ms;
        t = (loc->ms - start_ms) / 1000;

        for (const auto& cell : loc->cellLTE) {
            data->msMap[cell.pci].push_back(t);
            data->dbmMap[cell.pci].push_back(cell.dbm);
            data->rssnrMap[cell.pci].push_back(cell.rssnr);
            data->rsrpMap[cell.pci].push_back(cell.rsrp);
            data->rsrqMap[cell.pci].push_back(cell.rsrq);
            data->rssiMap[cell.pci].push_back(cell.rssi);
            //std::cout << "Data dbmMap size: " << data->dbmMap.size() << " Added: PCI=" << cell.pci << " Time=" << loc->ms << " Signal=" << cell.dbm << std::endl;
            if (data->msMap[cell.pci].size() > 2000) {
                data->msMap[cell.pci].erase(data->msMap[cell.pci].begin());
                data->dbmMap[cell.pci].erase(data->dbmMap[cell.pci].begin());
                data->rsrpMap[cell.pci].erase(data->rsrpMap[cell.pci].begin());
                data->rsrqMap[cell.pci].erase(data->rsrqMap[cell.pci].begin());
                data->rssiMap[cell.pci].erase(data->rssiMap[cell.pci].begin());
                data->rssnrMap[cell.pci].erase(data->rssnrMap[cell.pci].begin());
            }
        }
        loc->isNew = false;
    }
    if (ImPlot::BeginSubplots("Metrics", 2, 2, ImVec2(-1, 800))) {
        if (ImPlot::BeginPlot("Rssnr")) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 20, ImGuiCond_FirstUseEver);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - 30.0, t, ImGuiCond_Always);
            ImPlot::SetupAxes("Time", "dB", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::PushColormap(ImPlotColormap_Dark); 
            for (const auto& [pci, rssnr] : data->rssnrMap) {
                const auto& x = data->msMap[pci];
                int count = std::min(x.size(), rssnr.size());
                ImPlot::PlotLine(std::to_string(pci).c_str(), x.data(), rssnr.data(), count);
            }
            ImPlot::PopColormap();
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Rsrp")) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, -140, -40, ImGuiCond_FirstUseEver);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - 30.0, t, ImGuiCond_Always);
            ImPlot::SetupAxes("Time", "dB", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::PushColormap(ImPlotColormap_Dark); 
            for (const auto& [pci, rsrp] : data->rsrpMap) {
                const auto& x = data->msMap[pci];
                int count = std::min(x.size(), rsrp.size());
                ImPlot::PlotLine(std::to_string(pci).c_str(), x.data(), rsrp.data(), count);
            }
            ImPlot::PopColormap();
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Rsrq")) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, -20, 0,  ImGuiCond_FirstUseEver);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - 30.0, t, ImGuiCond_Always);
            ImPlot::SetupAxes("Time", "dB", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::PushColormap(ImPlotColormap_Dark); 
            for (const auto& [pci, rsrq] : data->rsrqMap) {
                const auto& x = data->msMap[pci];
                int count = std::min(x.size(), rsrq.size());
                ImPlot::PlotLine(std::to_string(pci).c_str(), x.data(), rsrq.data(), count);
            }
            ImPlot::PopColormap();
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Rssi")) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, -140, -40, ImGuiCond_FirstUseEver);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - 30.0, t, ImGuiCond_Always);
            ImPlot::SetupAxes("Time", "dB", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::PushColormap(ImPlotColormap_Dark); 
            for (const auto& [pci, rssi] : data->rssiMap) {
                const auto& x = data->msMap[pci];
                int count = std::min(x.size(), rssi.size());
                ImPlot::PlotLine(std::to_string(pci).c_str(), x.data(), rssi.data(), count);
            }
            ImPlot::PopColormap();
            ImPlot::EndPlot();
        }
        ImPlot::EndSubplots();
    }
}

void Dbm_LinePlots(location *loc, dataPlot *data) {
    if (ImPlot::BeginPlot("Signal Strength", ImVec2(-1, 800))) {
        ImPlot::SetupAxisLimits(ImAxis_Y1, -140, -40, ImGuiCond_FirstUseEver);
        ImPlot::SetupAxisLimits(ImAxis_X1, t - 30.0, t, ImGuiCond_Always);
        ImPlot::SetupAxes("Time", "dBm", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
        ImPlot::PushColormap(ImPlotColormap_Paired); 
        for (const auto& [pci, dbm] : data->dbmMap) {
            const auto& x = data->msMap[pci];
            int count = std::min(x.size(), dbm.size());
            ImPlot::PlotLine(std::to_string(pci).c_str(), x.data(), dbm.data(), count);
        }
        ImPlot::PopColormap();
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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Включить Multi-Viewport / Platform Windows. Позволяет работать "окнам" вне основного окна. 
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool running = true;
    while (running) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event); 
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_None);

        ImGui::Begin("Data from phone:"); 
        ImGui::Text("Date = %s", loc->date.c_str());
        // ImGui::Text("Imei = %s", loc->imei.c_str());
        // ImGui::Text("Latitude = %.5f", loc->latitude);
        // ImGui::Text("Longitude = %.5f", loc->longitude);
        // ImGui::Text("Altitude = %.5f", loc->altitude);
        // ImGui::Text("Accuracy = %.10f", loc->accuracy);
        // ImGui::Text("Connected to cell: %d", loc->cidIsReg);
        if (ImGui::Button("Select in cli")) {
            selectAllLocations(loc->con);
        }

        LinePlots(loc, data);
        Dbm_LinePlots(loc, data);

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

    dotenv::init("../.env");
    PGconn *con;
    PGresult *res;

    std::string info = "host=" + std::string(std::getenv("HOST")) + " port=" + std::string(std::getenv("PORT")) + " dbname=" + std::string(std::getenv("DB_NAME")) + " user=" + std::string(std::getenv("DB_USER")) + " password=" + std::string(std::getenv("DB_USER_PASSWORD"));
    con = PQconnectdb(info.c_str());

    if(PQstatus(con) != CONNECTION_OK) {
        std::cerr << "Connection to database \033[31mfailed\033[0m: \n" << PQerrorMessage(con) << std::endl;
        PQfinish(con);
        return 1;
    } else {
        locationInfo.con = con;
        std::cout << "Connection to database \033[32msuccessful\033[0m! \n";
    }

    std::thread server_thread(run_server, &locationInfo);
    std::thread gui_thread(run_gui, &locationInfo, &data);

    server_thread.join();
    gui_thread.join();

    return 0;
}