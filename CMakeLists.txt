cmake_minimum_required(VERSION 3.16)
project(LeWM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Louvre REQUIRED Louvre)
find_package(Threads REQUIRED)

file(GLOB_RECURSE LEWM_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(LeWM ${LEWM_SRC})

target_include_directories(LeWM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" ${Louvre_INCLUDE_DIRS})
target_compile_options(LeWM PRIVATE ${Louvre_CFLAGS_OTHER})
target_link_directories(LeWM PRIVATE ${Louvre_LIBRARY_DIRS})
target_link_libraries(LeWM PRIVATE ${Louvre_STATIC_LIBRARIES} Threads::Threads)

if(DEFINED ENV{LEWM_LD_PATH})
    set_target_properties(LeWM PROPERTIES INSTALL_RPATH "$ENV{LEWM_LD_PATH}")
endif()

install(TARGETS LeWM RUNTIME DESTINATION bin)
install(FILES LeWM.desktop DESTINATION share/wayland-sessions)
