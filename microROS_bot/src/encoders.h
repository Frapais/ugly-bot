#ifndef ENCODERS_H
#define ENCODERS_H

// #include <Arduino.h>
#include "MT6701.h"

// Pin definitions
#define CS_PIN_L  7
#define CS_PIN_R  8

// Function declarations
void encodersSetup();
void updateEncoders();

#endif // ENCODERS_H