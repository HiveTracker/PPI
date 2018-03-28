#include "customPinout.h"
#include "PPI.h"

bool state = 0;

void setup() {
    Serial.setPins(0, PIN_SERIAL_TX);
    Serial.begin(230400);
    Serial.println("Starting...");

    pinMode(PIN_SERIAL_RX, OUTPUT);

    // measure pulse width
    PPI.setInputPin(diode_e_pins[0]);

    PPI.setShortcut(PIN_HIGH, TIMER_CLEAR);
    PPI.setShortcut(PIN_HIGH, TIMER_START);

    PPI.setShortcut(PIN_LOW, TIMER_CAPTURE); // capture register 0 for now
    PPI.setShortcut(PIN_LOW, TIMER_STOP); // capture register 0 for now

    // interrupt
    attachInterrupt(diode_e_pins[0], callback, FALLING);
}


void callback() {
    // takes about 2.2us from interrupt till this digitalWrite
    digitalWrite(PIN_SERIAL_RX, state = !state);

    // capture register 0 (takes about 0.8us)
    int cc = nrf_timer_cc_read(NRF_TIMER1, nrf_timer_cc_channel_t(0));

    digitalWrite(PIN_SERIAL_RX, state = !state);

    Serial.println(cc/16.); // convert to microseconds
}


void loop() {}


// Ideal setting function:
void setPPIcaptures() {
    PPI.resetChannels();    // TODO?

    PPI.setTimer(1);

    PPI.setInputPin(diode_e_pins[0]);
    PPI.setShortcut(PIN_HIGH, TIMER_CAPTURE); // chanel 0
    PPI.setShortcut(PIN_LOW,  TIMER_CAPTURE); // chanel 1

    PPI.setInputPin(diode_e_pins[1]);
    PPI.setShortcut(PIN_HIGH, TIMER_CAPTURE); // chanel 2
    PPI.setShortcut(PIN_LOW,  TIMER_CAPTURE); // chanel 3

    PPI.setTimer(2);

    PPI.setInputPin(diode_e_pins[2]);
    PPI.setShortcut(PIN_HIGH, TIMER_CAPTURE); // chanel 0
    PPI.setShortcut(PIN_LOW,  TIMER_CAPTURE); // chanel 1

    PPI.setInputPin(diode_e_pins[3]);
    PPI.setShortcut(PIN_HIGH, TIMER_CAPTURE); // chanel 2
    PPI.setShortcut(PIN_LOW,  TIMER_CAPTURE); // chanel 3
}



// Ideal setting function:
void setPPIresets() {
    PPI.resetChannels();    // TODO?

    PPI.setTimer(1);

    PPI.setInputPin(diode_e_pins[0]);
    PPI.setShortcut(PIN_HIGH, TIMER_CLEAR); // chanel 0

    PPI.setInputPin(diode_e_pins[1]);
    PPI.setShortcut(PIN_HIGH, TIMER_CLEAR); // chanel 1

    PPI.setTimer(2);

    PPI.setInputPin(diode_e_pins[2]);
    PPI.setShortcut(PIN_HIGH, TIMER_CLEAR); // chanel 2

    PPI.setInputPin(diode_e_pins[3]);
    PPI.setShortcut(PIN_HIGH, TIMER_CLEAR); // chanel 3
}


