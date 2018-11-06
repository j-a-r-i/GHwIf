cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)
set(hw "pc" CACHE STRING "Hardware type rpi/pc/win")

if (hw STREQUAL "rpi")
  message(STATUS "hw is RPI")
  set(HW_RPI ON)
elseif (hw STREQUAL "pc")
  message(STATUS "hw is linux pc")
  set(HW_RPI OFF)
elseif (hw STREQUAL "win")
  message(STATUS "hw is windows pc")
  set(HW_RPI OFF)
else()
  message(FATAL_ERROR "unknown hardware")
endif()

#option(HW_RPI
#       "Raspberry PI hardware" OFF)

if (HW_RPI)
  set(CMAKE_SYSTEM_NAME    Linux)
  set(CMAKE_SYSTEM_VERSION 1)
  set(CMAKE_C_COMPILER     arm-linux-gnueabihf-gcc)
  set(CMAKE_CXX_COMPILER   arm-linux-gnueabihf-g++)

  set(CMAKE_FIND_ROOT_PATH              /mnt/rpi)
  set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

  include_directories(
    /mnt/rpi/usr/include
    /mnt/rpi/usr/include/arm-linux-gnueabihf
    /mnt/rpi/usr/include/arm-linux-gnueabihf/curl)
  
  link_directories(
    /mnt/rpi/usr/lib
    /mnt/rpi/usr/lib/arm-linux-gnueabihf
    /mnt/rpi/lib)	    
endif (HW_RPI)


find_package(EXPAT REQUIRED)
#find_package(CURL  REQUIRED)


set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)

include(CTest)
include(private.txt)


configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("${PROJECT_BINARY_DIR}"
#  ${CURL_INCLUDE_DIR}
  ${EXPAT_INCLUDE_DIR}
  ../drivers
#  /usr/include/lua5.3
  rpi)

if (HW_RPI)
   set(EXTRA_LIBS ${EXTRA_LIBS} )
   set(EXTRA_SRCS ${EXTRA_SRCS} rpi/main_rpi.cpp rpi/hal.cpp)
else (HW_RPI)
   set(EXTRA_LIBS ${EXTRA_LIBS} sensors)
   set(EXTRA_SRCS ${EXTRA_SRCS} main_pc.cpp sensors.cpp)
endif (HW_RPI)

add_executable(hwif main2.cpp
		    serial.cpp
		    Socket.cpp
		    measures.cpp
		    web.cpp
		    disk.cpp
		    xmlparsesimple.cpp
		    logger.cpp
		    config.cpp
		    # luascript.cpp
		    scmscript.cpp
		    sun.cpp
		    tinyscheme/scheme.c
		    ${EXTRA_SRCS}
)

target_link_libraries(hwif uv
  # ${CURL_LIBRARIES}
  ${EXPAT_LIBRARIES}
  curl
  sqlite3
#  lua5.3
  ${EXTRA_LIBS}
)
