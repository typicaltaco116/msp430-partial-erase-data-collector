#pragma once
#include <msp430.h>
#include "unified_clock_system.h"

void Serial0_setup(UCS_freq_e freq);

void Serial0_write(char* targetPtr);

void Serial0_put(char targetByte);
