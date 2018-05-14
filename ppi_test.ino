#include "customPinout.h"
#include "PPI.h"

const int pinOut = PIN_SERIAL_RX;

// temporary simplification:
const int diodes[] = {diode_d_pins[1], diode_e_pins[1],
                      diode_d_pins[0], diode_e_pins[0]};

int captures[2][4] = {{0}}; // 2 timers, 4 channels


void setup() {
    Serial.setPins(0, PIN_SERIAL_TX); // RX is not used here
    Serial.begin(230400);

    pinMode(pinOut, OUTPUT);

    attachInterrupt(diodes[0], setPPIstarts, FALLING);
}


void loop() {
    static bool old_state = 0;

    if (digitalRead(diodes[0]) && old_state == 0) {
        NRF_TIMER1->TASKS_CAPTURE[0] = 1;

    digitalWrite(pinOut, HIGH);
    digitalWrite(pinOut, LOW);
    digitalWrite(pinOut, HIGH);
    digitalWrite(pinOut, LOW);
    digitalWrite(pinOut, HIGH);
    digitalWrite(pinOut, LOW);

        uint32_t val = NRF_TIMER2->CC[0];

        Serial.print("^ ");
        Serial.println(val/16.); // convert to microsec
    }
    old_state = digitalRead(diodes[0]);
}


void setPPIstarts() {
    digitalWrite(pinOut, HIGH);
    digitalWrite(pinOut, LOW);

    Serial.println("* 1 setPPIstarts");

    PPI.resetChannels();    // TODO?

    PPI.setInputPin(diodes[0]);
    PPI.setTimer(1);
    PPI.setShortcut(PIN_HIGH, TIMER_START);
    PPI.setShortcut(PIN_HIGH, TIMER_CLEAR);
    PPI.setTimer(2);
    PPI.setShortcut(PIN_HIGH, TIMER_START);
    PPI.setShortcut(PIN_HIGH, TIMER_CLEAR);

    attachInterrupt(diodes[0], setPPIcaptures, FALLING);
}


// Time stamp both rising and falling edges for the 4 photodiodes
// Timer 1: diode 0: channels: 0, 1 captures: 0, 1 (rising, falling edge)
//          diode 1: channels: 2, 3 captures: 2, 3 (rising, falling edge)
// Timer 2: diode 2: channels: 0, 1 captures: 4, 5 (rising, falling edge)
//          diode 3: channels: 2, 3 captures: 6, 7 (rising, falling edge)
void setPPIcaptures() {
    Serial.println("$ 2 setPPIcaptures");

    digitalWrite(pinOut, HIGH);
    digitalWrite(pinOut, LOW);
    digitalWrite(pinOut, HIGH);
    digitalWrite(pinOut, LOW);

    PPI.resetChannels();    // TODO?

    for (int i = 0; i < 4; i++) {

            char buf[128];
            sprintf(buf, "   tdcc: %d, %d - %d, %d\n", i/2+1, i, i*2, i*2+1);
            Serial.print(buf);

        PPI.setTimer(i/2 + 1);                      // timers 1 and 2

        PPI.setInputPin(diodes[i]);                 // diode 0 to 3
        PPI.setShortcut(PIN_HIGH, TIMER_CAPTURE);   // channel i*2
        PPI.setShortcut(PIN_LOW,  TIMER_CAPTURE);   // channel i*2 + 1
    }

    attachInterrupt(diodes[0], printCallback, FALLING);
}


void printCallback() {
    Serial.println("# 3 printCallback");

    // Timers 1 and 2, on 4 channels (0 to 3)
    for (int t = 1; t <= 2; t++) {
        for (int c = 0; c < 4; c++) {
            captures[t-1][c] = nrf_timer_cc_read(timers[t],
                                                 nrf_timer_cc_channel_t(c));

            char buf[128];
            sprintf(buf, "tcc: %d, %d ", t, c);
            Serial.print(buf);
            Serial.println(captures[t-1][c]/16.); // convert to microseconds
        }
    }
    Serial.println();

    setPPIstarts(); // prepare for next loop
}

/*
// Clear both timers in case of rising edge for the 4 photodiodes
// This needs to be disabled as soon as any edge is detected
// TODO: try using TASKS_CHG[i].DIS with FORK.TEP[i]
void setPPIclears() {
    PPI.resetChannels();    // TODO?

    // Timers 1 and 2, on 4 channels (0 to 3)
    for (int t = 1; t <= 2; t++) {
        PPI.setTimer(t);

        for (int c = 0; c < 4; c++) {
            PPI.setInputPin(diodes[c]);
            PPI.setShortcut(PIN_HIGH, TIMER_CLEAR); // channel 0 to 3
        }
    }
}
*/
