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
    setPPIcaptures();
}


void loop() {}


// Time stamp both rising and falling edges for the 4 photodiodes
// Timer 1: diode 0: channels: 0, 1 (rising, falling edge)
//          diode 1: channels: 2, 3 (rising, falling edge)
// Timer 2: diode 2: channels: 4, 5 (rising, falling edge)
//          diode 3: channels: 6, 7 (rising, falling edge)
void setPPIcaptures() {
    PPI.resetChannels();    // TODO?

    for (int i = 0; i < 4; i++) {
        PPI.setTimer(i/2 + 1);                      // timers 1 and 2

        PPI.setInputPin(diode_e_pins[i]);           // diode 0 to 3
        PPI.setShortcut(PIN_HIGH, TIMER_CAPTURE);   // channel i*2
        PPI.setShortcut(PIN_LOW,  TIMER_CAPTURE);   // channel i*2 + 1
    }
}



// Clear both timers in case of rising edge for the 4 photodiodes
// This needs to be disabled as soon as any edge is detected
// TODO: try using TASKS_CHG[i].DIS with FORK.TEP[i]
void setPPIclears() {
    PPI.resetChannels();    // TODO?

    // timers 1 & 2, on 4 channels
    for (int timerNo = 1; timerNo <= 2; timerNo++) {
        PPI.setTimer(timerNo);

        for (int i = 0; i < 4; i++) {
            PPI.setInputPin(diode_e_pins[i]);
            PPI.setShortcut(PIN_HIGH, TIMER_CLEAR); // channel 0 to 3
        }
    }
}

