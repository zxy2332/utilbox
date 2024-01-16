cmake_minimum_required(VERSION 3.26)


project(cslog VERSION 0.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp *.cc *.h *.hpp)


add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE ${CMAKE_SOURCE_DIR}/external
)

target_link_libraries(${PROJECT_NAME}

)

generate_export_header(${PROJECT_NAME})


# 添加头文件的安装目标
set(HEADER_FILES cslog.h)


#install_func(${HEADER_FILES})