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

INCLUDE(GenerateExportHeader)

add_subdirectory(src)
