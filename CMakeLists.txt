cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)
set(hw "pc" CACHE STRING "Hardware type rpi/pc/win")

if (hw STREQUAL "rpi")
  message(STATUS "hw is RPI")
  set(HW_RPI ON)
  set(HW_WIN OFF)
elseif (hw STREQUAL "pc")
  message(STATUS "hw is linux pc")
  set(HW_RPI OFF)
  set(HW_WIN OFF)
elseif (hw STREQUAL "win")
  message(STATUS "hw is windows pc")
  set(HW_RPI OFF)
  set(HW_WIN ON)
else()
  message(FATAL_ERROR "unknown hardware")
endif()

option(SCHEME
  "use mini/tinyscheme" ON)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)

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
  /usr/include/glib-2.0
  /usr/lib/x86_64-linux-gnu/glib-2.0/include
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
#[[
		    luascript.cpp
		    lua/src/lauxlib.c
		    lua/src/lapi.c
		    lua/src/lstate.c
		    lua/src/ltable.c
		    lua/src/lmem.c
		    lua/src/ldebug.c
		    lua/src/ldo.c
		    lua/src/lvm.c
		    lua/src/lfunc.c
		    lua/src/ltm.c
		    lua/src/lgc.c
		    lua/src/lobject.c
		    lua/src/lctype.c
		    lua/src/lstring.c
		    lua/src/ldump.c
		    lua/src/lparser.c
		    lua/src/llex.c
		    lua/src/lcode.c
		    lua/src/lzio.c
		    lua/src/lopcodes.c
		    lua/src/lundump.c
		    #lua/src/linit.c
		    lua/src/lbaselib.c
		    lua/src/loadlib.c
		    lua/src/ldblib.c
		    #lua/src/lutf8lib.c
		    #lua/src/lmathlib.c
		    lua/src/lstrlib.c
		    #lua/src/loslib.c
		    #lua/src/liolib.c
		    lua/src/ltablib.c
		    lua/src/lcorolib.c
]]		    
		    scmscript.cpp
		    tinyscheme/scheme.c
		    sun.cpp
		    ${EXTRA_SRCS}
)

target_link_libraries(hwif
  # uv
  # ${CURL_LIBRARIES}
  ${EXPAT_LIBRARIES}
  curl
  sqlite3
  readline
#  lua5.3
  gio-2.0
  gobject-2.0
  glib-2.0
  ${EXTRA_LIBS}
)
