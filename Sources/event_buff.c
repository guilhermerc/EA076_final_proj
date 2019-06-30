/*!
 * @file event_buff.c
 * @brief This file contains implementations related to the event ring
 * buffer
 *
 * @author Guilherme R C <guilherme.riciolic@gmail.com>
 */

#include <comm.h>
#include <event_buff.h>
#include <fsm.h>
#include <LEDB.h>
#include <LEDG.h>
#include <LEDR.h>
#include <UART0.h>
#include <UART2.h>

typedef enum
{
	NONE, //!< NONE
	EMPTY,//!< EMPTY
	FULL  //!< FULL
} EVENT_BUFF_STATE_ENUM;

EVENT_BUFF_STATE_ENUM state;

typedef enum
{
	BROKER,
	TERMINAL
} EVENT_HANDLER_MESSAGE_ORIGIN;

/*!
 * @brief A function that initializes the event ring buffer module
 */
void event_buff_init()
{
	state = EMPTY;
	event_buff_info.head_ptr = 0;
	event_buff_info.tail_ptr = 0;
}

/*!
 * @brief A function that checks if the event ring buffer is empty
 * @return	bool	TRUE: if it is empty
 * 					FALSE: if it isn't empty
 */
bool event_buff_is_empty()
{
	return (state == EMPTY) ? TRUE : FALSE;
}

/*!
 * @brief A function that checks if the event ring buffer is full
 * @return	bool	TRUE: if it is full
 * 					FALSE: if it isn't full
 */
bool event_buff_is_full()
{
	return (state == FULL) ? TRUE : FALSE;
}

/*!
 * @brief A function that inserts an event in the event ring buffer (if
 * it is not full)
 * @param	type	The event to be inserted
 */
void event_buff_insert_event(EVENT_BUFF_TYPE event)
{
	/*!
	 * Check if state is either NONE or EMPTY
	 */
	if(!event_buff_is_full())
	{
		event_buff_info.ring_buff[event_buff_info.head_ptr] =
				event;
		event_buff_info.head_ptr++;
		event_buff_info.head_ptr %= EVENT_BUFF_SIZE;

		if(event_buff_info.head_ptr ==
				event_buff_info.tail_ptr)
		{
			state = FULL;
		}

		if(state == EMPTY)
		{
			state = NONE;
		}
	}
}

/*!
 * @brief A function that consumes an event from the event ring buffer
 * @return	EVENT_BUFF_TYPE	The event itself
 */
EVENT_BUFF_TYPE event_buff_consume_event()
{
	/*!
	 * This function is only called when ring buffer is not empty.
	 * Therefore
	 * 	1 - I don't need to check it
	 * 	2 - The state is either NONE or FULL
	 */
	EVENT_BUFF_TYPE event =
			event_buff_info.ring_buff[event_buff_info.tail_ptr];
	event_buff_info.tail_ptr++;
	event_buff_info.tail_ptr %= EVENT_BUFF_SIZE;

	if(event_buff_info.head_ptr ==
			event_buff_info.tail_ptr)
	{
		state = EMPTY;
	}

	if(state == FULL)
	{
		state = NONE;
	}

	return event;
}

/*!
 * @brief A function to read a message from a determined UART
 * @param	origin	A param that indicates from which UART ring buffer
 * 					should this function read a message
 */
void event_handler_read_message(EVENT_HANDLER_MESSAGE_ORIGIN origin)
{
	uint8_t index = 0;
	comm_info.message_in[index] = '\0';

	do {
		switch(origin)
		{
		case TERMINAL:
		{
			UART0_RecvChar(&comm_info.message_in[index]);
			break;
		}
		case BROKER:
		{
			UART2_RecvChar(&comm_info.message_in[index]);
			break;
		}
		}
	} while(comm_info.message_in[index++] != '\n');
}

/*!
 * @brief A function that handles an event
 * @param	event	The event to be handled
 */
void event_handler(EVENT_BUFF_TYPE event)
{
	/*! If the event is somewhat related to the communication process,
	 * waits (blocking) for the channel to be available
	 */
	if(event == NEW_MESSAGE_FROM_BROKER ||
			event == NEW_MESSAGE_FROM_TERMINAL)
		while(comm_status() != AVAILABLE);

	if(event == NEW_MESSAGE_FROM_BROKER)
	{
		event_handler_read_message(BROKER);
		comm_process_msg();
	}
	else if(event == NEW_MESSAGE_FROM_TERMINAL)
	{
		if(comm_info.state == WAITING_FOR_CMD)
		{
			event_handler_read_message(TERMINAL);
			comm_process_msg();
		}
	}
	else if(event == SINGLE_FINGER_SNAPPING)
	{
		fsm_iterate(SINGLE_FINGER_SNAPPING);
	}
	else if(event == DOUBLE_FINGER_SNAPPING)
	{
		fsm_iterate(DOUBLE_FINGER_SNAPPING);
	}
	else if(event == ORIENTATION_CHANG_TO_HOR_UP)
	{
		/*
		LEDR_On();
		LEDG_Off();
		LEDB_Off();
		*/

		fsm_set_state(HORIZONTAL_UPWARDS_IDLE);
	}
	else if(event == ORIENTATION_CHANG_TO_HOR_DOWN)
	{
		/*
		LEDR_On();
		LEDG_Off();
		LEDB_Off();
		*/

		fsm_set_state(HORIZONTAL_DOWNWARDS_IDLE);
	}
	else if(event == ORIENTATION_CHANG_TO_LAT)
	{
		/*
		LEDR_Off();
		LEDG_On();
		LEDB_Off();
		*/

		fsm_set_state(LATERAL_IDLE);
	}
	else if(event == ORIENTATION_CHANG_TO_VER)
	{
		/*
		LEDR_Off();
		LEDG_Off();
		LEDB_On();
		*/

		fsm_set_state(VERTICAL_IDLE_ZOOM_MODE);
	}
	else if(event == POSITIVE_ACCELERATION)
	{
		fsm_iterate(POSITIVE_ACCELERATION);
	}
	else if(event == NEGATIVE_ACCELERATION)
	{
		fsm_iterate(NEGATIVE_ACCELERATION);
	}
}
