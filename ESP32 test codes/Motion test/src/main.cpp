#include <Arduino.h>
#include <PID_v1.h>
#include <encoders.h>

#define INTA_1 6
#define INTA_2 7
#define INTB_1 8
#define INTB_2 9

#define MAX_PWM 255 // Maximum speed for the motors, adjust as needed
#define TICKS_PER_METER 1492 // Encoder ticks for 1 meter
#define CONTROL_INTERVAL 10 // Control interval in milliseconds

// Robot geometry
#define WHEEL_BASE 0.16 // Distance between wheels in meters
#define WHEEL_DIAMETER 0.068 // Diameter of the wheels in meters

// Define Variables we'll be connecting to
double SetpointL, SetpointR, InputL, InputR, OutputL, OutputR;

// Specify the links and initial tuning parameters
double Kpl = 1.0, Kil = 0.4, Kdl = 0.1;
double Kpr = 1.1, Kir = 0.4, Kdr = 0.1;

PID myPIDL(&InputL, &OutputL, &SetpointL, Kpl, Kil, Kdl, DIRECT);
PID myPIDR(&InputR, &OutputR, &SetpointR, Kpr, Kir, Kdr, DIRECT);

// Motion control variables
double velocityForward = 0.0; // Forward velocity in m/s
double velocityRotate = 0.0;  // Rotational velocity in rad/s

unsigned long lastControlTime = 0;

void setup() {
    Serial.begin(115200);
    encodersSetup();

    // Initialize PID controllers
    myPIDL.SetMode(AUTOMATIC);
    myPIDR.SetMode(AUTOMATIC);

    myPIDL.SetSampleTime(CONTROL_INTERVAL);
    myPIDR.SetSampleTime(CONTROL_INTERVAL);
    myPIDL.SetOutputLimits(-MAX_PWM, MAX_PWM); // PID output limits for left motor
    myPIDR.SetOutputLimits(-MAX_PWM, MAX_PWM); // PID output limits for right motor

    // Initialize encoder inputs
    InputL = updateEncoderL();
    InputR = updateEncoderR();

    // Set initial motion (example: forward motion at 0.5 m/s and rotation at 0.0 rad/s)
    velocityForward = 0.2;
    velocityRotate = 0.0;

    lastControlTime = millis();
}

void loop() {
    unsigned long currentTime = millis();

    // Check if it's time for the next control update
    if (currentTime - lastControlTime >= CONTROL_INTERVAL) {
        lastControlTime = currentTime;

        // Calculate wheel velocities based on forward and rotational motion
        double velocityL = velocityForward - (velocityRotate * WHEEL_BASE / 2);
        double velocityR = velocityForward + (velocityRotate * WHEEL_BASE / 2);

        // Calculate target encoder ticks for each wheel
        double targetTicksL = velocityL * TICKS_PER_METER * (CONTROL_INTERVAL / 1000.0);
        double targetTicksR = velocityR * TICKS_PER_METER * (CONTROL_INTERVAL / 1000.0);

        // Update PID setpoints
        SetpointL += targetTicksL;
        SetpointR += targetTicksR;

        // Update encoder inputs
        InputL = updateEncoderL();
        InputR = updateEncoderR();

        // Compute PID outputs
        myPIDL.Compute();
        myPIDR.Compute();

        // Drive motors based on PID outputs
        if (OutputL > 0) {
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

        // Debugging output
        Serial.print("VelocityForward: ");
        Serial.print(velocityForward);
        Serial.print(" VelocityRotate: ");
        Serial.print(velocityRotate);
        Serial.print(" || InputL: ");
        Serial.print(InputL);
        Serial.print(" InputR: ");
        Serial.print(InputR);
        Serial.print(" || OutputL: ");
        Serial.print(OutputL);
        Serial.print(" OutputR: ");
        Serial.println(OutputR);
    }
}