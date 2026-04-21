#include <../include/models/structs.h>
#include <../include/tiles/tiles.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <mutex>

#include "../../external/imgui/imgui.h"
#include "../../external/implot/implot.h"
#include "../../external/imgui/backends/imgui_impl_opengl3.h"
#include "../../external/imgui/backends/imgui_impl_sdl2.h"

extern double start_ms;
extern double t;
extern int cnt_plots;
extern std::mutex loc_mutex;

void LinePlots(location *loc, dataPlot *data);

void Dbm_LinePlots(location *loc, dataPlot *data);

void MapPlot(location *loc);