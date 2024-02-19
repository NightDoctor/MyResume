cmake_minimum_required(VERSION 3.10)
project(UDPServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

file(GLOB SOURCE_MAIN main.cpp)

file(GLOB SOURCE_SERVER server.cpp)
file(GLOB SOURCE_SERVER_H server.h)

file(GLOB SOURCE_CLIENT_H client.h)

file(GLOB SOURCE_DEVICE device.cpp worker.cpp)
file(GLOB SOURCE_DEVICE_H device.h worker.h)

file(GLOB SOURCE_COMMON_H common.h)
file(GLOB SOURCE_CONFIG_H config.h)

set(SOURCE_INCLUDES
    ${SOURCE_MAIN}

    ${SOURCE_SERVER}
    ${SOURCE_SERVER_H}

    ${SOURCE_CLIENT_H}

    ${SOURCE_DEVICE}
    ${SOURCE_DEVICE_H}

    ${SOURCE_COMMON_H}
    ${SOURCE_CONFIG_H}
        common.cpp
)

add_executable(main ${SOURCE_INCLUDES})
