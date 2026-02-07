#include <Braccio.h>
#include <Servo.h>

int Gripper;
int Gripper_ROT;
const int msg_pin = 8;
const int CLOSE = 73;
const int OPEN = 10;

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;

void setup() {
  Serial.begin(9600);
  pinMode(msg_pin, INPUT);
  Braccio.begin();
  Gripper = OPEN;
  Gripper_ROT = 180;

  //Braccio.ServoMovement(200,         0, 43, 60, 0, 180,  10);
  //Braccio.ServoMovement(200,         0, 43, 60, 0, 180,  180);
  //Braccio.ServoMovement(200,         0, 43, 60, 0, 180,  180);
  //Braccio.ServoMovement(20,         90, 43, 180, 30, 90, 10);
  //delay(1000);
  //Braccio.ServoMovement(20,         90, 43, 55, 30, 90, 10);
  //delay(1000);
  //Braccio.ServoMovement(20,         90, 43, 55, 30, 90, 73);
  //Braccio.ServoMovement(50,         0, 43, 180, 0, 175, 73);

}

void loop() {
  Braccio.ServoMovement(5, 0, 43, 180, 0, Gripper_ROT, Gripper);
  int msg = digitalRead(msg_pin);
  
  /*
  if (msg == 1) {
     Serial.println("HIGH");
     Gripper = CLOSE;
   } else if (msg == 0) {
     Serial.println("LOW");
     Gripper = OPEN;
  }
  */
  
  if (msg == 1) {
     Serial.println("HIGH");
     //Gripper += 10;
     Gripper_ROT += 10;
   } else if (msg == 0) {
     Serial.println("LOW");
  }


  if (Gripper_ROT >= 180) {
    Gripper_ROT = 0;
  }

  //if (Gripper >= 73) {
  //  Gripper = 10;
  //}
  
  delay(10);
}