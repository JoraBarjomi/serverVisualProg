#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <libpq-fe.h>
#include <../include/models/structs.h>

int insertIntoAllLocations(PGconn *con, std::string imei, float latitude, float longitude, float altitude, float accuracy, std::string date, double ms, bool is_reg, int cid_is_reg);
void insertIntoCellLte(PGconn *con, int fk, std::vector<cellInfoLteData> cells);
void selectAllLocations(PGconn *con);