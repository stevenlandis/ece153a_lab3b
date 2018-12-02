/*
These functions return the sine and cosine of a floating point input.
This is done through by implementing the taylor series.
Because the taylor series is unstable after PI/2, the double angle formula is used for values larger than PI/2
Input - 
	x - a floating point value that can range from -PI to PI. 
Returns
	floating point value of the sine or cosine of x respectively
	sine and cosine are undefined for x out of bounds.
*/

#ifndef TRIG_H
#define TRIG_H

#include "fft.h"

int factorial(int a); 
float sine(float x);
float cosine(float x);
float fastCos(int k, int bi);
float fastSin(int k, int bi);

#endif
