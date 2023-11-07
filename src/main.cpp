/*
 *      Project Motor_control
 *      Using a switch to control a motor
 *
 *      main.cpp
 */

#include <Arduino.h>

/*
 *     Definiciones
 */
#define MOTOR           23
#define SWITCH          22

/*
 *  Public functions
 */


void
setup(void)
{
    Serial.begin(SERIAL_BAUD);
    pinMode(MOTOR, OUTPUT);
    pinMode(SWITCH, INPUT_PULLUP);
}

void
loop(void)
{
    int digital_value = digitalRead(SWITCH);
    Serial.println(digital_value);
    if(digital_value == HIGH)
        digitalWrite(MOTOR, HIGH);
    else
        digitalWrite(MOTOR, LOW);
}
