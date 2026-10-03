#pragma once
// Test double only: no GPIO, serial port, board, or real sensor is opened.
class Ultrasonic {
public:
    Ultrasonic(int trigger, int echo) : trigger_pin(trigger), echo_pin(echo) {}
    int read() { ++read_count; return reading; }
    int trigger_pin, echo_pin, reading = 0, read_count = 0;
};
