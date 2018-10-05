#pragma once

#include <stdint.h>

/** Error codes
 */
typedef enum errors {
    ERR_MY     = 0x01,
} error_t;

extern void _error(error_t code);
