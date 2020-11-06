#include <Ultrasonic.h>
//https://github.com/ErickSimoes/Ultrasonic
//https://github.com/Martinsos/arduino-lib-hc-sr04


const int led = 3;
const int buzzer = 4;
const int minTime = 10;
int signalTimeout = 1000;
int signalCounter = 0;

Ultrasonic ultrasonic(12, 13);
int distance;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

  Serial.begin(115200);
  //HCSR04.begin(triggerPin, echoPins, echoCount);
}

// the loop routine runs over and over again forever:
void loop() {
  distance = ultrasonic.read();

  digitalWrite(led, LOW);
  noTone(buzzer);

  Serial.print("Distance: ");
  Serial.println(distance);

  signalTimeout = minTime * distance;
  signalCounter = signalCounter + minTime;

  Serial.println("---");
  delay(minTime);

  if (signalCounter >= signalTimeout) {
    digitalWrite(led, HIGH);
    tone(buzzer, 1000);
    signalCounter = 0;
    delay(minTime);
  }
  
  //
}
