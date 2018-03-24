#include "type.h"
#include <p32xxxx.h>

int	main()
{
	u16 i;
	u16 delay;
	i = 0;
	delay = 2048;
    TRISDbits.TRISD8 = 1;	// Put the button to in mode
    TRISFbits.TRISF1 = 0;	// Put the led in output mode
    TRISBbits.TRISB0 = 0;
    T2CONbits.ON = 1;		// Put the timer on
    TMR2 = 0;				// Set the timer's counter to zero
    PR2 = 243;				// Set the Period Register to this magic number

    while (42)
    {
        if (IFS0bits.T2IF && ++i)	// When the timer ticks increment i
            IFS0bits.T2IF = 0;		// and reset the tick register
        if (i >= delay)
        {
            i = 0;
            LATFbits.LATF1 ^= 1;	// Switch the light state
			LATBbits.LATB0 ^= 1;
        }
        if (!PORTDbits.RD8)			// If the button is pushed
        {
            if ((delay /= 2) <= 64)	// If the delay is two small reset it
                delay = 2048;
            while (!PORTDbits.RD8);	// Wait for the button to release
        }
    }
}

int main1()
{
    TRISD = 1 << 8;		// Set the button pin to input
    TRISF &= ~(1 << 1);	// Set the led pin to output

    while (42)
    {
        while (PORTD & (1 << 8));		// While not pushed
        LATF ^= 1 << 1;					// Switch led state
        while (!(PORTD & (1 << 8)));	// While pushed
    }
}
