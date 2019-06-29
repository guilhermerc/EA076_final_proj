/*
 * FSM.h
 *
 *  Created on: 29/06/2019
 *      Author: guilherme
 */

#ifndef SOURCES_FSM_H_
#define SOURCES_FSM_H_

typedef enum FSM_STATE
{
	HORIZONTAL_UPWARDS_IDLE,
	HORIZONTAL_DOWNWARDS_IDLE,
	LATERAL_IDLE,
	VERTICAL_IDLE_ZOOM_MODE,
	VERTICAL_IDLE_CALLING_MODE
} FSM_STATE;

void fsm_set_state(FSM_STATE state);
void fsm_iterate(EVENT_BUFF_TYPE event);

#endif /* SOURCES_FSM_H_ */
