#include <iostream>

#include <thread>
#include <curl/curl.h>

#include "../include/server/server.h"
#include "../include/gui/gui.h"
#include "../include/database/db_con.h"
#include "../include/database/db_queries.h"
#include "../include/gui/plots.h"
#include "../include/models/structs.h"
#include "../include/tiles/tiles.h"
#include "../include/utils/utils.h"

int main(int argc, char *argv[]) {
    
    curl_global_init(CURL_GLOBAL_ALL);
    static location locationInfo;
    static dataPlot data;
    initConnection(&locationInfo);

    std::thread server_thread(run_server, &locationInfo);
    std::thread gui_thread(run_gui, &locationInfo, &data);

    server_thread.join();
    gui_thread.join();

    curl_global_cleanup();

    return 0;
}