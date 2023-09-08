cmake_minimum_required(VERSION 3.26)
project(cpputil)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp *.cc *.h *.hpp)


find_package(fmt  REQUIRED
        PATHS ${CMAKE_SOURCE_DIR}/external/FMT
)

add_library(${PROJECT_NAME} SHARED ${SRC}
        time_util.cpp
        time_util.h)

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE fmt::fmt
)

generate_export_header(${PROJECT_NAME})