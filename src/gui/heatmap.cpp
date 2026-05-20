#include "../../include/gui/heatmap.h"
#include "../../include/tiles/tiles.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

struct RGB {
  float r, g, b;
}; 

struct HSL {
  float h, s, l;
};

HSL rgb2hsl(float r, float g, float b) {
  HSL result;
  
  r /= 255;
  g /= 255;
  b /= 255;
  
  float max = MAX(MAX(r,g),b);
  float min = MIN(MIN(r,g),b);
  
  result.h = result.s = result.l = (max + min) / 2;

  if (max == min) {
    result.h = result.s = 0;
  }
  else {
    float d = max - min;
    result.s = (result.l > 0.5) ? d / (2 - max - min) : d / (max + min);
    
    if (max == r) {
      result.h = (g - b) / d + (g < b ? 6 : 0);
    }
    else if (max == g) {
      result.h = (b - r) / d + 2;
    }
    else if (max == b) {
      result.h = (r - g) / d + 4;
    }
    
    result.h /= 6;
  }

  return result;  
}

float hue2rgb(float p, float q, float t) {
  if (t < 0) 
    t += 1;
  if (t > 1) 
    t -= 1;
  if (t < 1./6) 
    return p + (q - p) * 6 * t;
  if (t < 1./2) 
    return q;
  if (t < 2./3)   
    return p + (q - p) * (2./3 - t) * 6;

  return p;
}

RGB hsl2rgb(float h, float s, float l) {
  RGB result;
  
  if(0 == s) {
    result.r = result.g = result.b = l * 255; 
  }
  else {
    float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    float p = 2 * l - q;
    result.r = hue2rgb(p, q, h + 1./3) * 255;
    result.g = hue2rgb(p, q, h) * 255;
    result.b = hue2rgb(p, q, h - 1./3) * 255;
  }

  return result;
}

bool isCalculated = false;
bool readyToRender = false;
int width = 512;
int height = 512;

std::vector<std::byte> pixels(width * height * 4);
double _minX = 0, _maxX = 0, _minY = 0, _maxY = 0;
GLuint _textureId = 0;
bool _hasNewData = false;

ImVec4 ValueToColor(double value)
{
  double lin = std::pow(10.0, value  / 10.0);
  double linMin = std::pow(10.0, -110.0 / 10.0);
  double linMax = std::pow(10.0, -80.0  / 10.0);
  float t = std::clamp(static_cast<float>((lin - linMin) / (linMax - linMin)), 0.0f, 1.0f);
  RGB rgb = hsl2rgb(0.666f * (1.0f - t), 1.0f, 0.5f);
  return ImVec4(rgb.r / 255.0f, rgb.g / 255.0f, rgb.b / 255.0f, 0.75f);
}

std::pair<double, double> IDW(const std::vector<Dot>& heatmapDots, double x, double y)
{
  double wgt = 0, wgt_tmp, result = 0;
  double minDist = 999999;
  for (int i = 0; i < heatmapDots.size(); i++) {
      double dist = pow(heatmapDots[i].x - x, 2) + pow(heatmapDots[i].y - y, 2);
      if (dist < minDist)
        minDist = dist;
      wgt_tmp = 1.0/dist;
      wgt += wgt_tmp;
      result += wgt_tmp * heatmapDots[i].value;
  }
  result /= wgt;
  return std::pair<double, double>(result, minDist);
}

std::vector<Dot> heatmapDots;
void RenderHeatmap(const std::vector<pciInfo> &pcis)
{
    if (!readyToRender && !isCalculated)
    {
      isCalculated = true;
      for (const auto& pcinf : pcis)
      {
        for (size_t i = 0; i < pcinf.latitudeV.size(); ++i)
        {
          double lat = pcinf.latitudeV[i];
          double lon = pcinf.longitudeV[i];
          if (lat < 54.0 || lat > 56.0 || lon < 81.0 || lon > 84.0) continue;

          double lat_rad = lat * M_PI / 180.0;
          double final_y = (toMercY(lat_rad) / (2 * M_PI)) * 360.0;
          heatmapDots.push_back({lon, final_y, (double)pcinf.rsrpV[i]});
        }
      }

      std::thread([]()
      {
        if (heatmapDots.empty()) {
          isCalculated = false;
          return;
        }

        double minX = 999999; double maxX = -999999;
        double minY = 999999; double maxY = -999999;

        for (const auto& dot : heatmapDots) {
          if (dot.x < minX) minX = dot.x;
          if (dot.x > maxX) maxX = dot.x;
          if (dot.y < minY) minY = dot.y;
          if (dot.y > maxY) maxY = dot.y;
        }
        minX -= 0.02; maxX += 0.02;
        minY -= 0.02; maxY += 0.02;

        for (int py = 0; py < height; py++)
        {
          for (int px = 0; px < width; px++)
          {
            double x = minX + (maxX - minX) * (double)px / (width - 1);
            double y = maxY + (minY - maxY) * (double)py / (height - 1);
            auto [value, dist] = IDW(heatmapDots, x, y);
            float alphaFactor = (dist >= 0.0000015) ? 0.0f : 1.0f;

            ImVec4 color = ValueToColor(value);

            int idx = (py * width + px) * 4;
            pixels[idx + 0] = static_cast<std::byte>(std::clamp(color.x * 255.0f, 0.0f, 255.0f));
            pixels[idx + 1] = static_cast<std::byte>(std::clamp(color.y * 255.0f, 0.0f, 255.0f));
            pixels[idx + 2] = static_cast<std::byte>(std::clamp(color.z * 255.0f, 0.0f, 255.0f));
            pixels[idx + 3] = static_cast<std::byte>(std::clamp(static_cast<float>(color.w * 255.0f * 0.75f * alphaFactor), 0.0f, 255.0f));
          }
        }

        _minX = minX; _maxX = maxX; _minY = minY; _maxY = maxY;

        isCalculated = false;
        readyToRender = true;
        _hasNewData = true;
      }).detach();
    }

    if (_hasNewData)
    {
      if (_textureId == 0) glGenTextures(1, &_textureId);
      glBindTexture(GL_TEXTURE_2D, _textureId);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
      _hasNewData = false;
    }

    if (_textureId != 0 && readyToRender && !_hasNewData)
    {
      ImPlotPoint bmin{_minX, _minY};
      ImPlotPoint bmax{_maxX, _maxY};
      ImVec2 uv0{0, 0};
      ImVec2 uv1{1, 1};
      ImPlot::PlotImage("Heatmap", (ImTextureID)(intptr_t)_textureId, bmin, bmax, uv0, uv1);
    }
}