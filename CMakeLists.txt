cmake_minimum_required(VERSION 3.10)
project(d3d8_hook LANGUAGES CXX)

add_library(d3d8 SHARED d3d8.def "main.cpp" "config.cpp" "hooks-helper.cpp")

# 32-bit target
set_target_properties(d3d8 PROPERTIES
    OUTPUT_NAME "d3d8"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Force 32-bit in Visual Studio if needed
if(MSVC)
    set(CMAKE_GENERATOR_PLATFORM "Win32")
endif()

target_link_libraries(d3d8 PRIVATE user32 kernel32)
