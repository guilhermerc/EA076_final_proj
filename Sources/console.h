/*!
** @file console.h
** @brief This file exports what is necessary from console.c
**
** @author Guilherme R C <guilherme.riciolic@gmail.com>
*/

#ifndef SOURCES_CONSOLE_H_
#define SOURCES_CONSOLE_H_

#include <comm.h>
#include <UART0.h>

#define CONSOLE_BUFFER_SIZE	512

typedef struct
{
	COMM_SENDING_STATUS status;
	UART0_TComData buffer[CONSOLE_BUFFER_SIZE];
} CONSOLE_INFO_STRUCT;

CONSOLE_INFO_STRUCT console_info;

void console_init();
void console_write(char * tag, UART0_TComData * message);

#endif /* SOURCES_CONSOLE_H_ */
