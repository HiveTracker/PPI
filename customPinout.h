// This file adapts the pinout for the HiveTracker V1.0

#ifndef _PINOUT_
#define _PINOUT_

// LEDs
#define LED_R                (10)
#define LED_G                (14)
#define LED_B                (12)
#define PIN_LED              LED_G
#define LED_BUILTIN          PIN_LED
#define BUILTIN_LED          PIN_LED

// Button
#define BUTTON               (26)

// Serial
#define PIN_SERIAL_RX        (25) /* J4 - E */
#define PIN_SERIAL_TX        (27) /* J4 - D */

// I2C
#define PIN_WIRE_SDA         (29)
#define PIN_WIRE_SCL         (28)

#endif // _PINOUT_

