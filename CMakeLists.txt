cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_INSTALL_PREFIX "/opt")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SERIAL_PORT   "/dev/ttyS0" CACHE STRING "serial port that is used")
set(SQLITE_DB     "data.db"    CACHE STRING "sqlite database file")
set(VERSION_MAJOR  0           CACHE INTEGER "version number1")
set(VERSION_MINOR  4           CACHE INTEGER "version number2")

option(HW_LINUX      "build for linux/windows"   ON)
option(HW_RPI        "build for raspberry pi"    OFF)
option(SCR_PYTHON    "scripting with python"     ON)
option(SCR_SCHEME    "scripting with tinyscheme" OFF)
option(SCR_LUA       "scripting with lua"        OFF)
option(USE_SENSORS   "use lmsensors"             ON)
option(USE_BLUETOOTH "use bluetooth"             OFF)
option(USE_WS        "use websocket"             OFF)
option(USE_JSON      "use json"                  OFF)
option(USE_LIBUV     "use libuv"                 ON)

set(USE_GLIB OFF)
set(USE_READLINE OFF)

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


if (HW_LINUX)
  find_package(EXPAT REQUIRED)
else (HW_LINUX)
  set(EXPAT_INCLUDE_DIR C:/usr/Expat 2.2.6/Source/lib)
endif (HW_LINUX)
#find_package(CURL  REQUIRED)

include(CTest)
include(private.txt)


configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

set(EXTRA_LIBS ${EXTRA_LIBS})
set(EXTRA_INCS ${EXTRA_INCS})

if (HW_RPI)
  list(APPEND EXTRA_INCS ../drivers)
  set(EXTRA_SRCS ${EXTRA_SRCS} rpi/main_rpi.cpp rpi/hal.cpp)
else (HW_RPI)
  set(EXTRA_SRCS ${EXTRA_SRCS} main_pc.cpp)
endif (HW_RPI)

if (SCR_SCHEME)
  list(APPEND EXTRA_SRCS tinyscheme/scheme.c)
  list(APPEND EXTRA_SRCS scmfunctions.cpp scmscript.cpp)
endif (SCR_SCHEME)

if (SCR_LUA)
  #list(APPEND EXTRA_INCS /usr/include/lua5.3)
  #list(APPEND EXTRA_LIBS lua5.3)
  list(APPEND EXTRA_SRCS
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
		    lua/src/lbaselib.c
		    lua/src/loadlib.c
		    lua/src/ldblib.c
		    lua/src/lstrlib.c
		    lua/src/ltablib.c
		    lua/src/lcorolib.c)
endif (SCR_LUA)

if (SCR_PYTHON)
  list(APPEND EXTRA_INCS
    /usr/include/python2.7)
  list(APPEND EXTRA_SRCS
    pyscript.cpp)
  list(APPEND EXTRA_LIBS
    python2.7)
endif (SCR_PYTHON)

if (USE_LIBUV_SOURCES)
  list(APPEND EXTRA_SRCS
    libuv-v1.24.1/src/uv-common.c
    libuv-v1.24.1/src/strscpy.c
    libuv-v1.24.1/src/timer.c
    libuv-v1.24.1/src/inet.c
    libuv-v1.24.1/src/threadpool.c
    libuv-v1.24.1/src/unix/core.c
    libuv-v1.24.1/src/unix/async.c
    libuv-v1.24.1/src/unix/linux-core.c
    libuv-v1.24.1/src/unix/loop.c
    libuv-v1.24.1/src/unix/udp.c
    libuv-v1.24.1/src/unix/tcp.c
    libuv-v1.24.1/src/unix/thread.c
    libuv-v1.24.1/src/unix/poll.c
    libuv-v1.24.1/src/unix/signal.c
    libuv-v1.24.1/src/unix/process.c
    libuv-v1.24.1/src/unix/tty.c
    libuv-v1.24.1/src/unix/pipe.c
    libuv-v1.24.1/src/unix/stream.c
    libuv-v1.24.1/src/unix/linux-syscalls.c
    libuv-v1.24.1/src/unix/linux-inotify.c
    libuv-v1.24.1/src/unix/sysinfo-memory.c
    )
  list(APPEND EXTRA_INCS
    libuv-v1.24.1/include
    libuv-v1.24.1/src)
  list(APPEND EXTRA_LIBS
    pthread)
endif (USE_LIBUV_SOURCES)

if (USE_LIBUV)
  list(APPEND EXTRA_LIBS
    uv
    pthread)
endif (USE_LIBUV)  

if (USE_BLUETOOTH)
  list(APPEND EXTRA_SRCS btooth.cpp)
  list(APPEND EXTRA_LIBS bluetooth)
endif (USE_BLUETOOTH)

if (USE_SENSORS)
  list(APPEND EXTRA_SRCS sensors.cpp)
  list(APPEND EXTRA_LIBS sensors)
endif (USE_SENSORS)

if (USE_GLIB)
  list(APPEND EXTRA_INCS
    /usr/include/glib-2.0
    /usr/lib/x86_64-linux-gnu/glib-2.0/include)
  
  list(APPEND EXTRA_LIBS
    gio-2.0
    gobject-2.0
    glib-2.0)
endif (USE_GLIB)

if (USE_READLINE)
  list(APPEND EXTRA_LIBS  readline)
endif (USE_READLINE)

if (HW_LINUX)
  list(APPEND EXTRA_SRCS disk.cpp)
endif (HW_LINUX)

include_directories("${PROJECT_BINARY_DIR}"
  #  ${CURL_INCLUDE_DIR}
  ${EXPAT_INCLUDE_DIR}
  ../drivers
  rpi
  external
  ${EXTRA_INCS}
  )

add_executable(hwif main2.cpp
		    file.cpp
		    web.cpp
		    db.cpp
		    xmlparsesimple.cpp
		    htmlparse.cpp
		    logger.cpp
		    config.cpp
		    sun.cpp
		    site.cpp
		    ssh.cpp
		    EventLoop.cpp
		    EventLoopSelect.cpp
		    infoitem.cpp
		    external/sqlite3.c
		    ${EXTRA_SRCS}
)

target_link_libraries(hwif
  # uv
  # ${CURL_LIBRARIES}
  ${EXPAT_LIBRARIES}
  curl
  tidy
  ssh
  dl  # for sqlite3
  ${EXTRA_LIBS}
)
