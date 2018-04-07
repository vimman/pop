/* 
 * File:   main.c
 * Author: bocal
 *
 * Created on April 8, 2018, 12:38 PM
 */

#define OUTPUT  0
#define INPUT   1

#define HIGH    1
#define LOW     0

#define true    1
#define false   0


#include <p32xxxx.h>


/*
 * 
 */
int main() {

    /*  ===[VARIABLES]===  */
    char button_pressed = false;
    /*  ===[/VARIABLES]===  */

    /*  ===[TIMER SETTINGS]===  */
    TMR2    = 0; /* Init the timer counter */
    PR2     = 31250; /* This is the value that will trigger the T2IF */
    T2CONbits.TCKPS = 4; /* 6 = 1:64 prescale value; 5 = 1:32; ... */
    T2CONbits.ON = true; /* Activate the timer */
    /*  ===[/TIMER SETTINGS]===  */

    /*  ===[IO PINS CONFIG]===  */
    TRISFbits.TRISF1 = OUTPUT; /* LED pin */
    TRISDbits.TRISD8 = INPUT; /* Button pin */
    /*  ===[/IO PINS CONFIG]===  */

    while (true) {
        if (IFS0bits.T2IF == HIGH) {
            IFS0bits.T2IF = LOW;
            LATFbits.LATF1 ^= 1;
        }
        if (!PORTDbits.RD8) {
            if (button_pressed) {
                if (T2CONbits.TCKPS == 0)
                    T2CONbits.TCKPS = 4;
                else
                    T2CONbits.TCKPS--;
                button_pressed = false;
            }
        } else {
            button_pressed = true;
        }  
    }
}

