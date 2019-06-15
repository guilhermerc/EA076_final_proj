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
	NEW_MESSAGE_FROM_TERMINAL
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
