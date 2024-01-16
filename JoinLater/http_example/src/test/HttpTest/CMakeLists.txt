cmake_minimum_required(VERSION 3.26)
project(HttpTest)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp *.cc *.h *.hpp)

if(CPPHTTPLIB_OPENSSL_SUPPORT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "CPPHTTPLIB_OPENSSL_SUPPORT")
    message("CPPHTTPLIB_OPENSSL_SUPPORT true")
endif()


add_executable(${PROJECT_NAME}  ${SRC})

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/external
)

target_link_libraries(${PROJECT_NAME}

)

