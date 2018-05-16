#include "customPinout.h"
#include "PPI.h"

const int pinOut = PIN_SERIAL_RX;

// temporary simplification:
const int diodes[] = {diode_d_pins[1], diode_e_pins[1],
                      diode_d_pins[0], diode_e_pins[0]};

int captures[2][4] = {{0}}; // 2 timers, 4 channels

bool waitingForPrint = 0;

void setup() {
    Serial.setPins(0, PIN_SERIAL_TX); // RX is not used here
    Serial.begin(230400);

    pinMode(pinOut, OUTPUT);

    // start timers, they will be sync'ed (reset) in PPI
    nrf_timer_task_trigger(timers[1], NRF_TIMER_TASK_START);
    nrf_timer_task_trigger(timers[2], NRF_TIMER_TASK_START);
    setPPIresets();
}


void loop() {
    static bool old_state = 0;
    bool new_state = digitalRead(diodes[0]);

    if (!new_state && old_state == 1) {
        digitalWrite(pinOut, HIGH);
        digitalWrite(pinOut, LOW);
        if (waitingForPrint) {
            waitingForPrint = 0;
            printCallback();
        } else {
            digitalWrite(pinOut, HIGH);
            digitalWrite(pinOut, LOW);
            setPPIcaptures();
        }
    }

    old_state = new_state;
}


void setPPIresets() {
    PPI.resetChannels();

    PPI.setTimer(1);
    int forkTimer = 2;

    // sync timers using all photodiodes
    for (int i = 0; i < 4; i++) {
        PPI.setInputPin(diodes[i]);
        PPI.setShortcut(PIN_HIGH, TIMER_CLEAR, forkTimer);
    }
}


// Time stamp both rising and falling edges for the 4 photodiodes
// Timer 1: diode 0: channels: 0, 1 captures: 0, 1 (rising, falling edge)
//          diode 1: channels: 2, 3 captures: 2, 3 (rising, falling edge)
// Timer 2: diode 2: channels: 0, 1 captures: 4, 5 (rising, falling edge)
//          diode 3: channels: 2, 3 captures: 6, 7 (rising, falling edge)
void setPPIcaptures() {
    PPI.resetChannels();

    for (int i = 0; i < 4; i++) {

        PPI.setTimer(i/2 + 1);                      // timers 1 and 2

        PPI.setInputPin(diodes[i]);                 // diode 0 to 3
        PPI.setShortcut(PIN_HIGH, TIMER_CAPTURE);   // channel i*2
        PPI.setShortcut(PIN_LOW,  TIMER_CAPTURE);   // channel i*2 + 1
    }

    waitingForPrint = 1;
}


void printCallback() {
    // Timers 1 and 2, on 4 channels (0 to 3)
    for (int t = 1; t <= 2; t++) {
        for (int c = 0; c < 4; c++) {
            captures[t-1][c] = nrf_timer_cc_read(timers[t],
                                                 nrf_timer_cc_channel_t(c));
            Serial.println(captures[t-1][c]/16.); // convert to microseconds
        }
    }
    Serial.println();

    setPPIresets(); // prepare for next loop
}


