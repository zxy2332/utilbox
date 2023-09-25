cmake_minimum_required(VERSION 3.26)


project(cpputil VERSION 0.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp *.cc *.h *.hpp)




find_package(fmt REQUIRED
        PATHS ${CMAKE_SOURCE_DIR}/external/FMT
)

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE  fmt::fmt
)

generate_export_header(${PROJECT_NAME})


# 添加头文件的安装目标
set(HEADER_FILES file_util.h time_util.h)


#install_func(${HEADER_FILES})