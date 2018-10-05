cmake_minimum_required(VERSION 2.6)

find_package(GTest REQUIRED)
find_package(CURL  REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(utest
  tdisk.cpp ../disk.cpp
  tweb.cpp  ../web.cpp
  txml.cpp  ../xmlparsesimple.cpp
  ttimer.cpp)

target_link_libraries(utest pthread
  ${GTEST_LIBRARIES}
  ${CURL_LIBRARIES}
  expat)
