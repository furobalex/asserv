/*
 * encoder.cpp
 *
 *  Created on: 13 janv. 2011
 *	  Author: HoHen
 */
#include "encoder.h"
#include "include_arduino.h"

long value_left_enc;
long value_right_enc;

void initEncoders(){
	value_left_enc = 0;
	value_right_enc = 0;

	pinMode(PIN_LEFT_A,INPUT);
	pinMode(PIN_LEFT_B,INPUT);
	pinMode(PIN_RIGHT_A,INPUT);
	pinMode(PIN_RIGHT_B,INPUT);

	digitalWrite(PIN_LEFT_A, HIGH);
	digitalWrite(PIN_LEFT_B, HIGH);
	digitalWrite(PIN_RIGHT_A, HIGH);
	digitalWrite(PIN_RIGHT_B, HIGH);

	attachInterrupt(INTERRUPT_LEFT_A,valueChangeOnEncoderLeftPinA,RISING);
	attachInterrupt(INTERRUPT_RIGHT_A,valueChangeOnEncoderRightPinA,RISING);
}


void valueChangeOnEncoderLeftPinA(){
	if(VALUE_PIN_LEFT_B == HIGH)
      		value_left_enc++;
      	else
      		value_left_enc--;
}

void valueChangeOnEncoderRightPinA(){
	if(VALUE_PIN_RIGHT_B == HIGH)
		value_right_enc++;
	else
		value_right_enc--;
}
