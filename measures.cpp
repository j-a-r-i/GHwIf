#include "serial.h"
#include <sstream>
#include <iostream>

std::string& Measure::getJson()
{
    std::ostringstream os;

    os << "{";

    for (int i=0; i<MAX_TEMP; i++) {
	os << "t" << (i+1) << "=" << temp[i];
	if (i < MAX_TEMP-1) {
	    os << ",";
	}
    }

    os << "}";

    tmp = os.str();

    //std::cout << tmp << std::endl;
    
    return tmp;
}
