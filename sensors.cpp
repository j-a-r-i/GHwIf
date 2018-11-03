/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#include <sensors/sensors.h>
#include "sensors.h"
#include "logger.h"

template<typename Func>
void each_sensor(Func fn)
{
    sensors_chip_name const * cn;
    int c = 0;

    while ((cn = sensors_get_detected_chips(0, &c)) != 0)
    {
        //std::cout << "-----------------" << cn->prefix << " " << cn->path << std::endl;

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
		    char *label = sensors_get_label(cn, feat);
                    int err = sensors_get_value(cn, subf->number, &val);
                    if (err < 0) {
			Log::err(__FUNCTION__, "sensor_get_value");
                    }
                    else {
			fn(label, val, subf->number);
                    }
                }
            }
            
        }
    }
}

//------------------------------------------------------------------------------
Sensors::Sensors() : InfoReader("sensors")
{
    sensors_init(NULL);

    each_sensor([](const char* name, double value, int subFeature) {
	    Log::value(name, value);
	});
}

Sensors::~Sensors()
{
    sensors_cleanup();
}

void Sensors::read()
{
    each_sensor([](const char* name, double value, int subFeature) {
	    Log::value(name, value);
	});

}

void Sensors::print()
{
}


