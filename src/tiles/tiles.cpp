#include "../../include/tiles/tiles.h"

namespace fs = std::filesystem;

std::map<std::string, GLuint> Mapa;
std::unordered_map<std::string, std::future<void*>> LoadingTiles;

size_t onPullResponse(void *data, size_t size, size_t nmemb, void *userp) {
  size_t realsize{size * nmemb};
  auto &blob = *static_cast<std::vector<unsigned char> *>(userp);
  auto const *const dataptr{static_cast<unsigned char *>(data)};
  blob.insert(blob.cend(), dataptr, dataptr + realsize);
  return realsize;
}

std::vector<unsigned char> curlLoad(int z, int x, int y) {
    std::vector<unsigned char> blob;

    CURL *curl{curl_easy_init()};

    std::ostringstream urlmaker;
    urlmaker << "https://a.tile.openstreetmap.org";
    urlmaker << '/' << z << '/' << x << '/' << y << ".png";

    curl_easy_setopt(curl, CURLOPT_URL, urlmaker.str().c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&blob);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, onPullResponse);

    const bool ok{curl_easy_perform(curl) == CURLE_OK};
    curl_easy_cleanup(curl);

    return blob;
}

void *stbLoad(std::vector<unsigned char> _rawBlob) {
    int w, h, c;
    return stbi_load_from_memory(
        _rawBlob.data(),
        _rawBlob.size(),
        &w, &h, &c, STBI_rgb_alpha
    );
}

GLuint glLoad(const void *data) {
    GLuint _id{0};
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    return _id;
}

float toMercY(float lat_rad) {
    return log(tan(lat_rad) + 1.0 / (cos(lat_rad)));
}

int lonToTileX(double lon, int z) {
    return static_cast<int>(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

int latToTileY(double merc_y, int z) {
    return static_cast<int>(floor((0.5 - merc_y / 360.0) * pow(2.0, z)));
}

double ConvertX (int x, int z) {
    return ((x / static_cast<double>(1 << z) - 0.5) * 360.0);
}

double ConvertY (int y, int z) {
    return ((0.5 - y / static_cast<double>(1 << z)) * 360.0);
}

std::vector<unsigned char> getTile(int z, int x, int y) {

    std::string dirs = std::to_string(z) + "/" + std::to_string(x) + "/";
    std::string path = dirs + std::to_string(y) + ".png";

    if (fs::exists(path)) {
        std::ifstream file(path, std::ios::binary);
        if (file) {
            return std::vector<unsigned char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        }
    }

    std::vector<unsigned char> data = curlLoad(z, x, y);

    if (data.size() > 100) {
        fs::create_directories(dirs);
        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }

    return data;
}