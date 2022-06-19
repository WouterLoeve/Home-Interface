# ESP32-LoRa

## How to run
1. Install esp-idf: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-setup.html
2. Clone https://github.com/TaraHoleInIt/tarablessd1306 from components dir
3. Go to settings.h: and uncomment the board definition
4. Execute ```idf.py menuconfig```
5. Execute ```idf.py flash```.
6. Execute ```idf.py monitor```

## Folder contents

The project **Home-Interface** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Oled drawing Wiki: https://github.com/TaraHoleInIt/tarablessd1306/wiki/Drawing