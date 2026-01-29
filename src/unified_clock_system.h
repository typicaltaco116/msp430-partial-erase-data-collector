#pragma once
#include <msp430.h>

#define SMCLK_1MHZ (1048576.0)
#define SMCLK_8MHZ (8 * 1048576.0)
#define SMCLK_16MHZ (16 * 1048576.0)
#define SMCLK_20MHZ (20 * 1048576.0)

typedef enum{
  UCS_1MHZ,
  UCS_8MHZ,
  UCS_16MHZ,
  UCS_20MHZ,
} UCS_freq_e;

void UCS_setMCLKFreq(UCS_freq_e freq);

UCS_freq_e UCS_getMCLKFreq(void);
