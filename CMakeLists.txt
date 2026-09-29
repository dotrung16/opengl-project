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
option(APP_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(APP_BUILD_TESTS "Build unit tests" ON)

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

add_library(app_options INTERFACE)
target_compile_features(app_options INTERFACE cxx_std_20)
target_compile_options(app_options INTERFACE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Wshadow>
  $<$<AND:$<BOOL:${APP_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:GNU,Clang>>:-Werror>
  $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-ffunction-sections -fdata-sections>
)

add_library(app_core STATIC
  src/app.cpp
  src/window.cpp
  src/renderer.cpp
  src/frame_pacer.cpp
  src/frame_stats.cpp
  src/input.cpp
)
target_include_directories(app_core PUBLIC include)
target_link_libraries(app_core PUBLIC app_options PRIVATE glfw glad)
target_compile_definitions(app_core PRIVATE GLFW_INCLUDE_NONE)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE app_core)
target_link_options(app PRIVATE
  $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-Wl,--gc-sections>
)

if(APP_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
