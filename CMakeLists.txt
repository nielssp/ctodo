project(ctodo)
set(ctodo_VERSION_MAJOR 1)
set(ctodo_VERSION_MINOR 3)

cmake_minimum_required(VERSION 2.6)

find_package(Curses REQUIRED)
find_package(CURL REQUIRED)

configure_file(src/config.h.in src/config.h)

include_directories("$(CURSES_INCLUDE_DIR)")
include_directories("$(CURL_INCLUDE_DIR)")
include_directories(src)

file(GLOB_RECURSE SRC_LIST src/*.c)

add_executable(ctodo ${SRC_LIST})

# TODO: enable/disable sync support
target_link_libraries(ctodo ${CURSES_LIBRARIES} ${CURL_LIBRARIES})

install(TARGETS ctodo DESTINATION bin)
