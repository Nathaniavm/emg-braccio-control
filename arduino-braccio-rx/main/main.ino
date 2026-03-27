#include <Braccio.h>
#include <Servo.h>
#include <SoftwareSerial.h>

// Braccio
Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;

// STYRING
int direction = 0;
int joint = 1;

int M1 = 0;
int M2 = 45;
int M3 = 180;
int M4 = 0;
int M5 = 175;
int M6 = 73;
const int adjustment = 2;
const int timestep = 20;

// Kommunikasjon
String incoming = "";

void setup() {
  Serial.begin(9600);  
  Braccio.begin();
}

void loop() {
// ---- Les seriell data ----
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {   // ferdig melding

      int sep = incoming.indexOf(';');

      if (sep > 0) {
        direction = incoming.substring(0, sep).toInt();
        joint = incoming.substring(sep + 1).toInt();
      }

      incoming = "";   // tøm buffer
    }
    else {
      incoming += c;   // bygg opp meldingen
    }
  }

  switch (joint) {
    case (1):
      if (direction == 2) {M1 += adjustment;}
      else if (direction == 1) {M1 -= adjustment;}
      M1 = constrain(M1, 0, 180);
      break;

    case (2):
      if (direction == 2) {M2 += adjustment;}
      else if (direction == 1) {M2 -= adjustment;}
      M2 = constrain(M2, 15, 165);
      break;

    case (3):
      if (direction == 2) {M3 += adjustment;}
      else if (direction == 1) {M3 -= adjustment;}
      M3 = constrain(M3, 0, 180);
      break;

    case (4):
      if (direction == 2) {M4 += adjustment;}
      else if (direction == 1) {M4 -= adjustment;}
      M4 = constrain(M4, 0, 180);
      break;

    case (5):
      if (direction == 2) {M5 += adjustment;}
      else if (direction == 1) {M5 -= adjustment;}
      M5 = constrain(M5, 0, 180);
      break;

    case (6):
      if (direction == 2) {M6 += adjustment;}
      else if (direction == 1) {M6 -= adjustment;}
      M6 = constrain(M6, 10, 73);
      break;
  }
  

  delay(10);
  Braccio.ServoMovement(timestep, M1, M2, M3, M4, M5, M6);
}
