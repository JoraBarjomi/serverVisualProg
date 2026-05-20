#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <zmq.hpp>
#include <mutex>
#include <fstream>
#include "stb_image_write.h"
#include "../models/structs.h"
#include "../utils/utils.h"

extern std::mutex loc_mutex;
extern int cnt;

void run_server(location *loc);