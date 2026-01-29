#include <msp430.h>
#include "Serial.h"
#include "unified_clock_system.h"

static void setup1MHz(void)
{
  UCA1BRW = 9;            // 1048576 Hz for 115200 baud
  UCA1MCTL |= UCBRS0;     // Modulation (UCBRS0=0x01, UCOS16=0)
  UCA1CTL1 &= ~UCSWRST;   // Clear software reset to initialize USCI state machine
}

static void setup8MHz(void)
{
  UCA1BRW = 72;           // 115200 
  UCA1MCTL |= UCBRS_6;
  UCA1CTL1 &= ~UCSWRST;
}

static void setup16MHz(void)
{
  UCA1BRW = 145;
  UCA1MCTL |= UCBRS_5;
  UCA1CTL1 &= ~UCSWRST;
}

static void setup20MHz(void)
{
  UCA1BRW = 182;
  UCA1MCTL |= UCBRS_0;
  UCA1CTL1 &= ~UCSWRST;
}

void Serial0_setup(UCS_freq_e freq) 
// The Serial output from USCI_A1 is muxed with the JTAG interface over USB
{
  P4SEL |= BIT4 + BIT5;   // Set USCI_A1 RXD/TXD to receive/transmit data
  UCA1CTL1 |= UCSWRST;    // Set reset in order to modify control registers
  UCA1CTL0 &= 0x00;       // USCI_A1 control register
  UCA1CTL1 |= UCSSEL_2;   // Clock source SMCLK

  switch (freq) {
    case UCS_1MHZ:
    setup1MHz();
    break;

    case UCS_8MHZ:
    setup8MHz();
    break;

    case UCS_16MHZ:
    setup16MHz();
    break;

    case UCS_20MHZ:
    setup20MHz();

    default:
    break;
  }
}

void Serial0_write(char* targetPtr)
//currently does not use interrupts for filling the transmit buffer
{
    while(*targetPtr) {
        while(!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *targetPtr++;
    }
}

void Serial0_put(char targetByte)
{
  while(!(UCA1IFG & UCTXIFG));
  UCA1TXBUF = targetByte;
}
