#include "../../include/database/db_queries.h"

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