cmake_minimum_required(VERSION 3.26)
project(zsocket)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp *.cc *.h *.hpp)

#add_compile_options(-fPIC)

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME}
		PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)


# 生成头文件
generate_export_header(${PROJECT_NAME})