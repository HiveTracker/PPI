#include "PPI.h"

// This test copies to state of an input pin in an output...
// ...and measures the widths of the pulses of the input.
// To simplify the test, it uses Serial RX as the input.

#define clone_pin 22

void setup() {
    Serial.begin(115200);

    // copy input in output (opposite)
    PPI.setInputPin(PIN_SERIAL_RX);
    PPI.setOutputPin(clone_pin);
    PPI.setShortcut(PIN_CHANGE, PIN_TOGGLE);

    // measure pulse width
    PPI.setInputPin(PIN_SERIAL_RX);

    PPI.setShortcut(PIN_LOW, TIMER_CLEAR);
    PPI.setShortcut(PIN_LOW, TIMER_START);

    PPI.setShortcut(PIN_HIGH, TIMER_CAPTURE); // capture register 0 for now
    PPI.setShortcut(PIN_HIGH, TIMER_STOP);
}


void loop() {
    // capture register 0
    int cc = nrf_timer_cc_read(NRF_TIMER1, nrf_timer_cc_channel_t(0));

    static int cc_bak = 0;
    if (cc_bak != cc) {         // check if a new value came
        Serial.println(cc/16.); // convert to microseconds
        cc_bak = cc;
    }
}

