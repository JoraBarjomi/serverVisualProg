#pragma once
#include <vector>
#include <algorithm>
#include <utility>
#include "../models/structs.h"
#include "imgui.h"
#include "implot.h"

struct Dot {
    double x;
    double y;
    double value;
};

void RenderHeatmap(const std::vector<pciInfo>& dots);