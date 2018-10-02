cmake_minimum_required(VERSION 2.6)

option(HW_RPI
       "Raspberry PI hardware" OFF)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("${PROJECT_BINARY_DIR}")

if (HW_RPI)
   set(EXTRA_LIBS ${EXTRA_LIBS} )
   set(EXTRA_SRCS ${EXTRA_SRCS} rpi/gpio.cpp)
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
		    ${EXTRA_SRCS}
)

target_link_libraries(hwif uv
			   curl
			   ${EXTRA_LIBS}
)
