const int testPin = 0;

/* Timings in microseconds
    ___                ___
___|   |_____...______|
   0   100            10000

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
    delay(10);
}

