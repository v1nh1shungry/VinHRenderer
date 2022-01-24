cmake_minimum_required(VERSION 3.0.0)
project(VinHRenderer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CONFIGURATION_TYPES "Release")

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -GL -fp:fast")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto -ffast-math")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(INC_PATH include ThirdParty/include)
include_directories(${INC_PATH})

set(SOURCES src/camera.cpp
            src/depth_shader.cpp
            src/gl.cpp
            src/initializer.cpp
            src/light.cpp
            src/matrix.cpp
            src/model.cpp
            src/phong_shader.cpp
            src/texture_shader.cpp
            src/white_shader.cpp
            ThirdParty/src/tgaimage.cpp
            ThirdParty/src/win32.cpp
            main.cpp)

add_executable(VinHRenderer ${SOURCES})