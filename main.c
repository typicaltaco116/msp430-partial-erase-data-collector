/***************************************************************
* FILENAME: main.c
* AUTHOR: Jack Pyburn
* DATE: 01/14/2026
* STATUS: 
*        
* DESCRIPTION:
* Reports the number of bit errors after various partial erase
* operations ranging from 0.05ms to 1.0ms
****************************************************************/
#include <msp430.h> 
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "src/flash_operations.h"
#include "src/flash_statistics.h"
#include "src/Serial.h"
#include "src/unified_clock_system.h"
#include "src/event_timer.h"
#include "src/flash_partial_operations.h"


/* FLASH BANK D starts at 0x1_C400 and ends at 0x2_43FF */
#define F5529_FLASH_BANK_D      0x1C400
#define CHIP_ID_ADR             0x1A0A

#define ENDL "\r\n"
#define BUF_SIZE                64

static void haltUntilInput(void);
static uint64_t get_chipID(void);
static void doRoutineStatisticsCSV(f_bank_t, char*);


int main(void)
{
  char outputBuffer[BUF_SIZE];
  f_bank_t bank_D = (f_bank_t)F5529_FLASH_BANK_D;

  WDTCTL = WDTPW + WDTHOLD;	// stop watchdog timer

  // CONSIDER REMOVAL
  haltUntilInput(); // holds program until user presses KEY1

  UCS_setMCLKFreq(UCS_8MHZ);

  Serial0_setup(UCS_8MHZ);

  /* PRINT HEADER */
  Serial0_write("----------------------------------------------------------------------------------------------------" ENDL);
  Serial0_write("- Partial Erase Bit Error Rate Benchmark (FINE)" ENDL);
  snprintf(outputBuffer, BUF_SIZE, "- Subject Chip ID: 0x%08llX" ENDL, get_chipID());
  Serial0_write(outputBuffer);
  Serial0_write("----------------------------------------------------------------------------------------------------" ENDL);

  // RESET ENTIRE BANK
  f_bankStress(bank_D, 0x0000, 1);

  // PERFORM STATISTICS
  doRoutineStatisticsCSV(bank_D, outputBuffer);

  Serial0_write("----------------------------------------------------------------------------------------------------" ENDL);

  Serial0_write(" DONE..." ENDL ENDL);

  return 0;

}

#define TEST_SIZE 20

static void doRoutineStatisticsCSV(f_bank_t bankPtr, char* charBuffer)
{
  f_segment_t seg;
  fs_stats_s stats;

  uint16_t testValues[20] = {
    (uint16_t)( 10.0000E-6 * SMCLK_8MHZ / 1.0 ), //  83
    (uint16_t)( 25.2632E-6 * SMCLK_8MHZ / 1.0 ), //  211
    (uint16_t)( 40.5263E-6 * SMCLK_8MHZ / 1.0 ), //  339
    (uint16_t)( 55.7895E-6 * SMCLK_8MHZ / 1.0 ), //  467
    (uint16_t)( 71.0526E-6 * SMCLK_8MHZ / 1.0 ), //  596
    (uint16_t)( 86.3158E-6 * SMCLK_8MHZ / 1.0 ), //  724
    (uint16_t)( 101.5789E-6 * SMCLK_8MHZ / 1.0 ), //  852
    (uint16_t)( 116.8421E-6 * SMCLK_8MHZ / 1.0 ), //  980
    (uint16_t)( 132.1053E-6 * SMCLK_8MHZ / 1.0 ), //  1108
    (uint16_t)( 147.3684E-6 * SMCLK_8MHZ / 1.0 ), //  1236
    (uint16_t)( 162.6316E-6 * SMCLK_8MHZ / 1.0 ), //  1364
    (uint16_t)( 177.8947E-6 * SMCLK_8MHZ / 1.0 ), //  1492
    (uint16_t)( 193.1579E-6 * SMCLK_8MHZ / 1.0 ), //  1620
    (uint16_t)( 208.4211E-6 * SMCLK_8MHZ / 1.0 ), //  1748
    (uint16_t)( 223.6842E-6 * SMCLK_8MHZ / 1.0 ), //  1876
    (uint16_t)( 238.9474E-6 * SMCLK_8MHZ / 1.0 ), //  2004
    (uint16_t)( 254.2105E-6 * SMCLK_8MHZ / 1.0 ), //  2132
    (uint16_t)( 269.4737E-6 * SMCLK_8MHZ / 1.0 ), //  2260
    (uint16_t)( 284.7368E-6 * SMCLK_8MHZ / 1.0 ), //  2388
    (uint16_t)( 300.0000E-6 * SMCLK_8MHZ / 1.0 ), //  2516
  };

  float testValuesFloat[20] = {
    ( 9.8944 ),
    ( 25.1532 ),
    ( 40.4119 ),
    ( 55.6707 ),
    ( 71.0487 ),
    ( 86.3075 ),
    ( 101.5663 ),
    ( 116.8251 ),
    ( 132.0839 ),
    ( 147.3427 ),
    ( 162.6015 ),
    ( 177.8603 ),
    ( 193.1190 ),
    ( 208.3778 ),
    ( 223.6366 ),
    ( 238.8954 ),
    ( 254.1542 ),
    ( 269.4130 ),
    ( 284.6718 ),
    ( 299.9306 ),
  };

  // SET POINTER BACK TO BASE SEGMENT
  seg = (f_segment_t)bankPtr; 

  Serial0_write("----------------------------------------------------------------------------------------------------" ENDL);
  Serial0_write("  Segment Bit Errors (CSV)" ENDL);
  Serial0_write("----------------------------------------------------------------------------------------------------" ENDL);
  Serial0_write("Partial Erase Times (us),");

  for (uint16_t i = 0; i < 20; ++i) {
    snprintf(charBuffer, BUF_SIZE, "%6.4f, ", testValuesFloat[i]);
    Serial0_write(charBuffer);
  }
  Serial0_write(ENDL);

  // STATS FOR EACH SEGMENT IN BANK
  for (uint16_t s = 0 ; s < F_BANK_N_SEGMENTS; s++) {

    // OUTPUT SEGMENT INDEX
    snprintf(charBuffer, BUF_SIZE, "%02u (0x%p),", s, seg);
    Serial0_write(charBuffer);

    // CHECK PARTIAL ERASE OPERATIONS
    for (uint16_t i = 0; i < TEST_SIZE; ++i) { // loop through all partial erases
      __delay_cycles(SMCLK_8MHZ);
      f_segmentErase(seg);
      f_segmentWriteRAM(0x0000, (uint16_t*)seg);
      f_segmentPartialErase(seg, testValues[i]);
      fs_checkBits(seg, &stats, 0xFFFF);

      snprintf(charBuffer, BUF_SIZE, " %4u,", stats.incorrect_bit_count);
      Serial0_write(charBuffer); // only recording incorrect bits!
    }

    Serial0_write(ENDL);

    seg++;
  }

}

static void haltUntilInput(void)
{
  P1REN |= BIT1;
  P1DIR |= BIT0;
  P1OUT |= BIT1 + BIT0;
  while(P1IN & BIT1);// wait till button press to start
  P1OUT &= ~BIT0;
}

static uint64_t get_chipID(void)
{
  return *(uint64_t*)CHIP_ID_ADR;
}
