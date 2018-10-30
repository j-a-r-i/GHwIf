cmake_minimum_required(VERSION 2.6)

option(HW_RPI
       "Raspberry PI hardware" OFF)

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
		    
  set(EXTRA_SRCS ${EXTRA_SRCS} thal.cpp ../rpi/hal.c)
endif (HW_RPI)

find_package(GTest REQUIRED)
#find_package(CURL  REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS}
  ../../drivers
#  /usr/include/lua5.3
#  /usr/include/guile/2.0
  ../rpi
  ../tinyscheme
)


add_executable(utest
  tdisk.cpp ../disk.cpp
  tweb.cpp  ../web.cpp
  txml.cpp  ../xmlparsesimple.cpp
  tdb.cpp   ../db.cpp
#  tlua.cpp  ../luascript.cpp
#  tguile.cpp ../guilescript.cpp
  ttimer.cpp
  ttsch.cpp  ../scmscript.cpp ../tinyscheme/scheme.c
  ../logger.cpp ../config.cpp
  tcfg.cpp
  ${EXTRA_SRCS})

target_link_libraries(utest pthread
  ${GTEST_LIBRARIES}
#  ${CURL_LIBRARIES}
  curl
  sqlite3
#  lua5.3
#  guile-2.0
  expat)
