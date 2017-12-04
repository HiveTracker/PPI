#include "PPI.h"

void setup() {

    //bind a timer event to the toggle of the pin
    //set an interval for the timer (in milliseconds)
    PPI.setTimerInterval(500);
    PPI.setOutputPin(LED_G); //select the output pin
    PPI.setShortcut(TIMER, PIN_TOGGLE); //bind TIMER event to PIN_TOGGLE action

    PPI.setOutputPin(LED_B); //select the output pin
    PPI.setShortcut(TIMER, PIN_TOGGLE); //bind TIMER event to PIN_TOGGLE action

}


void loop() {
    delay(1000);
}
