
cmake_minimum_required(VERSION 3.12)
project(MultithreadedWebServer CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

add_executable(webserver
    src/main.cpp
    src/server.cpp
    src/thread_pool.cpp
    src/http.cpp
    src/logger.cpp
)

target_include_directories(webserver PRIVATE include)
target_compile_options(webserver PRIVATE -O3 -Wall -Wextra -Wpedantic -pthread)
target_link_libraries(webserver pthread)
