cmake_minimum_required(VERSION 3.16)
project(LeWM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Louvre REQUIRED)
find_package(Threads REQUIRED)

file(GLOB_RECURSE LEWM_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(LeWM ${LEWM_SRC})

target_link_libraries(LeWM PRIVATE Louvre::Louvre Threads::Threads)
target_include_directories(LeWM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

install(TARGETS LeWM RUNTIME DESTINATION bin)
install(FILES LeWM.desktop DESTINATION share/wayland-sessions)
