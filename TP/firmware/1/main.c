#include "type.h"
#include <p32xxxx.h>

int main()
{
    TRISD = 1 << 8;						// Set the button pin to input
    TRISF &= ~(1 << 1);					// Set the led pin to output

    while (42)
    {
        while (PORTD & (1 << 8));		// While not pushed
        LATF ^= 1 << 1;					// Switch led state
        while (!(PORTD & (1 << 8)));	// While pushed
    }
}
