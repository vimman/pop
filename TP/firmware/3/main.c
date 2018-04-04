
/* PIC32MX340F512H Configuration Bit Settings

/* DEVCFG1 */
#pragma config FNOSC = SOSC		/* Oscillator (Low Power Secondary Osc) */
#pragma config FSOSCEN = ON		/* Secondary Oscillator Enable (Enabled) */
#pragma config FPBDIV = DIV_1	/* Peripheral Clock Divisor (Sys_Clk/1) */
#pragma config FWDTEN = ON		/* Watchdog ON */
#pragma config WDTPS = PS128	/* Watchdog Timer Postscaler (1:128) */

/* #pragma config statements should precede project file includes */
/* Use project enums instead of #define for ON and OFF */

#include "type.h"			/* contains s8 s16 s32 u8 u16 u32 */
#include <p32xxxx.h>		/* contains device specific defines */
#include <sys/attribs.h>	/* contains __ISR() Macros */

#define BASE_PR 32768		/* 32768 cycles of 32.768 kHz = 1 second = 0.5Hz */

void	Init(void);			/* Initialize hardware and global variables */

void	__ISR (_EXTERNAL_1_VECTOR, IPL7SRS) ButtonInterrupt(void)
{
	IFS0bits.INT1IF = 0;	/* External Interrupt flag reset */
	TMR2 = 0;				/* Reset TMR2 to avoid overflow */
	if ((PR2 /= 2) < 2048)	/* If the delay is two small reset it */
		PR2 = BASE_PR;
}

void	__ISR (_TIMER_2_VECTOR, IPL6SRS) T2Interrupt(void)
{
	LATFbits.LATF1 ^= 1;	/* Toggle LED LD1 */
	IFS0bits.T2IF = 0;		/* Reset interrupt flag */
}

void	Init()
{
	__builtin_disable_interrupts(); /* Globally disable interrupts */

	/* Input / Output Configuration */
	TRISFbits.TRISF1 = 0;	/* Set LED to output */
	LATFbits.LATF1 = 0;		/* Set LED OFF */
	TRISDbits.TRISD8 = 1;	/* Set Button to input */

	/* Timer Configuration */
	T2CONbits.TCKPS = 0;	/* Set pre-scale to 1 */
	PR2 = BASE_PR;			/* Set Timer 2 period */
	TMR2 = 0;				/* Clear counter */
	T2CONbits.TON = 1;		/* Enable Timer 2 */

	/* Timer Interrupt Configuration */
	IPC2bits.T2IP = 6;		/* Interrupt Priority */
	IFS0bits.T2IF = 0;		/* Timer 2 interrupt flag reset */
	IEC0bits.T2IE = 1;		/* Interrupt Enable for timer 2 */

	/* Button Interrupt Configuration */
	IPC1bits.INT1IP = 7;	/* Button Interrupt Priority */
	INTCONbits.INT1EP = 0;	/* Set Ext. Interrupt Polarity to Falling Edge */
	IFS0bits.INT1IF = 0;	/* External Interrupt flag reset */
	IEC0bits.INT1IE = 1;	/* Interrupt Enable for Ext. Interrupt 1 */

	/* Watchdog Configuration */
	WDTCONbits.ON;			/* Start Watchdog Timer */

	/* Set Interrupt Controller for multi-vector mode */
	INTCONSET = _INTCON_MVEC_MASK;

	__builtin_enable_interrupts(); /* Globally enable interrupts */
}

int		main()
{
	Init();
	while (42)
		WDTCONbits.WDTCLR = 1;
}
