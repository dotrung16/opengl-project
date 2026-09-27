cmake_minimum_required(VERSION 3.20)
project(opengl-project LANGUAGES C CXX)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(APP_ENABLE_LTO "Enable LTO for Release/RelWithDebInfo" ON)
if(APP_ENABLE_LTO AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error LANGUAGES C CXX)
  if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
  else()
    message(STATUS "IPO/LTO not supported: ${ipo_error}")
  endif()
endif()

find_package(glfw3 3.4 REQUIRED)

add_subdirectory(external/glad)

add_executable(app
  src/main.cpp
  src/app.cpp
  src/window.cpp
  src/renderer.cpp
  src/frame_pacer.cpp
  src/frame_stats.cpp
)

target_include_directories(app PRIVATE include)
target_link_libraries(app PRIVATE glfw glad)
target_compile_definitions(app PRIVATE GLFW_INCLUDE_NONE)

target_compile_options(app PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
  $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-ffunction-sections -fdata-sections>
)

target_link_options(app PRIVATE
  $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-Wl,--gc-sections>
)
