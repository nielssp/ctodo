cmake_minimum_required (VERSION 2.6)
project (ctodo)
set (ctodo_VERSION_MAJOR 1)
set (ctodo_VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

include_directories("$(PROJECT_BINARY_DIR)")

add_executable (ctodo ctodo.c)
target_link_libraries (ctodo ncurses)

install (TARGETS ctodo DESTINATION bin)
