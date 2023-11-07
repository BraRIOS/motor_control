#   Project Motor_Control

##  Using a switch intended to control mechanically a motor to half turn, and an input from a Websocket to control how many times the motor turns.

    Electronica Digital - Austral - 2023 - Grupo 1_3
    Uses ESP32 microcontroller

###  Hardware

    10k ohm resistor
    220 ohm resistor
    3-6V DC motor with gearbox
    switch
    2N2222 transistor or TIP31C
    1N4001 diode (flyback diode, optional but recommended)
    breadboard
    jumper wires

###  Connections

    Connect 10k resistor ends between GND and GPIO "SWITCH" (see platformio.ini)
    Connect switch ends between 5V and GPIO "SWITCH" (see platformio.ini)

    Connect 220 ohm resistor between GPIO "MOTOR" (see platformio.ini) and base of transistor
    Connect motor between 5V and collector of transistor
    Connect anode of flyback diode to collector of transistor and cathode to 5V
    Connect emitter of transistor to GND

###  Verification

    Run `pio run -t upload` to upload the program to the ESP32
    Run `pio device monitor` to see the output from the ESP32

###  Links

[ESP32 WebSocket Server: Control Outputs (ESP32/ESP8266 + Arduino IDE)](https://randomnerdtutorials.com/esp32-websocket-server-arduino/)

[Control a DC Motor with Arduino, ESP8266 or ESP32 without IC](https://diyi0t.com/control-dc-motor-without-ic-motor-driver/)