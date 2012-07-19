
#include "include_arduino.h"


#include "parameters.h"
#include <math.h>

#include "encoder.h"
#include "robotstate.h"
#include "pwm.h"
#include "fifo.h"
#include "message.h"
#include "control.h"

unsigned long index = 0;
unsigned long timeStart = 0;

void setup(){
        Serial.begin(115200);
	value_pwm_left = 0;
	value_pwm_right = 0;
	/*Initialise la file des buts a atteindre*/
	initGoals();
	/*Active les pwm*/
	initPWM();
	/*Initialise le regulateur*/
	initController();
	/*Active les interruptions sur les encodeurs*/
	initEncoders();
	/*Definit la position initiale du robot*/
	initRobotState();
	/*Active la liaison serie*/
        //initSerialLink();

        //pushGoalPosition(NO_ID,(double)100*ENC_MM_TO_TICKS, (double)0*ENC_MM_TO_TICKS, (double)100);
        
        //Serial.println(1000 * (ENC_MM_TO_TICKS));
        
        pushGoalPosition(NO_ID, (0 * ENC_MM_TO_TICKS), (1000 * ENC_MM_TO_TICKS), (double)255);

	// LED qui n'en est pas une
	pinMode(DEL_PIN,OUTPUT);
}

void loop(){  
        /* on note le temps de debut */
	timeStart = micros();

         /*Serial.print(value_left_enc);
         Serial.print(" : ");
         Serial.println(value_right_enc);
         
         Serial.print(robot_get_x() * (ENC_TICKS_TO_MM * .01));
         Serial.print(" - ");
         Serial.println(robot_get_y() * (ENC_TICKS_TO_MM * .01));*/
	/* La del est allumee pendant le traitement */
	digitalWrite(DEL_PIN, HIGH);

	/* zone programmation libre */
	/*lecture des ordres*/
        //readIncomingData();

	/*recuperation du but suivant (vitesse, angle ou position) */
	if(current_goal.isReached)
		popGoal(); /* va changer la valeur de current_goal */

	/*traitement des taches*/
	if(!current_goal.isReached){
		if(current_goal.type == TYPE_SPEED)
			speedControl(&value_pwm_left,&value_pwm_right);
		else if(current_goal.type == TYPE_ANGLE)
			angleControl(&value_pwm_left,&value_pwm_right);
		else if(current_goal.type == TYPE_POSITION)
			positionControl(&value_pwm_left,&value_pwm_right);
		else if(current_goal.type == TYPE_PWM)
			pwmControl(&value_pwm_left,&value_pwm_right);
	}

	/*ecriture de la sortie*/
	setLeftPWM(value_pwm_left);
	setRightPWM(value_pwm_right);

	/*modele d'evolution*/
	computeRobotState();

	/* fin zone de programmation libre */
	
	/* On eteint la del */
	digitalWrite(DEL_PIN, LOW);

	/* On attend le temps qu'il faut pour boucler */
	long udelay = DUREE_CYCLE*1000-(micros()-timeStart);
	if(udelay<0)
		Serial.println("ouch : mainloop trop longue");
	else
		 delayMicroseconds(udelay);

}


