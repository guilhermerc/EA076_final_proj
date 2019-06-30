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
#include <MMA1.h>
#include <stdint.h>
#include <stdlib.h>
#include <TI2.h>
#include <Ultrasonic.h>
#include <WAIT1.h>

/* User includes (#include below this line is not maintained by Processor Expert) */

enum ORIENTATION
{
	HORIZONTAL_UPWARDS,
	HORIZONTAL_DOWNWARDS,
	LATERAL,
	VERTICAL
};

#define GRAVITY_THRESHOLD	2000

#define POSITIVE_LOWER_THRESHOLD	1
#define POSITIVE_UPPER_THRESHOLD	5
#define NEGATIVE_LOWER_THRESHOLD	25
#define NEGATIVE_UPPER_THRESHOLD	30
#define PARASITIC_ECHO_AVOIDANCE	50
#define AMBIENT_TEMPERATURE	22

/* TODO: Read about the KY-038 sensor
 * TODO: Test if it's possible to connect to the MQTT Broker hosted in my
 * computer using a smartphone
 */

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */
	enum ORIENTATION last_orientation = -1;
	uint16_t distance = 0;

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
	TI2_Enable();

	MMA1_Init();
	US_Init();

	/*!
	 * Infinite loop that checks if the event ring buffer has events to
	 * be handled.
	 */
	for(;;)
	{
	    if(abs(MMA1_GetX()) >= GRAVITY_THRESHOLD &&
	    		last_orientation != VERTICAL)
	    {
	    	last_orientation = VERTICAL;
	    	event_buff_insert_event(ORIENTATION_CHANG_TO_VER);
	    }
	    else if(abs(MMA1_GetY()) >= GRAVITY_THRESHOLD &&
	    		last_orientation != LATERAL)
	    {
	    	last_orientation = LATERAL;
	    	event_buff_insert_event(ORIENTATION_CHANG_TO_LAT);
	    }
	    else if(MMA1_GetZ() >= GRAVITY_THRESHOLD &&
	    		last_orientation != HORIZONTAL_UPWARDS)
	    {
	    	last_orientation = HORIZONTAL_UPWARDS;
	    	event_buff_insert_event(ORIENTATION_CHANG_TO_HOR_UP);
	    }
	    else if(MMA1_GetZ() <= -GRAVITY_THRESHOLD &&
	    		last_orientation != HORIZONTAL_DOWNWARDS)
	    {
	    	last_orientation = HORIZONTAL_DOWNWARDS;
	    	event_buff_insert_event(ORIENTATION_CHANG_TO_HOR_DOWN);
	    }

	    distance = US_usToCentimeters(US_Measure_us(),
	    		AMBIENT_TEMPERATURE);

	    if(distance >= POSITIVE_LOWER_THRESHOLD && distance <=
	    		POSITIVE_UPPER_THRESHOLD)
	    	event_buff_insert_event(POSITIVE_ACCELERATION);
	    else if(distance >= NEGATIVE_LOWER_THRESHOLD && distance <=
	    		NEGATIVE_UPPER_THRESHOLD)
	    	event_buff_insert_event(NEGATIVE_ACCELERATION);

	    WAIT1_Waitms(PARASITIC_ECHO_AVOIDANCE);

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
} /*** 	End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

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
