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
		    char *label = sensors_get_label(cn, feat);

		    fn(cn->prefix, label, subf->number);
                }
            }
            
        }
    }
}

//------------------------------------------------------------------------------
SensorItem::SensorItem(const char *name, const char* chip, int feat) :
    InfoItemReal(name),
    chipName(chip),
    subFeature(feat)
{
}
    

//------------------------------------------------------------------------------
Sensors::Sensors() : InfoReader("sensors")
{
    sensors_init(NULL);

    each_sensor([this](const char* chip, const char* name, int subFeature) {
	    SensorItem *item = new SensorItem(name, chip, subFeature);
	    
	    infos.push_back(item);
	});
}

Sensors::~Sensors()
{
    sensors_cleanup();
}

void Sensors::read()
{
    sensors_chip_name const * cn;
    int c = 0;
    
    while ((cn = sensors_get_detected_chips(0, &c)) != 0) {
	for (auto item : infos) {
	    SensorItem *sensor = static_cast<SensorItem*>(item);

	    if (sensor->getChipName() == cn->prefix) {
		double val = 0.0;
		int err = sensors_get_value(cn, sensor->getFeature(), &val);
		if (err < 0) {
		    Log::err(__FUNCTION__, "sensor_get_value");
		}
		else {
		    Log::value(sensor->getName().c_str(), val);
		}
	    }
	}
    }
}

void Sensors::print()
{
}


