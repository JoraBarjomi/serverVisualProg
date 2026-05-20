#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "../models/structs.h"
#include "../database/db_queries.h"

std::vector<std::string> split(const std::string& s, char delimiter);

void collectAllDots(location *loc, std::vector<pciInfo> *dots);