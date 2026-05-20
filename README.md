<a id="readme-top"></a>

<br />
<div align="center">
  <a href="https://github.com/JoraBarjomi/serverVisualProg">
    <img src="Logo.svg" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">GeoTracker Server</h3>

  <p align="center">
    C++17 десктопное приложение для приёма, визуализации и хранения геоданных в реальном времени
    <br />
    <a href="https://github.com/JoraBarjomi/serverVisualProg"><strong>Explore the docs »</strong></a>
    <br />
    <a href="https://github.com/JoraBarjomi/serverVisualProg/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    &middot;
    <a href="https://github.com/JoraBarjomi/serverVisualProg/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
  </p>
</div>

<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contributing">Contributing</a></li>
  </ol>
</details>

## About The Project

[![Product Name Screen Shot][product-screenshot]](https://i.postimg.cc/2ykxGQJw/image.png)

GeoTracker Server — это многопоточное C++17 десктопное приложение, которое:

- Принимает геолокацию и сигнальные данные (RSRP, RSRQ, RSSI, RSSNR, dBm) от Android-клиента через ZeroMQ
- Визуализирует метрики в реальном времени с помощью Dear ImGui и ImPlot
- Рендерит интерактивную карту OpenStreetMap с heatmap-наложением RSRP
- Сохраняет всё в PostgreSQL и ведёт runtime-лог сырых JSON-сообщений

### Built With

[![C++][Cpp-shield]][Cpp-url] [![CMake][CMake-shield]][CMake-url] [![PostgreSQL][Postgres-shield]][Postgres-url] [![ZeroMQ][ZMQ-shield]][ZMQ-url] [![SDL2][SDL2-shield]][SDL2-url] [![OpenGL][OpenGL-shield]][OpenGL-url] [![Dear ImGui][ImGui-shield]][ImGui-url] [![ImPlot][ImPlot-shield]][ImPlot-url] [![nlohmann/json][Json-shield]][Json-url] [![dotenv-cpp][Dotenv-shield]][Dotenv-url] [![libcurl][Curl-shield]][Curl-url] [![stb_image][Stb-shield]][Stb-url]

## Getting Started

Чтобы запустить проект локально, выполните следующие шаги.

### Prerequisites

Установите системные зависимости.

- **Debian / Ubuntu**

  ```sh
  sudo apt update
  sudo apt install -y \
      cmake build-essential \
      libsdl2-dev libglew-dev \
      libpq-dev libzmq3-dev \
      libcurl4-openssl-dev \
      nlohmann-json3-dev \
      libstb-dev
  ```

- **Arch Linux**
  ```sh
  sudo pacman -S cmake sdl2 glew postgresql-libs zeromq curl nlohmann-json stb
  ```

> **Важно:** проект использует Git-подмодули. Если директории `external/imgui`, `external/implot` или `external/dotenv-cpp` пустые, выполните:
>
> ```sh
> git submodule update --init --recursive
> ```

### Installation

1. Клонируйте репозиторий
   ```sh
   git clone --recurse-submodules https://github.com/JoraBarjomi/serverVisualProg.git
   ```
2. Перейдите в директорию проекта
   ```sh
   cd serverVisualProg
   ```
3. Создайте и перейдите в директорию сборки
   ```sh
   mkdir build && cd build
   ```
4. Соберите проект
   ```sh
   cmake ..
   make -j$(nproc)
   ```
5. Скопируйте и настройте `.env`
   ```sh
   cp ../.env.example ../.env
   # Отредактируйте ../.env, указав параметры подключения к PostgreSQL
   ```

## Usage

Запускайте бинарник **строго из директории `build/`**, иначе сломается загрузка `.env` и кэширование тайлов:

```sh
cd build
./main
```

После запуска приложение:

1. Инициализирует подключение к PostgreSQL
2. Запускает **Server Thread** — ZeroMQ REP-сокет на `tcp://*:12345`
3. Запускает **GUI Thread** — окно SDL2/OpenGL с Dear ImGui

Отправьте JSON-пейлоад от Android-клиента через ZMQ REQ на `tcp://localhost:12345`:

```json
{
  "locationInfo": {
    "imei": "123456789012345",
    "lat": 55.7558,
    "lon": 37.6173,
    "alt": 150.0,
    "accuracy": 10.5,
    "date": "12:34 20.05.2026",
    "ms": 1716195240000,
    "is_reg": true,
    "cid_is_reg": 12345
  },
  "cellLte": [
    {
      "ci": 111,
      "pci": 63,
      "bandwidth": 20,
      "earfcn": 1800,
      "tac": 100,
      "asu_level": 30,
      "cqi": 10,
      "rsrp": -85,
      "rsrq": -10,
      "rssi": -70,
      "rssnr": 15,
      "dbm": -85,
      "timing_advance": 1
    }
  ]
}
```

В интерфейсе приложения доступны вкладки:

- **Main Data** — live-графики RSRP, RSRQ, RSSI, RSSNR и dBm по каждому PCI (кольцевой буфер до 2000 точек)
- **Map View** — интерактивная карта OpenStreetMap с IDW-heatmap RSRP

## Contributing

<a href="https://github.com/JoraBarjomi/bd_rgr/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=JoraBarjomi/bd_rgr" alt="contrib.rocks image" />
</a>

## Acknowledgments

- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImPlot](https://github.com/epezent/implot)
- [nlohmann/json](https://github.com/nlohmann/json)
- [ZeroMQ](https://zeromq.org/)
- [OpenStreetMap](https://www.openstreetmap.org/)
- [Best-README-Template](https://github.com/othneildrew/Best-README-Template)

[product-screenshot]: https://i.postimg.cc/2ykxGQJw/image.png
[Cpp-shield]: https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white
[Cpp-url]: https://isocpp.org/
[CMake-shield]: https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white
[CMake-url]: https://cmake.org/
[Postgres-shield]: https://img.shields.io/badge/PostgreSQL-316192?style=for-the-badge&logo=postgresql&logoColor=white
[Postgres-url]: https://www.postgresql.org/
[ZMQ-shield]: https://img.shields.io/badge/ZeroMQ-FF6600?style=for-the-badge&logo=zeromq&logoColor=white
[ZMQ-url]: https://zeromq.org/
[SDL2-shield]: https://img.shields.io/badge/SDL2-0F0F0F?style=for-the-badge&logo=sdl&logoColor=white
[SDL2-url]: https://www.libsdl.org/
[OpenGL-shield]: https://img.shields.io/badge/OpenGL-3.3-5586A4?style=for-the-badge&logo=opengl&logoColor=white
[OpenGL-url]: https://www.opengl.org/
[ImGui-shield]: https://img.shields.io/badge/Dear%20ImGui-0A0A0A?style=for-the-badge&logo=cplusplus&logoColor=white
[ImGui-url]: https://github.com/ocornut/imgui/tree/docking
[ImPlot-shield]: https://img.shields.io/badge/ImPlot-114CB3?style=for-the-badge&logo=cplusplus&logoColor=white
[ImPlot-url]: https://github.com/epezent/implot
[Json-shield]: https://img.shields.io/badge/nlohmann%2Fjson-00882B?style=for-the-badge&logo=json&logoColor=white
[Json-url]: https://github.com/nlohmann/json
[Dotenv-shield]: https://img.shields.io/badge/dotenv--cpp-ECD53F?style=for-the-badge&logo=dotenv&logoColor=black
[Dotenv-url]: https://github.com/mrtazz/dotenv-cpp
[Curl-shield]: https://img.shields.io/badge/libcurl-073551?style=for-the-badge&logo=curl&logoColor=white
[Curl-url]: https://curl.se/libcurl/
[Stb-shield]: https://img.shields.io/badge/stb__image-4A4A4A?style=for-the-badge&logo=c&logoColor=white
[Stb-url]: https://github.com/nothings/stb
