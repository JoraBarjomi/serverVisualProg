# GeoTracker server

GeoTracker Server is a C++ application that receives geodata from Android-client, visualizes it in real-time plots using Dear ImGui and ImPlot, and stores data in Postgresql.

---

## Tech Stack
- **Server:** C++17
- **UI:** ImGui
- **Charts:** ImPlot
- **Database:** PostgreSQl
- **Build:** CMake

---

## Structure

```
    serverVisualProg/
    ├── include/      # headers.h
    ├── src/          # main.cpp
    ├── data/         # data.json (runtime данные)
    ├── external/     # imgui, implot, dotenv-cpp
    ├── CMakeLists.txt
    └── README.md
```

---

## Getting Started

**Quick start:**
   ```bash
   git clone --recurse-submodules https://github.com/JoraBarjomi/serverVisualProg.git
   cd serverVisualProg
   mkdir build && cd build
   cmake .. && make -j4
   ./main
   ```
