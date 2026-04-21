#include "../../include/database/db_con.h"

void initConnection(location *loc){

    PGconn *con;
    PGresult *res;
    dotenv::init("../.env");

    std::string info = "host=" + std::string(std::getenv("HOST")) + " port=" + std::string(std::getenv("PORT")) + " dbname=" + std::string(std::getenv("DB_NAME")) + " user=" + std::string(std::getenv("DB_USER")) + " password=" + std::string(std::getenv("DB_USER_PASSWORD"));
    con = PQconnectdb(info.c_str());

    if(PQstatus(con) != CONNECTION_OK) {
        std::cerr << "Connection to database \033[31mfailed\033[0m: \n" << PQerrorMessage(con) << std::endl;
        PQfinish(con);
    } else {
        loc->con = con;
        std::cout << "Connection to database \033[32msuccessful\033[0m! \n";
    }

}