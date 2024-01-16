set(PROJ_NAME behavior_util)

#自动使用Qt工具构建
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB SRCS *.cc *.h *.cpp *.hpp)
set(EXPORTED_HDR ${CMAKE_CURRENT_SOURCE_DIR}/${PROJ_NAME}_export.h)
set(SRCS ${SRCS} ${EXPORTED_HDR})

SET(QT_EXTERNEL_DIR ${EXTERNEL_DIR}/Qt5.14)

#包含目录
include_directories(${CMAKE_CURRENT_BINARY_DIR} private
					${CMAKE_SOURCE_DIR}/src/common
					${CMAKE_SOURCE_DIR}/src/behavior_common
					${CMAKE_SOURCE_DIR}/src/behavior_common/custom_ui
					${CMAKE_SOURCE_DIR}/src/behavior_common/common_define
					${EXTERNEL_DIR}
					${QT_EXTERNEL_DIR}/include
					${QT_EXTERNEL_DIR}/include/QtCore
					${QT_EXTERNEL_DIR}/include/QtWidgets
					${QT_EXTERNEL_DIR}/include/QtGui
					${QT_EXTERNEL_DIR}/include/QtWebEngine
					${QT_EXTERNEL_DIR}/include/QtWebEngineCore
					${QT_EXTERNEL_DIR}/include/QtWebEngineWidgets
)
#库目录
LINK_DIRECTORIES( ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} 

)


link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
	${QT_EXTERNEL_DIR}/Qt5/lib
)

add_library(${PROJ_NAME} SHARED ${SRCS})
				
#链接库TARGET_
IF(WIN32)
  target_link_libraries(${PROJ_NAME} PRIVATE 
						custom_ui
						csexpression
				 PUBLIC ${Qt5Widgets_LIBRARIES}
						${Qt5Core_LIBRARIES}
						${Qt5WebEngine_LIBRARIES}
						${Qt5WebEngineCore_LIBRARIES}
						${Qt5WebEngineWidgets_LIBRARIES}
  )
ENDIF(WIN32)

if(UNIX)
target_link_libraries(${PROJ_NAME} dl)
set_target_properties(${PROJ_NAME} 
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin/)
else(UNIX)
set_target_properties(${PROJ_NAME} 
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin/)
endif(UNIX)

generate_export_header(${PROJ_NAME} EXPORT_FILE_NAME ${EXPORTED_HDR})

SET_TARGET_PROPERTIES(${PROJ_NAME} PROPERTIES FOLDER "behavior_common")
