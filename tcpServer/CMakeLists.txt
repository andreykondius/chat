set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")

set(SRC
    ../ServerMain.cpp
	TcpServer.cpp
)

add_executable(Server WIN32 ${SRC})

target_link_libraries(Server
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
)

target_include_directories(Server
    PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
)
