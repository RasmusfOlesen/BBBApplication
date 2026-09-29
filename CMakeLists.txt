cmake_minimum_required(VERSION 3.20)

project(BBBApplication LANGUAGES CXX)

find_package(Freetype REQUIRED)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

add_executable(bbb_application
	src/main.cpp
	src/Framebuffer.cpp
    src/Renderer.cpp
    src/Needle.cpp
    src/Gauge.cpp
    src/TextRenderer.cpp
)

target_include_directories(bbb_application PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SYSROOT}/usr/include/libdrm
)

target_link_libraries(bbb_application PRIVATE
    drm
    Freetype::Freetype
)

target_compile_options(bbb_application PRIVATE
    -g
    -O0
)