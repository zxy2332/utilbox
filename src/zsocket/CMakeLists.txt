cmake_minimum_required(VERSION 3.26)
project(zsocket VERSION 0.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp *.cc *.h *.hpp)

#add_compile_options(-fPIC)

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME}
		PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)


# 生成头文件
generate_export_header(${PROJECT_NAME})

# 添加头文件的安装目标
file(GLOB HEADERS *.h *.hpp)
set(HEADER_FILES ${HEADERS})


install_func(${HEADER_FILES})