/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#include <sensors/sensors.h>
#include <iostream>
#include <sstream>
#include <string>
#include "sensors.h"

/*
const char *sensors[] = {
"atk0110.temp1",
"atk0110.temp2",
"coretemp.temp2",
"coretemp.temp3"
};
*/

template<typename Func>
void each_sensor(Func fn)
{
    sensors_chip_name const * cn;
    int c = 0;
    std::stringstream ss;

    while ((cn = sensors_get_detected_chips(0, &c)) != 0)
    {
        //std::cout << cn->prefix << " " << cn->path << std::endl;

        sensors_feature const *feat;
        int f = 0;

        while ((feat = sensors_get_features(cn, &f)) != 0)
        {
            //std::cout << feat->name << std::endl;

            sensors_subfeature const *subf;
            int s = 0;

            while ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0)
            {
		if ((subf->flags & SENSORS_MODE_R) &&
		    (subf->type == SENSORS_SUBFEATURE_TEMP_INPUT))
                {
                    double val = 0.0;
                    int err = sensors_get_value(cn, subf->number, &val);
                    if (err < 0) {
                        std::cout << "error " << err << std::endl;
                    }
                    else {
			ss << cn->prefix << "." << feat->name;
			fn(ss.str().c_str(), val);

			ss.clear();
			ss.str(std::string());
			//std::cout << cn->prefix << "." << feat->name << "=" << val << std::endl;
                    }
                }
            }
            
        }
    }
}

//------------------------------------------------------------------------------
Sensors::Sensors() : InfoItem("sensors")
{
    sensors_init(NULL);

    each_sensor([](const char* name, double value) {
	    std::cout << name << "=" << value << std::endl;
	});
}

Sensors::~Sensors()
{
    sensors_cleanup();
}

void Sensors::read()
{
    each_sensor([](const char* name, double value) {
	    std::cout << name << "=" << value << std::endl;
	});

}

void Sensors::print()
{
}


