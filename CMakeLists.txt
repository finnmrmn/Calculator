cmake_minimum_required(VERSION 3.16)
project(MyGTKGMPProject C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(GMP REQUIRED gmp)

include_directories(${GTK4_INCLUDE_DIRS} ${GMP_INCLUDE_DIR})
link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

add_executable(app main.c)
target_link_libraries(app ${GTK4_LIBRARIES} ${GMP_LIBRARIES})
