#include <gtest/gtest.h>
#include "../config.h"
#include "../logger.h"

TEST(cfg, t01)
{
    Log::msg("test", Cfg::get(CFG_SERIAL_PORT));
}
