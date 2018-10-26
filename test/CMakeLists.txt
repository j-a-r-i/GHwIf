cmake_minimum_required(VERSION 2.6)

option(HW_RPI
       "Raspberry PI hardware" OFF)

find_package(GTest REQUIRED)
find_package(CURL  REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS}
  ../../drivers
  /usr/include/lua5.3
#  /usr/include/guile/2.0
  ../rpi
  ../tinyscheme
)

if (HW_RPI)
   set(EXTRA_SRCS ${EXTRA_SRCS} thal.cpp ../rpi/hal.c)
endif (HW_RPI)

add_executable(utest
  tdisk.cpp ../disk.cpp
  tweb.cpp  ../web.cpp
  txml.cpp  ../xmlparsesimple.cpp
  tdb.cpp   ../db.cpp
  tlua.cpp  ../luascript.cpp
#  tguile.cpp ../guilescript.cpp
  ttimer.cpp
  ttsch.cpp ../tinyscheme/scheme.c
  ../logger.cpp ../config.cpp
  tcfg.cpp
  ${EXTRA_SRCS})

target_link_libraries(utest pthread
  ${GTEST_LIBRARIES}
  ${CURL_LIBRARIES}
  sqlite3
  lua5.3
#  guile-2.0
  expat)
