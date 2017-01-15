project(ctodo)
set(CTODO_VERSION "1.3")

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option(SYNC_ENABLE "Enable experimental sync feature" OFF)
option(READLINE_ENABLE "Enable readline" ON)
configure_file(src/config.h.in src/config.h)

find_package(Curses REQUIRED)
include_directories("$(CURSES_INCLUDE_DIR)")

if(SYNC_ENABLE)
  find_package(CURL REQUIRED)
  include_directories("$(CURL_INCLUDE_DIR)")
endif()
if(READLINE_ENABLE)
  find_package(Readline REQUIRED)
  include_directories("$(READLINE_INCLUDE_DIR)")
endif()

include_directories(${CMAKE_BINARY_DIR}/src src)

file(GLOB SRC_LIST src/*.c)

if(NOT SYNC_ENABLE)
  list(REMOVE_ITEM SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/sync.c)
endif()
if(READLINE_ENABLE)
  list(REMOVE_ITEM SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/edit.c)
else()
  list(REMOVE_ITEM SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/wedit.c)
endif()

add_executable(ctodo ${SRC_LIST})

target_link_libraries(ctodo ${CURSES_LIBRARIES})
if(SYNC_ENABLE)
  target_link_libraries(ctodo ${CURL_LIBRARIES})
endif()
if(READLINE_ENABLE)
  target_link_libraries(ctodo ${READLINE_LIBRARY})
endif()

install(TARGETS ctodo DESTINATION bin)
