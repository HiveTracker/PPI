#include "customPinout.h"
#include "PPI.h"

#define E0 22

void setup() {
    Serial.setPins(PIN_SERIAL_RX, PIN_SERIAL_TX);
    Serial.begin(115200);
    Serial.println("Starting...");

    // measure pulse width
    PPI.setInputPin(E0);

    PPI.setShortcut(PIN_HIGH, TIMER_CLEAR);
    PPI.setShortcut(PIN_HIGH, TIMER_START);

    PPI.setShortcut(PIN_LOW, TIMER_CAPTURE); // capture register 0 for now
}


void loop() {
    // capture register 0
    int cc = nrf_timer_cc_read(NRF_TIMER1, nrf_timer_cc_channel_t(0));
    Serial.println(cc/16.); // convert to microseconds
}

