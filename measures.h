#ifndef __MEASURE_H
#define __MEASURE_H

#include <string>

class Measure
{
public:
    enum EMeasureItem {
	TEMP1 = 0,
	TEMP2,
	TEMP3,
	MAX_TEMP
    };

    Measure() {
	for (int i=0; i<MAX_TEMP; i++) {
	    temp[i] = 0.0;
	}
    }
    
    void set(EMeasureItem i, float t) {
	temp[i] = t;
    }

    void set(int i, float t) {
	temp[i] = t;
    }

    std::string& getJson();
    
private:
    float temp[MAX_TEMP];

    std::string tmp;
};

#endif //__MEASURE_H
