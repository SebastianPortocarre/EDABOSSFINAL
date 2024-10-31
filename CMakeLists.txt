cmake_minimum_required(VERSION 3.29)
project(Trabajo_final_eda)

set(CMAKE_CXX_STANDARD 14)

# Incluir directorios para Zstd
include_directories("/opt/homebrew/include")

# Agregar las rutas de las bibliotecas de Zstd
link_directories("/opt/homebrew/lib")

# Agregar los archivos fuente de tu proyecto
add_executable(Trabajo_final_eda
        main.cpp
        CuckooHashTab.cpp
        Compressor.cpp
        RegistroCiudadanos.cpp
        Tablas.cpp
        Sugeridor.cpp
)

# Vincular las bibliotecas de Zstd
target_link_libraries(Trabajo_final_eda
        /opt/homebrew/lib/libzstd.dylib
)