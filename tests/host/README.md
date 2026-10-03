# Host-only sketch characterization

Run `sh tests/host/run.sh` with a C++17 compiler. There are no package downloads,
Arduino board installs, serial connections, sensor access, or physical outputs.
The harness includes the production `owl_main.ino` directly and replaces only
Arduino/Ultrasonic IO with in-memory test doubles. Undefined-behavior sanitizer
is enabled. The production sketch is intentionally unchanged.

Checks cover setup pin/baud calls, output call order, one sensor read per loop,
the vibration lead, threshold equality, counter reset, changed readings,
zero distance and 500 cm observations, and a simulated 32-bit clock rollover.

## Observations, not a safety specification

- Counters advance by 30 each loop; the explicit delay is 20 ms. Sensor read,
  serial and IO costs are additional. This does **not** measure real elapsed time.
- A zero reading immediately produces HIGH vibration/LED calls and tone(4, 0).
  No-echo semantics depend on the actual sensor library; this is not endorsed
  as a safe failure response.
- 500 cm is not rejected. The first alert is at loop 167.
- The sketch does not read millis(). Mock-clock rollover leaves output unchanged;
  this is not proof of correct elapsed-time scheduling.
- Host int is usually 32-bit; many Arduino boards use 16-bit int. The harness
  stays within shared representable arithmetic (through distance 3276 and
  counter 32760). Larger/negative inputs can expose conversion/overflow issues.
  No extrapolation to target integer widths or undefined behavior is valid.

## Required before firmware or policy changes

Record the exact board/FQBN, core version, installed Ultrasonic library/version
(the source comments name two alternatives), wiring and electrical baseline.
Obtain the owner's intended no-echo/invalid/out-of-range behavior, then validate
timing and outputs on an appropriate supervised bench. No target firmware build,
flashing, physical behavior, assistive safety, or navigation efficacy is validated
by these tests. CI runs only the host executable with read-only repository access.
