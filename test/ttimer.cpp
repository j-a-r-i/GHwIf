#include <gtest/gtest.h>
#include <iostream>
#include <chrono>
#include <thread>


TEST(Timer, One)
{
    int i;
    for (i=0; i<4; i++) {
	//std::cout << "TIMER "
	//          << i
	//	  << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }			      
}
