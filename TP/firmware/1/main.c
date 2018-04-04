#include "type.h"
#include <p32xxxx.h>

int main1()
{
	u8 sw = 1;

	TRISD = 1 << 8;			/* Set the button pin to input */
	TRISF &= ~(1 << 1);		/* Set the led pin to output */
	while (42)
	{
		if (!(PORTD & (1 << 8)) && sw)
		{
			sw = 0;
			LATF ^= 1 << 1;	/* Switch led state */
		}
		else if (PORTD & (1 << 8))
			sw = 1;
	}
}
