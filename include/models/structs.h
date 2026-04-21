#pragma once
#include <string>
#include <map>
#include <vector>
#include <libpq-fe.h>
#include <nlohmann/json.hpp>

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