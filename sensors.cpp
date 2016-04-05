#include <sensors/sensors.h>
#include <iostream>
#include "sensors.h"


Sensors::Sensors()
{
    sensors_init(NULL);
}

Sensors::~Sensors()
{
    sensors_cleanup();
}


void Sensors::scan()
{
    sensors_chip_name const * cn;
    int c = 0;

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
			std::cout << cn->prefix << "." << feat->name << "=" << val << std::endl;
                    }
                }
            }
            
        }
    }
}
