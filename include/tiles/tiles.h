#pragma once
#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <curl/curl.h>
#include <stb_image.h>
#include <future>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath>

#include <filesystem>
#include <fstream>

extern std::map<std::string, GLuint> Mapa;
extern std::unordered_map<std::string, std::future<void*>> LoadingTiles;

size_t onPullResponse(void *data, size_t size, size_t nmemb, void *userp);

std::vector<unsigned char> curlLoad(int z, int x, int y);

void *stbLoad(std::vector<unsigned char> _rawBlob);

GLuint glLoad(const void *data);

int lonToTileX(double lon, int z);

int latToTileY(double merc_y, int z);

double ConvertX (int x, int z);

double ConvertY (int y, int z);

float toMercY(float lat_rad);

std::vector<unsigned char> getTile(int z, int x, int y);