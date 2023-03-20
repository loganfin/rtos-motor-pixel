# rtos-motor-pixel

An extension of my rtos-motor project that adds WS2812 LED logic.

## Building

This program depends on Pico_WS2812, pico-sdk and FreeRTOS-Kernel.

1. Clone the repository:

    ```
    git clone https://github.com/loganfin/rtos-motor-pixel.git
    ```

2. Configure the build directory using CMake:
    * If you already have the pico-sdk and the FreeRTOS-Kernel on your machine:

        ```
        cmake -B build -DPICO_SDK_PATH=</path/to/pico-sdk/> -DPICO_SDK_FREERTOS_SOURCE=</path/to/FreeRTOS-Kernel>
        ```

    * If you don't have the pico-sdk or the FreeRTOS-Kernel on your machine:

        ```
        git submodule update --init
        git -C lib/pico-sdk/ submodule update --init
        cmake -B build
        ```

3. Build the program:

    ```
    cmake --build build
    ```

The uf2 binary will be located inside the `build` directory that was created with the `cmake -B build` command.
