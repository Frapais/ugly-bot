#include <Arduino.h>

#include <PID_v1.h>

#include <encoders.h>



#define INTA_1 6
#define INTA_2 7
#define INTB_1 8
#define INTB_2 9

//Define Variables we'll be connecting to
double   Setpoint1, Setpoint2, InputL, InputR, OutputL, OutputR;

//Specify the links and initial tuning parameters
double Kpl=100, Kil=10000, Kdl=1;
double Kpr=100, Kir=10000, Kdr=1;

PID myPIDL(&InputL, &OutputL, &  Setpoint1, Kpl, Kil, Kdl, DIRECT);
PID myPIDR(&InputR, &OutputR, &  Setpoint1, Kpr, Kir, Kdr, DIRECT);


void setup()
{
  Serial.begin(115200);
  encodersSetup();
  //initialize the variables we're linked to
  InputL = updateEncoderL();
  InputR = updateEncoderR();

  Setpoint1 = 1492; //encoder ticks for 1 meter
  Setpoint2 = 0; //encoder ticks for 0 meter

  //turn the PID on
  myPIDL.SetMode(AUTOMATIC);
  myPIDR.SetMode(AUTOMATIC);

  myPIDL.SetSampleTime(10);
  myPIDR.SetSampleTime(10);
  myPIDL.SetOutputLimits(0, 100); // PID output limits for left motor
  myPIDR.SetOutputLimits(0, 100); // PID output limits for right motor
}

void loop()
{
  delay(100); // Delay to allow for serial output to be readable
  InputL = updateEncoderL();
  InputR = updateEncoderR();

  Serial.print("InputL: ");
  Serial.print(InputL);
  Serial.print(" InputR: ");
  Serial.print(InputR);
  Serial.print("  ||  OutputL: ");
  Serial.print(OutputL);
  Serial.print(" OutputR: ");
  Serial.println(OutputR);


  myPIDL.Compute();
  InputL = updateEncoderL();
  myPIDR.Compute();
  InputR = updateEncoderR();

  // analogWrite(PIN_OUTPUT, OutputL);
  if (OutputL > 0) {
    // FowardL((int)OutputL);
    analogWrite(INTA_1, OutputL);
    analogWrite(INTA_2, 0);
  } else {
    analogWrite(INTA_1, 0);
    analogWrite(INTA_2, OutputL);
  }

  if (OutputR > 0) {
    analogWrite(INTB_1, OutputR);
    analogWrite(INTB_2, 0);
  } else {
    analogWrite(INTB_1, 0);
    analogWrite(INTB_2, OutputR);
  }

  // For testing purposes, let's just toggle the motor outputs
  // This will turn on one motor at a time for 1 second each
  // analogWrite(INTA_1, 90);
  // analogWrite(INTA_2, 0);
  // analogWrite(INTB_1, 90);
  // analogWrite(INTB_2, 0);
    
  // delay(1000); // Wait for 1 second

  // analogWrite(INTA_1, 0);
  // analogWrite(INTA_2, 90);
  // analogWrite(INTB_1, 0);
  // analogWrite(INTB_2, 90);

  // delay(1000); // Wait for 1 second
  
}