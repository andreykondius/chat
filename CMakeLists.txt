cmake_minimum_required(VERSION 3.0)
project(Chat)

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Network Widgets REQUIRED)
if (Qt5Core_FOUND AND Qt5Network_FOUND AND Qt5Widgets_FOUND)
    message("Qt5 COMPONENTS Core Network Widgets was found !!!")
else ()
    message("Qt5 COMPONENTS Core Network Widgets was not found")
endif()

add_subdirectory(tcpServer)
add_subdirectory(tcpClient)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} CACHE INTERNAL "" FORCE)
