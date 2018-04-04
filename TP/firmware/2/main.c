/* PIC32MX340F512H Configuration Bit Settings */

/* DEVCFG1 */
#pragma config FNOSC = PRI      /* Oscillator Selection Bits (Primary) */
#pragma config POSCMOD = HS     /* Primary Oscillator Configuration (HS) */
#pragma config FPBDIV = DIV_8   /* Peripheral Clock Divisor (Sys_Clk/8) */

/* #pragma config statements should precede project file includes. */
/* Use project enums instead of #define for ON and OFF. */

#include "type.h"
#include <p32xxxx.h>

int main()
{
	u8    sw;    
	u16 i;
	u16 delay;
	u16 PR_base;
	_TRISD8 = 1;			/* Put the button to in mode */
	_TRISF1 = 0;			/* Put the led in output mode */
	_TRISB0 = 0;			/* Debug pin */

	T2CONbits.ON = 1;		/* Put the timer on */
	TMR2 = 0;				/* Set the timer's counter to zero */
	PR_base = 15625;		/* 8M / 8 / 64 (32 * PR_base = 500 000) */
	PR2 = PR_base;			/* Set the Period Register to this magic number */

	T2CONbits.TCKPS0 = 1;

	sw = 1;
	i = 0;
	delay = 32;
	while (42)
	{
		if (IFS0bits.T2IF && ++i)
			IFS0bits.T2IF = 0;		/* reset the tick register */
		if (i >= delay)				/* When the timer ticks */
		{
			i = 0;
			LATFbits.LATF1 ^= 1;	/* Switch the light state */
			LATBbits.LATB0 ^= 1;
		}
		if (!PORTDbits.RD8 && sw)	/* If the button is pushed */
		{
			IFS0bits.T2IF = 0;		/* reset the tick register */
			if ((delay /= 2) < 2)	/* If the delay is two small reset it */
				delay = 32;
			sw = 0;
		}
		else if (PORTDbits.RD8)		/* If the button is released */
			sw = 1;
	}
}
