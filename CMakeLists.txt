cmake_minimum_required(VERSION 3.10)

project(sfa-net)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download and extract ENet
include(FetchContent)
FetchContent_Declare(
  enet
  URL https://github.com/lsalzman/enet/archive/v1.3.17.zip
)
FetchContent_GetProperties(enet)
if(NOT enet_POPULATED)
  FetchContent_Populate(enet)
  add_subdirectory(${enet_SOURCE_DIR} ${enet_BINARY_DIR})
endif()

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/app)
include_directories(${enet_SOURCE_DIR}/include)

# Add source files
set(SERVER_SOURCES
    app/server_main.cpp
)
set(CLIENT_SOURCES
    app/client_main.cpp
)

# Add executables
add_executable(server ${SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})

# Link with ENet library and WinSock2
target_link_libraries(server enet ws2_32 winmm)
target_link_libraries(client enet ws2_32 winmm)
