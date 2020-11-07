// https://randomnerdtutorials.com/esp32-pwm-arduino-ide/

#include <Arduino.h>

const int pinLed = 4;

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

void setup()
{
    ledcSetup(ledChannel, freq, resolution); // configure LED PWM functionalitites
    ledcAttachPin(pinLed, ledChannel);       // attach the channel to the GPIO to be controlled
}

void loop()
{
    // increase the LED brightness
    for (int dutyCycle = 0; dutyCycle <= 20; dutyCycle++)
    {
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        delay(50);
    }

    // decrease the LED brightness
    for (int dutyCycle = 20; dutyCycle >= 0; dutyCycle--)
    {
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        delay(50);
    }
}