cmake_minimum_required(VERSION 3.14)
project(ZHttp)
project(ZHttplib VERSION 0.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp *.cc *.h *.hpp)

option(CPPHTTPLIB_OPENSSL_SUPPORT "CPPHTTPLIB_OPENSSL_SUPPORT" OFF)
if(CPPHTTPLIB_OPENSSL_SUPPORT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "CPPHTTPLIB_OPENSSL_SUPPORT")
    message("CPPHTTPLIB_OPENSSL_SUPPORT true")
endif()

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE ${CMAKE_SOURCE_DIR}/external
)


target_link_libraries(${PROJECT_NAME}
)

generate_export_header(${PROJECT_NAME})

# 添加头文件的安装目标
set(HEADER_FILES zhttplib.h)


#install_func(${HEADER_FILES})