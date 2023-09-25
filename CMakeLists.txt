cmake_minimum_required(VERSION 3.26)
project(utilbox)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(/utf-8)
option(ReleaseWithDebug "Release下带调试信息" OFF)
if(ReleaseWithDebug)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi -Od -Ob0")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:NOREF /OPT:NOICF")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:NOREF /OPT:NOICF")
    message("Release下带调试信息")
endif()

#设置输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin)

INCLUDE(GenerateExportHeader)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/install/)
include(install)
add_subdirectory(src)
