add_subdirectory(cslog)
add_subdirectory(ShortestEscape)
add_subdirectory(MainService)
add_subdirectory(MainHttpService)

option(BuildTest "BuildTest" OFF)
if(BuildTest)
    message("BuildTest true")
    add_subdirectory(test)
endif()

