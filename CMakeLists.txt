cmake_minimum_required(VERSION 2.6)

option(HW_RPI
       "Raspberry PI hardware" OFF)

find_package(EXPAT REQUIRED)
find_package(CURL  REQUIRED)


set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)

include(private.txt)


configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("${PROJECT_BINARY_DIR}"
  ${CURL_INCLUDE_DIR}
  ${EXPAT_INCLUDE_DIR}
  ../drivers
  /usr/include/lua5.3
  rpi)

if (HW_RPI)
   set(EXTRA_LIBS ${EXTRA_LIBS} )
   set(EXTRA_SRCS ${EXTRA_SRCS} rpi/hal.c)
else (HW_RPI)
   set(EXTRA_LIBS ${EXTRA_LIBS} sensors)
   set(EXTRA_SRCS ${EXTRA_SRCS} sensors.cpp)
endif (HW_RPI)

add_executable(hwif main2.cpp
		    serial.cpp
		    Socket.cpp
		    measures.cpp
		    web.cpp
		    disk.cpp
		    xmlparsesimple.cpp
		    logger.cpp
		    luascript.cpp
		    ${EXTRA_SRCS}
)

target_link_libraries(hwif uv
  ${CURL_LIBRARIES}
  ${EXPAT_LIBRARIES}
  sqlite3
  lua5.3
  ${EXTRA_LIBS}
)
