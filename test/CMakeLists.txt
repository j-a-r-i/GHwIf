cmake_minimum_required(VERSION 2.6)

option(HW_RPI
       "Raspberry PI hardware" OFF)

find_package(GTest REQUIRED)
find_package(CURL  REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS}
  ../../drivers
  ../rpi)

if (HW_RPI)
   set(EXTRA_SRCS ${EXTRA_SRCS} thal.cpp ../rpi/hal.c)
endif (HW_RPI)

add_executable(utest
  tdisk.cpp ../disk.cpp
  tweb.cpp  ../web.cpp
  txml.cpp  ../xmlparsesimple.cpp
  tdb.cpp   ../db.cpp
  ttimer.cpp
  ${EXTRA_SRCS})

target_link_libraries(utest pthread
  ${GTEST_LIBRARIES}
  ${CURL_LIBRARIES}
  sqlite3
  expat)
