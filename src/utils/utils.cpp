#include "../../include/utils/utils.h"

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void collectAllDots(location *loc, std::vector<pciInfo> *dots) {
    dots->clear();
    PGresult *res = PQexec(loc->con, "SELECT DISTINCT pci FROM cells_lte");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Error fetching PCIs: " << PQerrorMessage(loc->con) << std::endl;
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        int pci = atoi(PQgetvalue(res, i, 0));
        dots->push_back(selectAllMobileData(loc->con, pci));
    }
    PQclear(res);
}