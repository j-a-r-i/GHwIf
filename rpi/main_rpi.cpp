#include "../common.h"
#include "main_rpi.h"
extern "C" {
  #include "hw.h"
}

#define SPI_PORT 1

pointer scm_io_set(scheme *sch, pointer args)
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

pointer scm_io_clear(scheme *sch, pointer args)
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

pointer scm_spi(scheme *sch, pointer args)
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

pointer scm_delay(scheme *sch, pointer args)
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
    rt->addFunc("spi",      scm_spi);
    rt->addFunc("delay",    scm_delay);
}
