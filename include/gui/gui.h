#pragma once
#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "implot.h"

#include "../models/structs.h"
#include "../gui/plots.h"
#include "../database/db_queries.h"
#include "../tiles/tiles.h"
#include "../utils/utils.h"

void run_gui(location *loc, dataPlot *data);