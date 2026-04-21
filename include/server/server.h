#pragma once
#include <iostream>
#include <zmq.hpp>
#include <mutex>
#include "../models/structs.h"

extern std::mutex loc_mutex;
extern int cnt;

void run_server(location *loc);