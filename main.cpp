#include <Arduino.h>
#include <string.h>
#include <Servo.h>
//#include <A4988.h>
//#include <Stepper.h>
//#include <sstream>

using namespace std;

#define rmdirection 0x016
#define lmdirection 0x018
#define bmdireciton 0x01A
#define lrmdirection 0x01C
#define rmstep 0x017
#define lmstep 0x019
#define bmstep 0x01B
#define lrmstep 0x01D
#define sser 12
#define bser 13
#define fronfaceindex  1
#define upfaceindex    2
#define backfaceindex  3
#define downfaceindex  4
#define rightfaceindex 5
#define leftfaceindex  6
#define stepcounts 200

String serialinput = "";
int  x = 0 , swapface , index = 3  ,dface = 4; 
int faceshashset[4]= { 1 , 2 , 3 , 4 }; // Frotn , UP, Back , Down
int facescanhasset[4] = { 1 , 5 , 3 , 6 }; // Front , Right , Back , Left
int steps90deg = 50 , steps180deg = 100 , stepsi90deg = -50 , stepspeed = 2000 , delay_val = 200  , delay_val_ser = 200;
//  90 degres step    180 dergres step    -90 degres step     puls delay value   after movment delay   after movment servo delay value
/*Stepper rightmotor (stepcounts , rmdirection , rmstep);
Stepper leftmotor  (stepcounts , lmdirection , lmstep);
Stepper downmotor  (stepcounts , bmdireciton , bmstep);*/

Servo sideservo , bottomservo;

void setup() {

  pinMode(rmdirection,OUTPUT);
  pinMode(lmdirection,OUTPUT);
  pinMode(bmdireciton,OUTPUT);
  pinMode(lrmdirection , OUTPUT);
 
  pinMode(rmstep,OUTPUT);
  pinMode(lmstep,OUTPUT);
  pinMode(bmstep,OUTPUT);
  pinMode(lrmstep , OUTPUT);

  digitalWrite( rmdirection , LOW);
  digitalWrite( lmdirection , LOW);
  digitalWrite( bmdireciton , LOW);
  digitalWrite( rmstep , LOW);
  digitalWrite( lmstep , LOW);
  digitalWrite( bmstep , LOW);

  sideservo.attach(sser);
  bottomservo.attach(bser);

  sideservo.write(90);
  bottomservo.write(90);

  delay(2000);

  sideservo.write(180);
  bottomservo.write(120);

  Serial.begin(9600);

}

void loop() {
  serialinput = Serial.readString();
  if(serialinput.startsWith("S")){
    scancupe(serialinput.substring(2));
  } else {
    char arr[serialinput.length() + 1]; 
    strcpy(arr, serialinput.c_str()); 
    char *token = strtok(arr, " ");
    while (token != NULL)
    {
        soultionimp(token);
        token = strtok(NULL, " ");
    }

  }
}

void scancupe(String face){  // cube scan function
    if(face == "R"){
      bottomservo.write(120);
      sideservo.write(90);
      bottom(50);
    } else if(face == "U"){
      sideservo.write(180);
      bottomservo.write(90);
      dsmotors(1 , 50);
      dsmotors(1 , 50);
    } else if (face == "E"){
      bottomservo.write(120);
      sideservo.write(180);
    }
}

void soultionimp(String movment){ // soultion implimatino
  if(movment == "R"){
    fullcatch();
    right(steps90deg);
  } else if(movment == "R2"){
    fullcatch();
    right(steps180deg);
  } else if(movment == "R3"){
    fullcatch();   
    right(stepsi90deg);
  } else if(movment == "L"){
    fullcatch();   
    left(steps90deg);
  } else if(movment == "L2"){
    fullcatch();   
    left(steps180deg);
  } else if(movment == "L3"){
    fullcatch();   
    left(stepsi90deg);
  } else if(movment == "F"){
    getrightface(1);
    fullcatch();
    bottom(steps90deg);
  } else if(movment == "F2"){
    getrightface(1);
    fullcatch();
    bottom(steps180deg);
  } else if(movment == "F3"){
    getrightface(1);
    fullcatch();
    bottom(stepsi90deg);
  } else if(movment == "B"){
    getrightface(3);
    fullcatch();
    bottom(steps90deg);
  } else if(movment == "B2"){
    getrightface(3);
    fullcatch();
    bottom(steps180deg);
  } else if(movment == "B3"){
    getrightface(3);
    fullcatch();
    bottom(stepsi90deg);
  }else if(movment == "U"){
    getrightface(2);
    fullcatch();
    bottom(steps90deg);
  } else if(movment == "U2"){
    getrightface(2);
    fullcatch();
    bottom(steps180deg);
  } else if(movment == "U3"){
    getrightface(2);
    fullcatch();
    bottom(stepsi90deg);
  } else if(movment == "D"){
    getrightface(4);
    fullcatch();
    bottom(steps90deg);
  } else if(movment == "D2"){
    getrightface(2);
    fullcatch();
    bottom(steps180deg);
  } else if(movment == "D3"){
    getrightface(2);
    fullcatch();
    bottom(stepsi90deg);
  } else if (movment = "E"){
    fullcatch();
  } else {
    Serial.println("Invailde input");
  }
}

void getrightface(int faceindex){ // Get correct face to bottom motor
  swapface = 0;
  while(dface != faceindex){
    if(index == 3){
      index = 0;
      dface = faceshashset[index];
      swapface++;
    } else if (dface != faceindex){
      index++;
      dface = faceshashset[index];
      swapface++;
    }
  }

  if(swapface > 0){
    sidecatch();
    dsmotors(swapface , 50);
  }

}

void dsmotors(int swapfacenum , int steps){ //Rotate the right and left "side" motors at the same time
  digitalWrite(rmdirection , LOW);
  digitalWrite(lmdirection , HIGH);
  //for (int i = 0 ; i < swapfacenum ; i++){
    for (int j = 0 ; j < steps * swapfacenum ; j++){
      digitalWrite(rmstep , HIGH);
      digitalWrite(lmstep , HIGH);
      delayMicroseconds(stepspeed);
      digitalWrite(rmstep , LOW);
      digitalWrite(lmstep , LOW);
      delayMicroseconds(stepspeed);
    }
  //}
  delay(delay_val);
}

void right(int steps){ // ROtate the right motor
  if(steps < 0 ){
    digitalWrite(rmdirection , HIGH);
    steps = steps * -1;
  } else if (steps > 0 ){
    digitalWrite(rmdirection , LOW);
  }
  for (int i = 0 ; i < steps ; i++){
    digitalWrite(rmstep,HIGH);
    delayMicroseconds(stepspeed);
    digitalWrite(rmstep,LOW);
    delayMicroseconds(stepspeed);
  }
  delay(delay_val);

}

void left(int steps){ // Rotate the left motor
    if(steps < 0 ){
      digitalWrite(lmdirection , HIGH);
      steps = steps * -1;
  } else if (steps > 0 ){
      digitalWrite(lmdirection , LOW);
  }
  for (int i = 0 ; i < steps ; i++){
    digitalWrite(lmstep,HIGH);
    delayMicroseconds(stepspeed);
    digitalWrite(lmstep,LOW);
    delayMicroseconds(stepspeed);
  }
  delay(delay_val);
}

void bottom(int steps){ // Rotate the down motor
  if(steps < 0 ){
    digitalWrite(bmdireciton , HIGH);
    steps = steps * -1;
  } else if (steps > 0 ){
    digitalWrite(bmdireciton , LOW);
  }
  for (int i = 0 ; i < steps ; i++){
    digitalWrite(bmstep,HIGH);
    delayMicroseconds(stepspeed);
    digitalWrite(bmstep,LOW);
    delayMicroseconds(stepspeed);
  }
  delay(delay_val);
}

void fullcatch(){ // make all motors grape the cube
    bottomservo.write(90);
    delay(delay_val_ser);
    sideservo.write(90);
    delay(delay_val_ser);
}

void sidecatch(){ // make only right and left motor grape the cube
  sideservo.write(90);
  delay(delay_val_ser);
  bottomservo.write(180);
  delay(delay_val_ser);
}

/*void gotofrontface(){

}*/

/*#include <stdio.h>    // Test to spilet string by a specific charecter
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
string serialinput = "R3 D H F D3 I2 LF K5 D4 F GG R ";
string tostringchar;
string arr[50];
int i = 0;
//stringstream ss;
int main()
{
  stringstream  ss(serialinput);
    while (ss.good())
    {
        ss >> arr[i];
        cout << arr[i] << endl ;
        if (arr[i] == "R" || arr[i] == "R2" || arr[i] == "R3"){
            cout <<" this is right move" << endl;
        }
        i++;
    }

}*/