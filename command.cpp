#include "command.h"
#include "parameters.h"
#include "robotstate.h"
#include "fifo.h"
#include "message.h"
#include "encoder.h"
#include "pwm.h"
#include "include_arduino.h"


/**
 * Analyse le message et effectue les actions associees
 *
 * @param id : l'identifiant associe au message
 * @param header : le type de message (en-tete)
 * @param args : le tableau d'entier contenant les arguments
 * */
void cmd(int id, int id_cmd, int* args, int size){
	/* On analyse le message en fonction de son type */
	switch(id_cmd){

		case QA_ID: /* Identification */
		{
			sendMessage(id, ID_ASSERV, (char*)"asserv");
			break;
		}

		case QA_PING:
		{
			sendMessage(id, (char*)"Pong");
			break;
		}

		case QA_SETPID:
		{
			//ENC_CENTER_DIST_MM = args[0]/10.0;
			break;
		}

		case QA_GOTO:
		{
			if (size < 3)
				sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
			else
			{
				pushGoalPosition(id,(double)args[0]*ENC_MM_TO_TICKS, (double)args[1]*ENC_MM_TO_TICKS, (double)args[2]);
				sendMessage(id, 1);
			}
			break;
		}

		case QA_GOTOR:
		{
			if (size < 3)
				sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
			else
			{
				double co = cos(robot_get_angle());
				double si = sin(robot_get_angle());

				pushGoalPosition(id,((double)args[0]*co-(double)args[1]*si)*18+robot_get_x()*0.01, ((double)args[0]*si+(double)args[1]*co)*18+robot_get_y()*0.01, (double)args[2]);
				sendMessage(id, 1);
			}
			break;
		}

		case QA_TURN:
		{
			if (size < 2)
				sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
			else
			{
				double angle = moduloPI(((double)args[0]) * DEG_TO_RAD);
				sendMessage(-1, (int)(angle*100.0));
				pushGoalOrientation(id,angle,args[1]);
				sendMessage(id, 1);
			}
			break;
		}

		case QA_TURNR:
		{
			if (size < 2)
				sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
			else
			{
				double angle = moduloPI(((double)args[0]) * DEG_TO_RAD + robot_get_angle());
				pushGoalOrientation(id,angle,args[1]);
				sendMessage(id, 1);
			}
			break;
		}

		case QA_POS:
		{
			int x_mm = robot_get_x()*0.01*ENC_TICKS_TO_MM;
			int y_mm = robot_get_y()*0.01*ENC_TICKS_TO_MM;
			int a_deg = robot_get_angle()*RAD_TO_DEG;
			int tab[] = {x_mm,y_mm,a_deg};
			sendMessage(id,tab,3);
	        break;
		}

		case QA_SET_POS:
		{
			if (size < 2)
				sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
			else {
				robot_set_mm_x(args[0]);
				robot_set_mm_y(args[1]);
				robot_set_deg_angle(args[2]);
				value_left_enc = 0;
				value_right_enc = 0;
				sendMessage(id, 0);
			}
			break;
		}

		case QA_PWM:
		{
			if (size < 2)
				sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
			else
			{
				pushGoalPwm(id,args[0],args[1],args[2]);
				sendMessage(id, 1);
			}
			break;
		}
/*
		case Q_MODIF_GOAL_ABS:
		{
			if (size < 3)
				sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
			else
			{
				current_goal.type = TYPE_POSITION;
				current_goal.isReached = false;
				current_goal.x = (double)args[0]*ENC_MM_TO_TICKS;
				current_goal.y = (double)args[1]*ENC_MM_TO_TICKS;
				current_goal.speed = args[2];
				sendMessage(id, 1);
			}
			break;
		}

		case Q_MODIF_GOAL_REL:
		{
			if (size < 3)
				sendMessage(id, E_INVALID_PARAMETERS_NUMBERS);
			else
			{
				double co = cos(robot_state.angle);
				double si = sin(robot_state.angle);

				current_goal.type = TYPE_POSITION;
				current_goal.isReached = false;
				current_goal.x = (args[0]*co-args[1]*si)*18+robot_state.x;
				current_goal.y = (args[0]*si+args[1]*co)*18+robot_state.y;
				current_goal.speed = args[2];
				sendMessage(id, 1);
			}
			break;
		}
		*/
		case QA_CANCEL: /* comme stop */
		{
			clearGoals();
			current_goal.isCanceled = true;
			sendMessage(id, 0);
			break;
		}

		case QA_STOP: /* comme pause */
		{
			current_goal.isPaused = true;
			sendMessage(id, 0);
			break;
		}

		case QA_RESUME: /* comme resume */
		{
			current_goal.isPaused = false;
			sendMessage(id, 0);
			break;
		}

		case QA_RESET:
		{
			clearGoals();
			current_goal.isCanceled = true;
			current_goal.isPaused = false;
			value_left_enc = 0;
			value_right_enc = 0;
			robot_set_rad_angle(0.0);
			robot_set_x(0);
			robot_set_y(0);
			sendMessage(id, 0);
			break;
		}

		case QA_GETSENS:
		{
			if (value_pwm_right > 0)
				sendMessage(id, AVANT);
			else if (value_pwm_right < 0)
				sendMessage(id, ARRIERE);
			else
				sendMessage(id, ARRET);
			break;
		}

		case QA_GETENC:
		{
			int tab[2] = {value_left_enc,value_right_enc};
			sendMessage(id, tab, 2);
			break;
		}

		case Q_DEBUG : //TODO a degager quand tout marche
		{
			/*Serial.print("?,_________________§");
			Serial.print("uptime: ");Serial.print(millis());
			Serial.print("§angle: ");Serial.print(robot_state.angle, DEC);
			Serial.print("§speed: ");Serial.print(robot_state.speed*ENC_TICKS_TO_MM, DEC);
			Serial.print("§speedt: ");Serial.print(robot_state.speed, DEC);
			Serial.print("§x: ");Serial.print(robot_state.x*ENC_TICKS_TO_MM, DEC);
			Serial.print("§xt: ");Serial.print(robot_state.x, DEC);
			Serial.print("§y: ");Serial.print(robot_state.y*ENC_TICKS_TO_MM, DEC);
			Serial.print("§yt: ");Serial.print(robot_state.y, DEC);
			Serial.print("§encL: ");Serial.print(value_left_enc);
			Serial.print("§encR: ");Serial.println(value_right_enc);*/
			break;
		}
		

		default:
		{
			sendMessage(id,E_INVALID_CMD);
			break;
		}
	}
}
