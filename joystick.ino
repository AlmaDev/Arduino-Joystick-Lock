#include <Servo.h>
#include "music.h"


#define VRx A0
#define VRy A1
#define SW 9
#define LED LED_BUILTIN

int x = 0;
int y = 0;
String coor;
int counter = 0;
int clk = 0;
int count = 0;
const int melodyPin = 3;
bool done = false;
int servo_rot = 180;

Servo lock;

int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
};

int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

const String code[] = {"4852", "5652", "5256", "5248"};
String current_code[4];

void buzz(int targetPin, long frequency, long length) {
  digitalWrite(13, HIGH);
  long delayValue = 1000000 / frequency / 2;
  long numCycles = frequency * length / 1000;
  for (long i = 0; i < numCycles; i++) {
    digitalWrite(targetPin, HIGH);
    delayMicroseconds(delayValue);
    digitalWrite(targetPin, LOW);
    delayMicroseconds(delayValue);
  }
  digitalWrite(13, LOW);

}

void sing() {
  int size = sizeof(melody) / sizeof(int);
  for (int thisNote = 0; thisNote < size; thisNote++) {
    int noteDuration = 1000 / tempo[thisNote];

    buzz(melodyPin, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    buzz(melodyPin, 0, noteDuration);

  }
}

void setup() {
  lock.attach(11);
  pinMode(SW, INPUT);
  pinMode(melodyPin, OUTPUT);
  digitalWrite(SW, HIGH);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  lockD();
}

int treatValue(int data) {
  return (data * 9 / 1024) + 48;
}

void loop() {
  done = false;
  x = analogRead(VRx);
  y = analogRead(VRy);
  coor = String(String(treatValue(x)) + String(treatValue(y)));
  if (coor != "5252" && (digitalRead(SW) == 0)) {
    buzz(melodyPin, NOTE_E7, 100);
    for (int i = 0; i < 4; i = i + 1) {
      if (code[i] == current_code[i]) {
        Serial.println(count);
        count ++;
      }
      if (count == 4) {
        delay(250);
        sing();
        unlockD();
        Serial.println("Done");
        done = true;
        count = 0;
        counter = 0;
        for (byte i = 0; i < 4; i++) {
          current_code[i] = "0";
        }
      }
    }
    Serial.println(coor);
    current_code[counter] = coor;
    counter++;
  } else if (digitalRead(SW) == 0) {
    if (servo_rot == -180) {
      lockD();
      counter = 0;
    }
  }
  if (counter == 4 && (done == false)) {
    for (byte i = 0; i < 4; i++) {
      current_code[i] = "0";
    }
    counter = 0;
    count = 0;
    Serial.println("Cleared");
    delay(250);
    buzz(melodyPin, NOTE_E7, 100);
    buzz(melodyPin, NOTE_G6, 100);
  }
  delay(250);
}

void unlockD() {
  lock.write(-180);
  servo_rot = -180;
  delay(2000);
  Serial.println("Unlocking...");
}

void lockD() {
  lock.write(180);
  servo_rot = 180;
  delay(2000);
  Serial.println("Locking...");
}