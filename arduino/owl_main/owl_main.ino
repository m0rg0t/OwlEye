#include <Ultrasonic.h>
//https://github.com/ErickSimoes/Ultrasonic
//https://github.com/Martinsos/arduino-lib-hc-sr04

const int VIBRO_PIN = 2; //Digital pin where vibro attahed
const int LED_PIN = 3; //Digital pin where led attached
const int BUZZER_PIN = 4; //Digital pin where buzzer sound stuff is attached
const int HC_TRIG_PIN = 12; //Triger pin of HC-SR04 sensor
const int HC_ECHO_PIN = 13; //Echo pin of HC-SR04 sensor

const int MIN_TIME = 10; //minimal amunt of time we use
const int SIGNAL_INCREMENT_TIME = MIN_TIME * 3;
const int SIGNAL_PLAY_TIME = 20;

const int VIBRO_ADDITIONAL_TIME = 10;

int signalTimeout = 1000;
int signalBuzzCounter = 0;
int signalVibroCounter = 0;

Ultrasonic ultrasonic(HC_TRIG_PIN, HC_ECHO_PIN);
int distance;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(LED_PIN, OUTPUT);
  pinMode(VIBRO_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT); // Set buzzer - pin 9 as an output

  Serial.begin(115200);
  //HCSR04.begin(triggerPin, echoPins, echoCount);
}

// the loop routine runs over and over again forever:
void loop() {
  distance = ultrasonic.read();

  digitalWrite(VIBRO_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  Serial.print("Distance: ");
  Serial.println(distance);

  signalTimeout = MIN_TIME * distance;
  signalBuzzCounter = signalBuzzCounter + SIGNAL_INCREMENT_TIME;
  signalVibroCounter =  signalVibroCounter + SIGNAL_INCREMENT_TIME;
  Serial.println("---");

  //check for vibro motor
  //for good vibration we need to make it longer then buzzer sounds
  if (signalVibroCounter >= (signalTimeout - VIBRO_ADDITIONAL_TIME)) {
    digitalWrite(VIBRO_PIN, HIGH);
  }

  if (signalBuzzCounter >= signalTimeout) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, distance * 5);
    signalBuzzCounter = 0;
    signalVibroCounter = 0; //we reset vibro here too
  }
  delay(SIGNAL_PLAY_TIME);
}
