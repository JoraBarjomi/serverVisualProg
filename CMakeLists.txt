cmake_minimum_required(VERSION 3.14)
project(GeoTrackerServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json QUIET)

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/external)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set ( ZMQ_LIBRARIES ${ZMQ_LIBRARY} )
set ( ZMQ_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR} )

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${INCLUDE_DIR})

find_path (ZMQ_INCLUDE_DIR zmq.h)
find_library( ZMQ_LIBRARY NAMES zmq )

include(FindPkgConfig)
pkg_check_modules(STB REQUIRED stb)

add_library(imgui STATIC
    ${THIRD_PARTY_DIR}/imgui/imgui.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_sdl2.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${THIRD_PARTY_DIR}/imgui)
target_compile_options(imgui PRIVATE -fPIC)

add_library(implot STATIC
    ${THIRD_PARTY_DIR}/implot/implot.cpp
    ${THIRD_PARTY_DIR}/implot/implot_demo.cpp
    ${THIRD_PARTY_DIR}/implot/implot_items.cpp
)
target_include_directories(implot PUBLIC ${THIRD_PARTY_DIR}/implot)
target_link_libraries(implot PRIVATE imgui)
target_compile_options(implot PRIVATE -fPIC)

add_subdirectory(${THIRD_PARTY_DIR}/dotenv-cpp)

add_executable(main 
                ${SRC_DIR}/main.cpp 
                ${SRC_DIR}/database/db_con.cpp 
                ${SRC_DIR}/database/db_queries.cpp 
                ${SRC_DIR}/gui/gui.cpp 
                ${SRC_DIR}/gui/plots.cpp 
                ${SRC_DIR}/server/server.cpp 
                ${SRC_DIR}/tiles/tiles.cpp 
                ${SRC_DIR}/utils/utils.cpp 
                )
target_link_libraries(main PRIVATE imgui implot ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${ZMQ_LIBRARIES} ${STB_LIBRARIES} zmq PostgreSQL::PostgreSQL dotenv CURL::libcurl)
target_include_directories(main PRIVATE ${STB_INCLUDE_DIRS})