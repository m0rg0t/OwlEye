#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

constexpr int OUTPUT = 1, LOW = 0, HIGH = 1;
struct Event {
    std::string name;
    int pin;
    int value;
    bool operator==(const Event& other) const {
        return name == other.name && pin == other.pin && value == other.value;
    }
};
std::vector<Event> events;
std::uint32_t fake_clock = 0;
void pinMode(int pin, int mode) { events.push_back({"mode", pin, mode}); }
void digitalWrite(int pin, int value) { events.push_back({"write", pin, value}); }
void tone(int pin, unsigned int frequency) {
    events.push_back({"tone", pin, static_cast<int>(frequency)});
}
void noTone(int pin) { events.push_back({"silence", pin, 0}); }
void delay(unsigned long ms) {
    events.push_back({"delay", -1, static_cast<int>(ms)});
    fake_clock += static_cast<std::uint32_t>(ms);
}
struct SerialDouble {
    void begin(int baud) { events.push_back({"serial", -1, baud}); }
    void print(const char*) {}
    void println(const char*) {}
    void println(int) {}
} Serial;

// Compile the actual sketch, not a separately reimplemented policy.
#include "../../arduino/owl_main/owl_main.ino"

int assertions = 0;
void check(bool ok, const char* label) {
    ++assertions;
    if (!ok) { std::cerr << "FAIL: " << label << '\n'; std::exit(1); }
}
void reset(int reading) {
    events.clear();
    signalTimeout = 1000;
    signalBuzzCounter = signalVibroCounter = 0;
    distance = 0;
    ultrasonic.reading = reading;
    ultrasonic.read_count = 0;
    fake_clock = 0;
}
void expect_loop(bool vibro, bool buzz, int frequency) {
    std::vector<Event> expected{
        {"write", VIBRO_PIN, LOW}, {"write", LED_PIN, LOW},
        {"silence", BUZZER_PIN, 0}
    };
    if (vibro) expected.push_back({"write", VIBRO_PIN, HIGH});
    if (buzz) {
        expected.push_back({"write", LED_PIN, HIGH});
        expected.push_back({"tone", BUZZER_PIN, frequency});
    }
    expected.push_back({"delay", -1, SIGNAL_PLAY_TIME});
    check(events == expected, "exact ordered output calls");
}
int main() {
    reset(10);
    setup();
    check(events == std::vector<Event>{{"mode", 3, OUTPUT}, {"mode", 2, OUTPUT},
          {"mode", 4, OUTPUT}, {"serial", -1, 115200}}, "setup wiring and baud");
    check(ultrasonic.trigger_pin == 12 && ultrasonic.echo_pin == 13, "sensor pins");

    // At distance 10, count is 30/60/90/120: vibration leads sound by one loop.
    events.clear(); loop(); expect_loop(false, false, 0);
    check(signalBuzzCounter == 30 && signalTimeout == 100, "first interval");
    events.clear(); loop(); expect_loop(false, false, 0);
    events.clear(); loop(); expect_loop(true, false, 0);
    events.clear(); loop(); expect_loop(true, true, 50);
    check(signalBuzzCounter == 0 && signalVibroCounter == 0, "both reset");
    check(ultrasonic.read_count == 4 && fake_clock == 80, "one read and 20ms per loop");
    events.clear(); loop(); expect_loop(false, false, 0);

    // Threshold equality and immediate response to a new reading.
    reset(3); loop(); expect_loop(true, true, 15);
    reset(4); loop(); expect_loop(true, false, 0);
    ultrasonic.reading = 1;
    events.clear(); loop(); expect_loop(true, true, 5);

    // These are observations of existing behavior, NOT a safe failure policy.
    reset(0); loop(); expect_loop(true, true, 0);
    reset(500);
    for (int i = 1; i <= 167; ++i) {
        events.clear(); loop();
        expect_loop(i == 167, i == 167, 2500);
    }
    check(signalTimeout == 5000 && signalBuzzCounter == 0, "no range filtering");

    // The sketch uses loop counters, not millis(). Crossing a simulated 32-bit
    // clock boundary has no effect on its existing call sequence.
    reset(10); fake_clock = UINT32_MAX - 9;
    loop(); expect_loop(false, false, 0);
    check(fake_clock == 10 && signalBuzzCounter == 30, "mock clock rollover");
    events.clear(); loop(); expect_loop(false, false, 0);

    // Stay in the range representable by both a 16-bit and 32-bit signed int.
    reset(3276);
    for (int i = 0; i < 1092; ++i) { events.clear(); loop(); }
    expect_loop(true, true, 16380);
    check(signalBuzzCounter == 0, "last exact safe 16-bit timeout multiple");
    std::cout << assertions << " host characterization assertions passed\n";
}
