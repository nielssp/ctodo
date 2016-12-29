project(ctodo)
set(CTODO_VERSION "1.3")

cmake_minimum_required(VERSION 2.6)

option(SYNC_ENABLE "Enable experimental sync feature" OFF)
configure_file(src/config.h.in src/config.h)

find_package(Curses REQUIRED)
include_directories("$(CURSES_INCLUDE_DIR)")

if(SYNC_ENABLE)
  find_package(CURL REQUIRED)
  include_directories("$(CURL_INCLUDE_DIR)")
endif()

include_directories(src)

file(GLOB SRC_LIST src/*.c)

if(NOT SYNC_ENABLE)
  list(REMOVE_ITEM SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/sync.c)
endif()

add_executable(ctodo ${SRC_LIST})

target_link_libraries(ctodo ${CURSES_LIBRARIES})
if(SYNC_ENABLE)
  target_link_libraries(ctodo ${CURL_LIBRARIES})
endif()

install(TARGETS ctodo DESTINATION bin)
