#ifndef ENCODERS_H
#define ENCODERS_H

// #include <Arduino.h>
#include "MT6701.h"

// Pin definitions
#define CS_PIN_L  2
#define CS_PIN_R  3

// Function declarations
void encodersSetup();
long updateEncoderR();
long updateEncoderL();

#endif // ENCODERS_H
