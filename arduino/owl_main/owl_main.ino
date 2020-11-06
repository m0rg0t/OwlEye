#include <Ultrasonic.h>
//https://github.com/ErickSimoes/Ultrasonic
//https://github.com/Martinsos/arduino-lib-hc-sr04

const int LED = 3;
const int BUZZER = 4;

Ultrasonic ultrasonic(12, 13);
int distance;

// the setup routine runs once when you press reset:
void setup()
{
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

  Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop()
{
  distance = ultrasonic.read();

  digitalWrite(led, HIGH); // turn the LED on (HIGH is the voltage level)
  //tone(buzzer, 1000);

  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.println("---");
  delay(500);

  digitalWrite(led, LOW); // turn the LED off by making the voltage LOW
  //noTone(buzzer);
}
