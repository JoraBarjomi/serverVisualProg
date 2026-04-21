#include "../../include/gui/plots.h"

double start_ms = -1;
double t = 0;
int cnt_plots = 1;

void LinePlots(location *loc, dataPlot *data) {
    if (loc->isNew) {
        std::lock_guard<std::mutex> lock(loc_mutex);
        if (start_ms < 0) start_ms = loc->ms;
        t = (loc->ms - start_ms) / 1000;

        for (const auto& cell : loc->cellLTE) {
            data->msMap[cell.pci].push_back(t);
            data->dbmMap[cell.pci].push_back(cell.dbm);
            data->rssnrMap[cell.pci].push_back(cell.rssnr);
            data->rsrpMap[cell.pci].push_back(cell.rsrp);
            data->rsrqMap[cell.pci].push_back(cell.rsrq);
            data->rssiMap[cell.pci].push_back(cell.rssi);
            if (data->msMap[cell.pci].size() > 2000) {
                data->msMap[cell.pci].erase(data->msMap[cell.pci].begin());
                data->dbmMap[cell.pci].erase(data->dbmMap[cell.pci].begin());
                data->rsrpMap[cell.pci].erase(data->rsrpMap[cell.pci].begin());
                data->rsrqMap[cell.pci].erase(data->rsrqMap[cell.pci].begin());
                data->rssiMap[cell.pci].erase(data->rssiMap[cell.pci].begin());
                data->rssnrMap[cell.pci].erase(data->rssnrMap[cell.pci].begin());
            }
        }
        loc->isNew = false;
    }
    if (ImPlot::BeginSubplots("Metrics", 2, 2, ImVec2(-1, 800))) {
        if (ImPlot::BeginPlot("Rssnr")) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 20, ImGuiCond_FirstUseEver);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - 30.0, t, ImGuiCond_Always);
            ImPlot::SetupAxes("Time", "dB", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::PushColormap(ImPlotColormap_Dark); 
            for (const auto& [pci, rssnr] : data->rssnrMap) {
                const auto& x = data->msMap[pci];
                int count = std::min(x.size(), rssnr.size());
                ImPlot::PlotLine(std::to_string(pci).c_str(), x.data(), rssnr.data(), count);
            }
            ImPlot::PopColormap();
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Rsrp")) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, -140, -40, ImGuiCond_FirstUseEver);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - 30.0, t, ImGuiCond_Always);
            ImPlot::SetupAxes("Time", "dB", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::PushColormap(ImPlotColormap_Dark); 
            for (const auto& [pci, rsrp] : data->rsrpMap) {
                const auto& x = data->msMap[pci];
                int count = std::min(x.size(), rsrp.size());
                ImPlot::PlotLine(std::to_string(pci).c_str(), x.data(), rsrp.data(), count);
            }
            ImPlot::PopColormap();
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Rsrq")) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, -20, 0,  ImGuiCond_FirstUseEver);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - 30.0, t, ImGuiCond_Always);
            ImPlot::SetupAxes("Time", "dB", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::PushColormap(ImPlotColormap_Dark); 
            for (const auto& [pci, rsrq] : data->rsrqMap) {
                const auto& x = data->msMap[pci];
                int count = std::min(x.size(), rsrq.size());
                ImPlot::PlotLine(std::to_string(pci).c_str(), x.data(), rsrq.data(), count);
            }
            ImPlot::PopColormap();
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Rssi")) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, -140, -40, ImGuiCond_FirstUseEver);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - 30.0, t, ImGuiCond_Always);
            ImPlot::SetupAxes("Time", "dB", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::PushColormap(ImPlotColormap_Dark); 
            for (const auto& [pci, rssi] : data->rssiMap) {
                const auto& x = data->msMap[pci];
                int count = std::min(x.size(), rssi.size());
                ImPlot::PlotLine(std::to_string(pci).c_str(), x.data(), rssi.data(), count);
            }
            ImPlot::PopColormap();
            ImPlot::EndPlot();
        }
        ImPlot::EndSubplots();
    }
}

void Dbm_LinePlots(location *loc, dataPlot *data) {
    if (ImPlot::BeginPlot("Signal Strength", ImVec2(-1, 800))) {
        ImPlot::SetupAxisLimits(ImAxis_Y1, -140, -40, ImGuiCond_FirstUseEver);
        ImPlot::SetupAxisLimits(ImAxis_X1, t - 30.0, t, ImGuiCond_Always);
        ImPlot::SetupAxes("Time", "dBm", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
        ImPlot::PushColormap(ImPlotColormap_Paired); 
        for (const auto& [pci, dbm] : data->dbmMap) {
            const auto& x = data->msMap[pci];
            int count = std::min(x.size(), dbm.size());
            ImPlot::PlotLine(std::to_string(pci).c_str(), x.data(), dbm.data(), count);
        }
        ImPlot::PopColormap();
        ImPlot::EndPlot();
    }
}

void MapPlot(location *loc) {
    
    float lon = 82.94;
    float lat = 55.0;
    float center_x = lon;
    float lat_rad = lat * M_PI / 180.0;
    float mer_y = toMercY(lat_rad);
    float center_y = (mer_y / (2 * M_PI)) * 360.0;
    float span = 0.05;
    ImPlot::SetNextAxesLimits(
        center_x - span, center_x + span, 
        center_y - span, center_y + span, 
        ImGuiCond_Once
    );

    if (ImPlot::BeginPlot("Map frame", ImVec2(-1, 900), ImPlotFlags_Equal)) {

        ImVec2 _uv0{0, 0};
        ImVec2 _uv1{1, 1};
        ImVec4 _tint{1, 1, 1, 1};

        ImPlotRect rect = ImPlot::GetPlotLimits();

        double range_x = rect.X.Max - rect.X.Min;
        if (range_x <= 0.0) range_x = 0.001;
        int iz = static_cast<int>(std::floor(std::log2(360.0 / range_x))) + 3;
        if (iz < 0) iz = 0;
        if (iz > 18) iz = 18;

        int start_x = lonToTileX(rect.X.Min, iz);
        int end_x = lonToTileX(rect.X.Max, iz);
        int start_y = latToTileY(rect.Y.Max, iz);
        int end_y = latToTileY(rect.Y.Min, iz);
        int max_tile_index = (1 << iz) - 1;

        start_x = std::clamp(start_x, 0, max_tile_index);
        end_x = std::clamp(end_x, 0, max_tile_index);
        start_y = std::clamp(start_y, 0, max_tile_index);
        end_y = std::clamp(end_y, 0, max_tile_index);

        if ((end_x - start_x) < 10 && (end_y - start_y) < 10) {
            for (int ix = start_x; ix <= end_x; ix++) {
                for (int iy = start_y; iy <= end_y; iy++) {
                    std::string key = std::to_string(ix) + "/" + std::to_string(iy) + "/" + std::to_string(iz);
                    if (Mapa.find(key) == Mapa.end()) {
                            if (LoadingTiles.find(key) == LoadingTiles.end()) {
                                if (LoadingTiles.size() < 16) {
                                    LoadingTiles[key] = std::async(std::launch::async, [iz, ix, iy]() -> void* {
                                        return stbLoad(getTile(iz, ix, iy));
                                    });
                                }
                            } else {
                                auto& future = LoadingTiles[key];
                                if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                                    void* pixels = future.get();
                                    if (pixels) {
                                        Mapa[key] = glLoad(pixels);
                                        stbi_image_free(pixels);
                                    } else {
                                        Mapa[key] = 0;
                                    }
                                    LoadingTiles.erase(key);
                                }
                            }
                    }
                    ImVec2 bmin{ConvertX(ix, iz), ConvertY(iy + 1, iz)};
                    ImVec2 bmax{ConvertX(ix + 1, iz), ConvertY(iy, iz)};

                    if (Mapa.count(key) && Mapa[key] != 0) {
                        ImPlot::PlotImage(key.c_str(), (void*)(intptr_t)Mapa[key], bmin, bmax);
                    }
                }
            }   
        }    
        ImPlot::EndPlot();

    }

}