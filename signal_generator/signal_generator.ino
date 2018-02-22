const int testPin = 0;

/* Timings in microseconds
    ___             ___
___|   |___________|
   0  100         400

*/

void setup()
{
    pinMode(testPin, OUTPUT);
}


void loop()
{
    digitalWrite(testPin, HIGH);
    delayMicroseconds(100);

    digitalWrite(testPin, LOW);
    delayMicroseconds(300);
}

