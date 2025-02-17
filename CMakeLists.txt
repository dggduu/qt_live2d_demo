cmake_minimum_required(VERSION 3.30)
project(qt_live2d_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(D_OR_R ON)
set(CONSOLE ON)
set(FRAMEWORK_SOURCE OpenGL)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets Network)
file(GLOB LApp2D LAppLive2D/Src/*.cpp)
message(STATUS ${LApp2D})

if (NOT CONSOLE)
    set(CON WIN32)
endif ()

add_executable(qt_live2d_demo ${CON} main.cpp QtGLCore.cpp ${LApp2D} resources/qrc/resources.qrc
        settingelawin.cpp
        settingelawin.h
        petdialog.cpp
        petdialog.h
        petdialogworker.cpp
        petdialogworker.h
        petdialogworker.ui
)


target_link_libraries(qt_live2d_demo
        "${CMAKE_SOURCE_DIR}/Lib/Debug/libFramework.a"
        "${CMAKE_SOURCE_DIR}/Lib/Debug/libglew32d.dll.a"
        "${CMAKE_SOURCE_DIR}/Lib/Debug/libglfw3.a"
        "${CMAKE_SOURCE_DIR}/Lib/Debug/Live2DCubismCore_d.lib"
#        "${CMAKE_SOURCE_DIR}/Lib/Debug/ElaWidgetTools.a"
        "${CMAKE_SOURCE_DIR}/Lib/Debug/libElaWidgetTools.a"
        "${CMAKE_SOURCE_DIR}/Lib/Debug/libcrypto.dll.a"
        "${CMAKE_SOURCE_DIR}/Lib/Debug/libssl.dll.a"
        opengl32
        glu32
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGLWidgets
        Qt6::Network
)
include_directories(
        ./
        elaWidget/
        LAppLive2D/Inc/
        Thirdparty/Framework/src
        Thirdparty/Core/include
        Thirdparty/glew/include
        Thirdparty/glew/include/GL
        Thirdparty/glfw/include
        Thirdparty/glfw/include/GLFW
        Thirdparty/stb
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
endif()
endif()
if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E make_directory
"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy
"${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
endif()
foreach(QT_LIB Core Gui Widgets )
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy
"${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
"$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endforeach(QT_LIB)
endif()
