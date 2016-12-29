cmake_minimum_required (VERSION 2.6)
project (ctodo)
set (ctodo_VERSION_MAJOR 1)
set (ctodo_VERSION_MINOR 3)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

include_directories ("$(PROJECT_BINARY_DIR)")

set (SRC_LIST ctodo.c task.c file.c error.c sync.c stream.c)
# TODO: enable/disable sync support
add_executable (ctodo ${SRC_LIST})
target_link_libraries (ctodo ncurses curl)

install (TARGETS ctodo DESTINATION bin)
