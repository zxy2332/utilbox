cmake_minimum_required(VERSION 3.26)


project(ShortestEscape VERSION 0.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp *.cc *.h *.hpp)


add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE ${CMAKE_SOURCE_DIR}/external
        PRIVATE ${CMAKE_SOURCE_DIR}/src/cslog
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE cslog
)

option(ExceptionEnable "ExceptionEnable" ON)
if(ExceptionEnable)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "ExceptionEnable")
    message("ExceptionEnable true")
endif()
option(TimeTestEnable "TimeTestEnable" OFF)
if(TimeTestEnable)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "TIME_TEST")
    message("TimeTestEnable true")
endif()

generate_export_header(${PROJECT_NAME})


# 添加头文件的安装目标
set(HEADER_FILES ShortestEscape.h util.h)


#install_func(${HEADER_FILES})