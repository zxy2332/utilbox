cmake_minimum_required(VERSION 3.26)
project(cpputil_test)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp *.cc *.h *.hpp)


add_executable(${PROJECT_NAME}  ${SRC})

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/src/cpputil
)
target_link_libraries(${PROJECT_NAME}
    cpputil
)

