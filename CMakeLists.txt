project(ctodo)
set(ctodo_VERSION_MAJOR 1)
set(ctodo_VERSION_MINOR 3)

cmake_minimum_required(VERSION 2.6)

find_package(Curses REQUIRED)
find_package(CURL REQUIRED)

configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("$(CURSES_INCLUDE_DIR)")
include_directories("$(CURL_INCLUDE_DIR)")
include_directories("$(PROJECT_BINARY_DIR)")

set(SRC_LIST ctodo.c task.c file.c error.c sync.c stream.c)

add_executable(ctodo ${SRC_LIST})

# TODO: enable/disable sync support
target_link_libraries(ctodo ${CURSES_LIBRARIES} ${CURL_LIBRARIES})

install(TARGETS ctodo DESTINATION bin)
