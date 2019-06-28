/* ###################################################################
 **     Filename    : main.c
 **     Project     : EA076_D
 **     Processor   : MKL25Z128VLK4
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2019-03-18, 16:31, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 01.01
 ** @brief
 **         This module contains user's application code.
 **
 ** @author Guilherme R C <guilherme.riciolic@gmail.com>
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */

#include <comm.h>
#include <console.h>
#include <CPU.h>
#include <event_buff.h>
#include <KY_038.h>
#include <LEDR.h>
#include <LEDG.h>
#include <LEDB.h>
#include <MMA1.h>
#include <stdint.h>
#include <stdlib.h>
#include <TI1.h>

/* User includes (#include below this line is not maintained by Processor Expert) */

/* TODO: Read about the KY-038 sensor
 * TODO: Test if it's possible to connect to the MQTT Broker hosted in my
 * computer using a smartphone
 */

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */
	/* For example: for(;;) { } */

	/*!
	 * Initializing the modules
	 */
	event_buff_init();

	console_init();
	comm_init();

	KY_038_Enable();
	TI1_Enable();

	MMA1_Init();

	/*!
	 * Infinite loop that checks if the event ring buffer has events to
	 * be handled.
	 */
	for(;;)
	{
		int16_t x = 0,
				y = 0,
				z = 0;

        x = MMA1_GetX();
        y = MMA1_GetY();
        z = MMA1_GetZ();

        LEDR_Put(abs(x) > 2000);
        LEDG_Put(abs(y) > 2000);
        LEDB_Put(abs(z) > 2000);

		if(!event_buff_is_empty())
			event_handler(event_buff_consume_event());

	}

	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.5 [05.21]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
