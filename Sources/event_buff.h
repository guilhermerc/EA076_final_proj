/*!
 * @file event_buff.h
 * @brief This file exports what is necessary from event_buff.c
 *
 * @author Guilherme R C <guilherme.riciolic@gmail.com>
 */

#ifndef SOURCES_EVENT_BUFF_H_
#define SOURCES_EVENT_BUFF_H_

#include <PE_Types.h>

#define EVENT_BUFF_SIZE	32

typedef enum
{
	NEW_MESSAGE_FROM_BROKER,
	NEW_MESSAGE_FROM_TERMINAL,
	SINGLE_FINGER_SNAPPING,		/// These 4 events are the ones that
	DOUBLE_FINGER_SNAPPING,		/// actually puts the finite state
	POSITIVE_ACCELERATION,		/// machine into an acceptable state
	NEGATIVE_ACCELERATION,		/// (a state that publishes a command)
	ORIENTATION_CHANG_TO_HOR_UP,
	ORIENTATION_CHANG_TO_HOR_DOWN,
	ORIENTATION_CHANG_TO_LAT,
	ORIENTATION_CHANG_TO_VER
} EVENT_BUFF_TYPE;

typedef struct
{
	EVENT_BUFF_TYPE ring_buff[EVENT_BUFF_SIZE];
	/*!
	 * The 'head pointer' will point to the next possibly available
	 * slot, while the 'tail_pointer' will point to next event to be
	 * consumed
	 */
	uint8_t head_ptr;
	uint8_t tail_ptr;
} EVENT_BUFF_INFO;

EVENT_BUFF_INFO event_buff_info;

void event_buff_init();
bool event_buff_is_empty();
bool event_buff_is_full();
void event_buff_insert_event(EVENT_BUFF_TYPE type);
EVENT_BUFF_TYPE event_buff_consume_event();
void event_handler(EVENT_BUFF_TYPE event);

#endif /* SOURCES_EVENT_BUFFER_H_ */
