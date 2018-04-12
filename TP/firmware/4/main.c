#include "type.h"
#include <p32xxxx.h>		// contains device specific defines
#include <sys/attribs.h>	// contains __ISR() Macros

void init(void);
void switchMode(u8 newmode);

u8	mode;
u8	duty = 0;			// Duty cycle
u8	pwm_period = 0;		// index of the pwm period
u8	pwm_direction;		// Step change direction
u8	speed= 7;			// Set the speed to 0.5Hz
u32	waiting = 0;		// 2 seconde waiting variable

void    __ISR (_EXTERNAL_1_VECTOR, IPL5SRS) buttonInterrupt(void)
{
	IFS0bits.INT1IF = 0;	// External Interrupt flag reset
	if (mode == BLINK)
	{
		if (INTCONbits.INT1EP == FALLING_EDGE)	// When pushing the button
		{
			if (T2CONbits.TCKPS == 3)		// If prescaler is 8
				T2CONbits.TCKPS = 7;		// Reset to 256
			else
				T2CONbits.TCKPS--;			// Divide the prescaler by 2
			TMR3 = 0;						// Clear counter
			T3CONbits.TON = 1;				// Enable Timer 3
		}
		else
			T3CONbits.TON = 0;				// Disable Timer 3
	}
	waiting = 0;							// Reseting the 2sec time variable
	INTCONbits.INT1EP ^= 1;					// Switching the falling/rising edge
}

void    __ISR (_TIMER_2_VECTOR, IPL6SRS) mainTimerInterrupt(void)
{

	IFS0bits.T2IF = 0;					// Reset interrupt flag
	if (mode == DIM)
		LATFbits.LATF1 = (pwm_period++ < duty); // Increment pwm_period and check if it reach the duty cycle
	else
		LATFbits.LATF1 ^= 1;			// Toggle LED LD1
}

void __ISR (_TIMER_3_VECTOR, IPL7SRS) auxTimerInterrupt(void)
{
	IFS0bits.T3IF = 0;				// Reset interrupt flag
	if (mode == BLINK)				// If mode is blink
		switchMode(DIM);			// Switch to dim mode
	else
	{
		waiting++;
		if (duty == 255)			// Once the led is full on
			pwm_direction = -1;		// Switch to the other direction
		if (duty == 0)				// If the led is completly off
			pwm_direction = 1;		// Switch the direction
		duty += pwm_direction;		// Increment the duty cycle
		if (PORTDbits.RD8 == LOW)	// If the button is pushed
		{
			if (waiting == 204)			// (255*2)/2.5
			{
				speed += 2;				// Increment the prescaler to slow the blinking
				if (speed > 7)			// If the prescaler is the slowest
					speed = 7;			// reset to 
				switchMode(BLINK);		// Switch to blink mode
			}
		}
		else
		{
			waiting = 0;
		}
	}
}

void switchMode(u8 newmode)
{
	mode = newmode;
	if (mode == BLINK)
	{
		/* Main Timer Configuration */

		T2CONbits.TCKPS = speed;	// Set pre-scale to speed ( from 256 to 16 )
		PR2 = 31250;				// 8M / 256 = 31250 for 1 second
		TMR2 = 0;					// Clear counter
		T2CONbits.TON = 1;			// Enable Timer 2

		/* Aux Timer Configuration */

		T3CONbits.TCKPS = 7;		// Set pre-scale to 256
		PR3 = 62500;				// 8M / 256 * 2 for 2 seconds
		TMR3 = 0;					// Clear counter
		T3CONbits.TON = 0;			// Disable Timer 3
	}
	else if (mode == DIM)
	{
		speed = T2CONbits.TCKPS;	// Remember the blink speed

		/* Main Timer Configuration */

		T2CONbits.TCKPS = 5;		// Set pre-scale to 32
		PR2 = 1;					// Set Timer 2 period
		TMR2 = 0;					// Clear counter
		T2CONbits.TON = 1;			// Enable Timer 2

		/* Aux Timer Configuration */

		T3CONbits.TCKPS = 7;		// Set pre-scale to 256
		PR3 = 305;					// (8M / 256) * (2.5 / 256)
		TMR3 = 0;					// Clear counter
		T3CONbits.TON = 1;			// Enable Timer 3
	}
}

void init()
{
	__builtin_disable_interrupts();		// Globally disable interrupts

	/* Input / Output Configuration */

	TRISFbits.TRISF1 = OUTPUT;			// Set LED to output
	LATFbits.LATF1   = LOW;				// Set LED OFF
	TRISDbits.TRISD8 = INPUT;			// Set Button to input

	/* Main Timer Interrupt Configuration */

	IPC2bits.T2IP = 6;					// Interrupt Priority
	IFS0bits.T2IF = 0;					// Timer 2 interrupt flag reset
	IEC0bits.T2IE = 1;					// Interrupt Enable for timer 2

	/* Aux Timer Interrupt Configuration */

	IPC3bits.T3IP = 7;					// Interrupt Priority
	IFS0bits.T3IF = 0;					// Timer 3 interrupt flag reset
	IEC0bits.T3IE = 1;					// Interrupt Enable for timer 3

	/* Button Interrupt Configuration */

	IPC1bits.INT1IP     = 5;			// Button Interrupt Priority 
	INTCONbits.INT1EP   = FALLING_EDGE;	// Set Ext. Interrupt Polarity to Falling Edge 
	IFS0bits.INT1IF     = 0;			// External Interrupt flag reset 
	IEC0bits.INT1IE     = 1;			// Interrupt Enable for Ext. Interrupt 1 

	/* Watchdog Configuration */

	WDTCONbits.ON;						// Start Watchdog Timer

	/* Set Interrupt Controller for multi-vector mode */

	INTCONSET = _INTCON_MVEC_MASK;

	__builtin_enable_interrupts();  /* Globally enable interrupts */
}

int main()
{
	switchMode(BLINK);			// Set the blink mode first
	init();						// Initialise everything
	while (true)
		WDTCONbits.WDTCLR = 1;	// Clear the watchdog
}
