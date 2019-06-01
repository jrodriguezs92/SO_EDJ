// ********************************************************
//            Instituto Tecnológico de Costa Rica
//              Computer Engineering
//
//      Programmer: Esteban Agüero Pérez (estape11)
//      Programming Language: C / C++
//      Version: 1.0
//      Last Update: 28/05/2019
//
//              Operating Systems Principles
//              Professor. Diego Vargas
// ********************************************************

#include "meArm.h"
#include <Servo.h>

int baseServo = 5;
int rightServo = 4;
int leftServo = 3;
int clawPin = 8;

meArm arm;
bool claw = false; // closed

void refresh(String cmd){
    short value = cmd.substring(1, cmd.length()-1).toInt();
    if (cmd[0] == 'X') {
        arm.gotoPoint(value, arm.getY(), arm.getZ());
           
    } else if (cmd[0] == 'Y') {
        arm.gotoPoint(arm.getX(), value, arm.getZ());
           
    } else if (cmd[0] == 'Z') {
        arm.gotoPoint(arm.getX(), arm.getY(), value);
           
    } else if (cmd[0] == 'P') {
        arm.closeGripper();

    } else if (cmd[0] == 'D') {
	arm.openGripper();

    }
    
}

void setup() {
    Serial.begin(115200);
    Serial.println(F(">> Setting up meArm"));
    arm.begin(baseServo, rightServo, leftServo, clawPin); // starts the servos
    arm.gotoPoint(0,150, 50); // initial position
    
}

void loop() {
    //arm.gotoPoint(arm.getX(),arm.getY(),200);
    //delay(2000);
    //arm.gotoPoint(0,150, 0);
    //delay(2000);
    if (Serial.available() > 0) {
        refresh(Serial.readString());
        
    }
    delay(500);
    
}
