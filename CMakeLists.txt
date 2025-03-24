cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/sys/cmake/toolchain.cmake")
project(Hakkun-Example VERSION 1.0 LANGUAGES ASM C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(config/config.cmake)

file(GLOB_RECURSE SOURCES_ASM ${PROJECT_SOURCE_DIR}/src/*.S)
file(GLOB_RECURSE SOURCES_C ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE SOURCES_CXX ${PROJECT_SOURCE_DIR}/src/*.cpp)
set(SOURCES ${SOURCES_ASM} ${SOURCES_C} ${SOURCES_CXX})
add_executable(${PROJECT_NAME} ${SOURCES})
include_directories(lib/imgui)

include(sys/cmake/module.cmake)
include(sys/cmake/deploy.cmake)
include(sys/cmake/sail.cmake)
usesail(${PROJECT_NAME})
