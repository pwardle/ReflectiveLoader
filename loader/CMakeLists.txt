cmake_minimum_required(VERSION 3.2)

project(loader C CXX)
set(CMAKE_CXX_STANDARD 11)

set(TARGET loader)

FILE(GLOB SRC src/*.cpp)
FILE(GLOB HDR include/*.h src/*.h)

add_library(${TARGET} STATIC ${SRC} ${HDR})
target_include_directories(${TARGET} PUBLIC "include")
set_target_properties(${TARGET} PROPERTIES PUBLIC_HEADER include/custom_dlfcn.h)
target_compile_definitions(${TARGET} PRIVATE UNSIGN_TOLERANT=1)

install(TARGETS loader)
