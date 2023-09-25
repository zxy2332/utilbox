
function(install_func HEADER_FILES)

    #######################################安装部分#################################

    target_include_directories(
            ${PROJECT_NAME} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  # 指定构建接口路径
            $<INSTALL_INTERFACE:include>                    # install会自动引用include头文件
    )


    set_target_properties(${PROJECT_NAME} PROPERTIES    # 设置目标的属性
            DEBUG_POSTFIX d                                 # debug声称是添加d后缀
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}    # 二进制执行文件目录
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}    # 静态库目录
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}    # 动态库目录
    )


    # 设置安装目标
    install(
            TARGETS ${PROJECT_NAME}                         # 安装目标名称
            EXPORT ${PROJECT_NAME}Targets                   # 导出项目名
            PUBLIC_HEADER DESTINATION include               # 指定头文件目录
            #RUNTIME DESTINATION "bin/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>"  # 二进制执行文件目录
            #ARCHIVE DESTINATION "lib/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>"  # 静态库目录
            #LIBRARY DESTINATION "lib/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>"  # 动态库目录
            RUNTIME DESTINATION bin  # 二进制执行文件目录
            ARCHIVE DESTINATION lib  # 静态库目录
            LIBRARY DESTINATION lib  # 动态库目录
    )

    install(FILES ${ARGV} DESTINATION include)

    install(
            EXPORT ${PROJECT_NAME}Targets                   # 导出项目名
            FILE ${PROJECT_NAME}Targets.cmake               # 生成xxxTargets.cmake文件
            # NAMESPACE Ftest::                               # 添加命名空间
            DESTINATION lib/cmake/${PROJECT_NAME}                                # 指定相对路径cmake目录
    )

    include(CMakePackageConfigHelpers)


    #configure_file(     # 生成 xxxConfig.cmake文件
    #        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Config.cmake.in ${PROJECT_NAME}Config.cmake   # 根据.make.in 生成对应Config.cmake文件
    #        INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
    #)

    configure_package_config_file (                     # 生成 xxxConfig.cmake文件
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Config.cmake.in ${PROJECT_NAME}Config.cmake   # 根据.make.in 生成对应Config.cmake文件
            INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}                       # 安装目录
    )

    write_basic_package_version_file(                   # 生成 xxxConfigVersion.cmake文件
            ${PROJECT_NAME}ConfigVersion.cmake              # 指定文件名
            VERSION ${PACKAGE_VERSION}                      # 指定版本号
            COMPATIBILITY SameMajorVersion                  # 指定兼容性
    )


    install(
            FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake         # 复制xxxConfig.cmake文件到安装目录的cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake  # 复制xxxConfigVersion.cmake文件到安装目录的cmake
            DESTINATION lib/cmake/${PROJECT_NAME}                                # 指定相对路径
    )


    install(
            DIRECTORY
            #config/                                         # 复制配置文件夹
            DESTINATION static                              # 指定目录
    )
endfunction()