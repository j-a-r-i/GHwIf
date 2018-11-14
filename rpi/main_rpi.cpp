#include "../common.h"
#include "main_rpi.h"
extern "C" {
  #include "hw.h"
}

#define SPI_PORT 1

cell *scm_io_set(scheme *sch, cell *args)
{
    try {
	int pin   = arg_integer(sch, args);

	io_set((pin_t)pin);
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return sch->NIL;
}

cell *scm_io_clear(scheme *sch, cell *args)
{
    try {
	int pin   = arg_integer(sch, args);

	io_clear((pin_t)pin);
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return sch->NIL;
}

cell *scm_io_read(scheme *sch, cell *args)
{
    try {
	int pin   = arg_integer(sch, args);
	int value = io_read((pin_t)pin);

	if (value)
	    return sch->T;
	else
	    return sch->F;
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return sch->NIL;
}



cell *scm_spi(scheme *sch, cell *args)
{
    try {
	int data   = arg_integer(sch, args);

	spi_write(SPI_PORT, data);
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return sch->NIL;
}

cell *scm_delay(scheme *sch, cell *args)
{
    try {
	int us  = arg_integer(sch, args);

	delay_us(us);
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return sch->NIL;
}

void rpi_init(BaseRuntime *rt)
{
    Log::value("HW", "RPI");

    io_init();

    spi_init(SPI_PORT);

    rt->addFunc("io-set",   scm_io_set);
    rt->addFunc("io-clear", scm_io_clear);
    rt->addFunc("io-read",  scm_io_read);
    rt->addFunc("spi",      scm_spi);
    rt->addFunc("delay",    scm_delay);
}
