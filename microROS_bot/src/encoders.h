#ifndef ENCODERS_H
#define ENCODERS_H

// #include <Arduino.h>
#include "MT6701.h"

// Pin definitions
#define CS_PIN_L  6
#define CS_PIN_R  7

// Function declarations
void encodersSetup();
long updateEncoderR();
long updateEncoderL();

#endif // ENCODERS_H
