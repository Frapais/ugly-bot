#include <Arduino.h>
#include <SPI.h>
#include "MT6701.h"

// You could set any pin
#define CS_PIN_L  7
#define CS_PIN_R  8

MT6701 encoder_l;
MT6701 encoder_r;

// Variables to store incremental values and previous angles
float prev_angle_l = 0.0;
float prev_angle_r = 0.0;
long incremental_l = 0;
long incremental_r = 0;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  encoder_l.initializeSSI(CS_PIN_L);
  encoder_r.initializeSSI(CS_PIN_R);
  Serial.println("Init complete");
}

void loop() {
  // Read angles
  float angle_l = encoder_l.angleRead();
  float angle_r = encoder_r.angleRead();

  // Calculate incremental values for left encoder
  float delta_l = angle_l - prev_angle_l;
  if (delta_l > 180.0) {
    delta_l -= 360.0; // Handle wrap-around from 360 to 0
  } else if (delta_l < -180.0) {
    delta_l += 360.0; // Handle wrap-around from 0 to 360
  }
  incremental_l += (long)(delta_l);

  // Calculate incremental values for right encoder (reversed direction)
  float delta_r = angle_r - prev_angle_r;
  if (delta_r < -180.0) {
    delta_r += 360.0; // Handle wrap-around from 360 to 0
  } else if (delta_r > 180.0) {
    delta_r -= 360.0; // Handle wrap-around from 0 to 360
  }
  incremental_r -= (long)(delta_r); // Reverse the direction by negating the delta

  // Update previous angles
  prev_angle_l = angle_l;
  prev_angle_r = angle_r;

  // Print incremental values
  Serial.print("Incremental L: ");
  Serial.print(incremental_l);
  Serial.print("   |   Incremental R: ");
  Serial.println(incremental_r);

  delay(100);
}