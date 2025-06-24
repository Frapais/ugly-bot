#include <Arduino.h>

#include <PID_v1.h>

#include <encoders.h>



#define INTA_1 6
#define INTA_2 7
#define INTB_1 8
#define INTB_2 9

#define MAX_PWM 120 // Maximum speed for the motors, adjust as needed

//Define Variables we'll be connecting to
double   SetpointL, SetpointR, InputL, InputR, OutputL, OutputR;

//Specify the links and initial tuning parameters
double Kpl=1.2, Kil=1, Kdl=0.03;
double Kpr=1.3, Kir=1, Kdr=0.03;

PID myPIDL(&InputL, &OutputL, &  SetpointL, Kpl, Kil, Kdl, DIRECT);
PID myPIDR(&InputR, &OutputR, &  SetpointR, Kpr, Kir, Kdr, DIRECT);


void setup()
{
  Serial.begin(115200);
  encodersSetup();
  //initialize the variables we're linked to
  InputL = updateEncoderL();
  InputR = updateEncoderR();

  SetpointL = 1492; //LEFT encoder ticks for 1 meter
  SetpointR = 1492; //RIGHT encoder ticks for 1 meter

  //turn the PID on
  myPIDL.SetMode(AUTOMATIC);
  myPIDR.SetMode(AUTOMATIC);

  myPIDL.SetSampleTime(10);
  myPIDR.SetSampleTime(10);
  myPIDL.SetOutputLimits(-MAX_PWM, MAX_PWM); // PID output limits for left motor
  myPIDR.SetOutputLimits(-MAX_PWM, MAX_PWM); // PID output limits for right motor
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
    analogWrite(INTA_2, -OutputL);
  }

  if (OutputR > 0) {
    analogWrite(INTB_1, OutputR);
    analogWrite(INTB_2, 0);
  } else {
    analogWrite(INTB_1, 0);
    analogWrite(INTB_2, -OutputR);
  }
  
}