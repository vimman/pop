// PIC32MX340F512H Configuration Bit Settings

// DEVCFG1
#pragma config FNOSC = PRI      // Oscillator Selection Bits (Primary)
#pragma config POSCMOD = HS     // Primary Oscillator Configuration (HS)
#pragma config FPBDIV = DIV_8   // Peripheral Clock Divisor (Sys_Clk/8)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include "type.h"
#include <p32xxxx.h>

int main()
{
	u16 i = 0;
	u16 delay = 32;

	_TRISD8 = 1;			// Put the button to in mode
	_TRISF1 = 0;			// Put the led in output mode
	T2CONbits.ON = 1;		// Put the timer on
	T2CONbits.TCKPS0 = 1;	// Timer 2 configuration bits, pre-scale value of 2
	TMR2 = 0;				// Set the timer's counter to zero
	PR2 = 15625;			// 8M / 8 / 64 (32 * PR2 = 500 000) Period Register 

	while (42)
	{
		if (IFS0bits.T2IF && ++i)	// If the Interrupt Flag is on
			IFS0bits.T2IF = 0;		// Reset the tick register
		if (i >= delay)				// When the timer ticks
		{
			i = 0;					// Reset i
			LATFbits.LATF1 ^= 1;	// Switch the light state
		}
		if (!PORTDbits.RD8)			// If the button is pushed
		{
			IFS0bits.T2IF = 0;		// reset the tick register
			if ((delay /= 2) < 2)	// If the delay is two small reset it
				delay = 32;
			while (!PORTDbits.RD8);	// Wait for the button to release
		}
	}
}
