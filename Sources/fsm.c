/*
 * FSM.c
 *
 *  Created on: 29/06/2019
 *      Author: guilherme
 */

#include <comm.h>
#include <console.h>
#include <event_buff.h>
#include <fsm.h>
#include <WAIT1.h>

#define	DEBOUNCING_TIMEOUT	700

static FSM_STATE curr_state = -1;

void fsm_set_state(FSM_STATE state)
{
	curr_state = state;

	while(comm_status() != AVAILABLE);

	switch(state)
	{
	case HORIZONTAL_UPWARDS_IDLE:
	{
		console_write("NEW STATE", "HORIZONTAL UPWARDS IDLE\n");
		break;
	}
	case HORIZONTAL_DOWNWARDS_IDLE:
	{
		console_write("NEW STATE", "HORIZONTAL DOWNWARDS IDLE\n");
		break;
	}
	case LATERAL_IDLE:
	{
		console_write("NEW STATE", "LATERAL IDLE\n");
		break;
	}
	case VERTICAL_IDLE_ZOOM_MODE:
	{
		console_write("NEW STATE", "VERTICAL IDLE ZOOM MODE\n");
		break;
	}
	}

	WAIT1_Waitms(DEBOUNCING_TIMEOUT);

}

void fsm_iterate(EVENT_BUFF_TYPE event)
{
	switch(curr_state)
	{
	case HORIZONTAL_UPWARDS_IDLE:
	{
		switch(event)
		{
		case SINGLE_FINGER_SNAPPING:
		{
			console_write("NEW EVENT", "SINGLE FINGER SNAPPING\n");
			break;
		}
		case DOUBLE_FINGER_SNAPPING:
		{
			console_write("NEW EVENT", "DOUBLE FINGER SNAPPING\n");
			break;
		}
		}
		break;
	}
	case HORIZONTAL_DOWNWARDS_IDLE:
	{
		switch(event)
		{
		case DOUBLE_FINGER_SNAPPING:
		{
			console_write("NEW EVENT", "DOUBLE FINGER SNAPPING\n");
			break;
		}
		}

		break;
	}
	case LATERAL_IDLE:
	{
		switch(event)
		{
		case POSITIVE_ACCELERATION:
		{
			break;
		}
		case NEGATIVE_ACCELERATION:
		{
			break;
		}
		}
		break;
	}
	case VERTICAL_IDLE_ZOOM_MODE:
	{
		switch(event)
		{
		case POSITIVE_ACCELERATION:
		{
			break;
		}
		case NEGATIVE_ACCELERATION:
		{
			break;
		}
		}
		break;
	}
	case VERTICAL_IDLE_CALLING_MODE:
	{
		switch(event)
		{
		case SINGLE_FINGER_SNAPPING:
		{
			console_write("NEW EVENT", "SINGLE FINGER SNAPPING\n");
			break;
		}
		case DOUBLE_FINGER_SNAPPING:
		{
			console_write("NEW EVENT", "DOUBLE FINGER SNAPPING\n");
			break;
		}
		}
		break;
	}
	}
}
