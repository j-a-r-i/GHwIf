cmake_minimum_required(VERSION 2.6)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(utest
  tdisk.cpp ../disk.cpp
  tweb.cpp  ../web.cpp
  ttimer.cpp)
target_link_libraries(utest ${GTEST_LIBRARIES} pthread curl)