/*!
 * @file comm.c
 * @brief This file contains the function implementations related to
 * the communication.
 *
 * @author Guilherme R C <guilherme.riciolic@gmail.com>
 */

#include <comm.h>
#include <console.h>
#include <Events.h>
#include <stdint.h>
#include <string.h>
#include <UART2.h>

#define WIFI_SSID "\"EA076R\""
#define WIFI_PASSWORD "\"FRDMKL25\""

#define MQTT_IP "\"192.168.1.111\""
#define MQTT_PORT "1883"
#define MQTT_USERNAME "\"aluno\""
#define MQTT_PASSWORD "\"UNICAMP\""

#define DIR_TOPIC "\"EA076/grupoD3/dir\""
#define POWER_TOPIC "\"EA076/grupoD3/power\""
#define MODE_TOPIC "\"EA076/grupoD3/mode\""
#define THRESHOLD_TOPIC "\"EA076/grupoD3/threshold\""

#define DIR_TOPIC_WQ "EA076/grupoD3/dir"
#define POWER_TOPIC_WQ "EA076/grupoD3/power"
#define MODE_TOPIC_WQ "EA076/grupoD3/mode"
#define THRESHOLD_TOPIC_WQ "EA076/grupoD3/threshold"

#define TIME_ADJUSTMENT_TOPIC "\"EA076/grupoD3/adjust\""
#define TIME_ADJUSTMENT_TOPIC_WQ "EA076/grupoD3/adjust"

#define COMMAND_TOPIC "\"EA076/grupoD3/command\""
#define COMMAND_TOPIC_WQ "EA076/grupoD3/command"

#define LOG_TOPIC_WQ "EA076/grupoD3/log"

#define MAX_TOKENS	32

#define CMD_TYPE_INDEX	0
#define TOPIC_INDEX	1
#define MESSAGE_INDEX	2

#define IP_NUMBER_SIZE	32
#define MAC_ADDR_SIZE	64

static char ip_number[IP_NUMBER_SIZE];	/**< TODO: It should be const */
static char mac_addr[MAC_ADDR_SIZE];	/**< TODO: IT should be const */

typedef enum
{
	SUBSCRIBING_TO_DIR_TOPIC,
	SUBSCRIBING_TO_POWER_TOPIC,
	SUBSCRIBING_TO_MODE_TOPIC,
	SUBSCRIBING_TO_THRESHOLD_TOPIC,
	SUBSCRIBING_TO_ADJUST_TOPIC,
	SUBSCRIBING_TO_COMMAND_TOPIC,
	CONCLUDED
} SUBSCRIPTIONS_STATE_ENUM;

static SUBSCRIPTIONS_STATE_ENUM subscriptions_state;
static UART2_TComData * tokens[MAX_TOKENS];

void comm_response();
bool comm_are_there_conn_errors();

/*! @brief A function starts the communication with ESP01
 *
 * This function is responsible for starting the communication
 * with ESP01 (consequently with the MQTT broker).
 */
void comm_init()
{
	subscriptions_state = SUBSCRIBING_TO_DIR_TOPIC;

	comm_info.state = CONNECT_WIFI;
	comm_info.message_received = FALSE;
	console_info.status = DONE;
	comm_info.sending_status = DONE;

	comm_clear_input_buffer();
	comm_response();
}

/*! @brief A function that checks wether the FSM that handles the
 * communication is available.
 */
COMM_STATUS comm_status()
{
	COMM_STATUS comm_status = BUSY;
	if((comm_info.sending_status == DONE) && (console_info.status == DONE))
		comm_status = AVAILABLE;

	return comm_status;
}

/*! @brief A function that triggers the message sending process (to
 * ESP01)
 *
 * 	This function is responsible for triggering the sending message
 * 	process (to ESP01).
 */
void comm_send_msg()
{
	comm_info.sending_status = SENDING;
	UART2_OnTxChar();
}

/*! @brief A function that clears the communication input buffer */
void comm_clear_input_buffer()
{
	memset(comm_info.message_in, 0, MESSAGE_BUFFER_SIZE);
}

/*! @brief A function that responds to a received msg
 *
 * This function is responsible for assembling a response to the last
 * received message from the MQTT broker. It also implements connection
 * retries.
 */
void comm_response()
{
	if(comm_info.state == WAITING_FOR_CMD)	return;

	switch(comm_info.state)
	{
	case CONNECT_WIFI:
	{
		strcpy(comm_info.message_out, "CONNWIFI ");
		strcat(comm_info.message_out, WIFI_SSID);
		strcat(comm_info.message_out, ",");
		strcat(comm_info.message_out, WIFI_PASSWORD);
		strcat(comm_info.message_out, TERMINATING_CHARS);

		break;
	}
	case GET_IP_NUMB:
	{
		strcpy(comm_info.message_out, "GETIP");
		strcat(comm_info.message_out, TERMINATING_CHARS);

		break;
	}
	case GET_MAC_ADDR:
	{
		strcpy(comm_info.message_out, "GETMAC");
		strcat(comm_info.message_out, TERMINATING_CHARS);

		break;
	}
	case CONNECT_MQTT:
	{
		strcpy(comm_info.message_out, "CONNMQTT ");
		strcat(comm_info.message_out, MQTT_IP);
		strcat(comm_info.message_out, ",");
		strcat(comm_info.message_out, MQTT_PORT);
		strcat(comm_info.message_out, ",");
		strcat(comm_info.message_out, mac_addr);
		strcat(comm_info.message_out, ",");
		strcat(comm_info.message_out, MQTT_USERNAME);
		strcat(comm_info.message_out, ",");
		strcat(comm_info.message_out, MQTT_PASSWORD);
		strcat(comm_info.message_out, TERMINATING_CHARS);

		break;
	}
	case TOPIC_SUBSC:
	{
		switch(subscriptions_state)
		{
		case SUBSCRIBING_TO_DIR_TOPIC:
		{
			strcpy(comm_info.message_out, "SUBSCRIBE ");
			strcat(comm_info.message_out, DIR_TOPIC);
			strcat(comm_info.message_out, TERMINATING_CHARS);
			break;
		}
		case SUBSCRIBING_TO_POWER_TOPIC:
		{
			strcpy(comm_info.message_out, "SUBSCRIBE ");
			strcat(comm_info.message_out, POWER_TOPIC);
			strcat(comm_info.message_out, TERMINATING_CHARS);
			break;
		}
		case SUBSCRIBING_TO_MODE_TOPIC:
		{
			strcpy(comm_info.message_out, "SUBSCRIBE ");
			strcat(comm_info.message_out, MODE_TOPIC);
			strcat(comm_info.message_out, TERMINATING_CHARS);
			break;
		}
		case SUBSCRIBING_TO_THRESHOLD_TOPIC:
		{
			strcpy(comm_info.message_out, "SUBSCRIBE ");
			strcat(comm_info.message_out, THRESHOLD_TOPIC);
			strcat(comm_info.message_out, TERMINATING_CHARS);
			break;
		}
		case SUBSCRIBING_TO_ADJUST_TOPIC:
		{
			strcpy(comm_info.message_out, "SUBSCRIBE ");
			strcat(comm_info.message_out, TIME_ADJUSTMENT_TOPIC);
			strcat(comm_info.message_out, TERMINATING_CHARS);
			break;
		}
		case SUBSCRIBING_TO_COMMAND_TOPIC:
		{
			strcpy(comm_info.message_out, "SUBSCRIBE ");
			strcat(comm_info.message_out, COMMAND_TOPIC);
			strcat(comm_info.message_out, TERMINATING_CHARS);
			break;
		}
		}
		break;
	}
	case PUBLISHING:
	{
		strcpy(comm_info.message_out, comm_info.message_in);
		/*!
		 * TODO: DEBUGGING PURPOSES!! REMOVE IT BEFORE SUBMITTING!
		 *
		comm_info.state = WAITING_FOR_CMD;
		 *
		 */
		break;
	}
	}

	comm_send_msg();
}

/*! @brief A function that parses a received msg
 *
 * This function is responsible for parsing a received message and
 * change the state of the FSM (if needed).
 */
void comm_parse()
{
	static bool has_ip_number = FALSE;
	static bool has_mac_addr = FALSE;

	if(comm_are_there_conn_errors())	return;

	switch(comm_info.state)
	{
	case CONNECT_WIFI:
	{
		if(strcmp(comm_info.message_in, "CONNECT WIFI\r\n") == 0)
		{
			if(has_ip_number != TRUE)
				comm_info.state = GET_IP_NUMB;
			else if(has_mac_addr != TRUE)
				comm_info.state = GET_MAC_ADDR;
			else
				comm_info.state = CONNECT_MQTT;

		}
		else if(strcmp(comm_info.message_in, "ERROR WIFI\r\n") == 0)
		{
			comm_info.state = CONNECT_WIFI;
		}

		break;
	}
	case GET_IP_NUMB:
	{
		if(strcmp(comm_info.message_in, "NOIP\r\n") == 0)
		{
			comm_info.state = GET_IP_NUMB;
		}
		else
		{
			strcpy(ip_number, "\"");
			strcat(ip_number, comm_info.message_in);
			/*!
			 * The index which corresponds to the '\r' char is
			 * overwritten with '\"'
			 */
			ip_number[strlen(comm_info.message_in) - 1] = '\"';
			/*!
			 * The index which corresponds to the '\n' char is overwritten with '\0'
			 */
			ip_number[strlen(comm_info.message_in)] = '\0';
			has_ip_number = TRUE;
			comm_info.state = GET_MAC_ADDR;
		}

		break;
	}
	case GET_MAC_ADDR:
	{
		strcpy(mac_addr, "\"");
		strcat(mac_addr, comm_info.message_in);
		/*!
		 * The index which corresponds to the '\r' char is
		 * overwritten with '\"'
		 */
		mac_addr[strlen(comm_info.message_in) - 1] = '\"';
		/*!
		 * The index which corresponds to the '\n' char is overwritten with '\0'
		 */
		mac_addr[strlen(comm_info.message_in)] = '\0';
		has_mac_addr = TRUE;
		comm_info.state = CONNECT_MQTT;

		break;
	}
	case CONNECT_MQTT:
	{
		if(strcmp(comm_info.message_in, "CONNECT MQTT\r\n") == 0)
		{
			if(subscriptions_state != CONCLUDED)
				comm_info.state = TOPIC_SUBSC;
			else
				comm_info.state = WAITING_FOR_CMD;
		}
		else if(strcmp(comm_info.message_in, "NOWIFI\r\n") == 0)
		{
			comm_info.state = CONNECT_WIFI;
		}
		else if(strcmp(comm_info.message_in, "ERROR: -1\r\n") == 0 ||
				strcmp(comm_info.message_in, "ERROR: -2\r\n") == 0 ||
				strcmp(comm_info.message_in, "ERROR: -3\r\n") == 0 ||
				strcmp(comm_info.message_in, "ERROR: -4\r\n") == 0 ||
				strcmp(comm_info.message_in, "MQTT_DISCONNECTED\r\n") == 0)
		{
			comm_info.state = CONNECT_MQTT;
		}

		break;
	}
	case WAITING_FOR_CMD:
	{
		UART2_TComData message_in_copy[strlen(comm_info.message_in) + 1];
		strcpy(message_in_copy, comm_info.message_in);

		uint8_t curr_token_idx = 0;

		tokens[curr_token_idx++] = strtok(message_in_copy, " ");


		if((strcmp(tokens[CMD_TYPE_INDEX], "MESSAGE") == 0))
		{
			tokens[curr_token_idx++] = strtok(NULL, " [],");
			/**< The message cannot contain '[', ']' or ',' */
			tokens[curr_token_idx++] = strtok(NULL, "[],");

			if(strcmp(tokens[TOPIC_INDEX], DIR_TOPIC_WQ) == 0)
			{
				/*!
				 * The motor direction can only be changed when in 'ON'
				 * mode.
				 */
				/*
				if(motor_info.current_mode == ON)
				{
					if(strcmp(tokens[MESSAGE_INDEX], "CLOCKWISE") == 0)
					{
						motor_set_dir(CLOCKWISE);
					}
					else if(strcmp(tokens[MESSAGE_INDEX], "COUNTERCLOCKWISE") == 0)
					{
						motor_set_dir(COUNTERCLOCKWISE);
					}
				}
				*/
			}
			else if(strcmp(tokens[TOPIC_INDEX], POWER_TOPIC_WQ) == 0)
			{
				/*!
				 * The motor PWM can only be changed when in 'ON' mode.
				 */
				/*
				if(motor_info.current_mode == ON)
				{
					uint8_t percentage = (uint8_t) atoi(tokens[MESSAGE_INDEX]);
					motor_set_pwm(percentage * MAXIMUM_PWM/100);
				}
				*/
			}
			else if(strcmp(tokens[TOPIC_INDEX], MODE_TOPIC_WQ) == 0)
			{
				/*
				if(strcmp(tokens[MESSAGE_INDEX], "ON") == 0)
				{
					motor_set_mode(ON);
				}
				else if(strcmp(tokens[MESSAGE_INDEX], "OFF") == 0)
				{
					motor_set_mode(OFF);
				}
				else if(strcmp(tokens[MESSAGE_INDEX], "AUTO") == 0)
				{
					motor_set_mode(AUTO);
				}
				*/
			}
			else if(strcmp(tokens[TOPIC_INDEX], THRESHOLD_TOPIC_WQ) == 0)
			{
				/*
				float threshold = (float) atof(tokens[MESSAGE_INDEX]);
				motor_set_threshold(threshold);
				*/
			}
			else if(strcmp(tokens[TOPIC_INDEX], TIME_ADJUSTMENT_TOPIC_WQ) == 0)
			{
				/*
				struct tm time;
				strptime(tokens[MESSAGE_INDEX], "%T", &time);
				rtc_set_current_time(time.tm_sec, time.tm_min, time.tm_hour);
				*/
			}
			else if(strcmp(tokens[TOPIC_INDEX], COMMAND_TOPIC_WQ) == 0)
			{
				/*
				if(strcmp(tokens[MESSAGE_INDEX], "Dump") == 0)
					event_buff_insert_event(MEMORY_DUMP_REQUEST);
				*/
			}
		}
		else if((strcmp(tokens[0], "PUBLISH") == 0))
		{
			comm_info.state = PUBLISHING;
		}

		break;
	}
	case TOPIC_SUBSC:
	{
		if(strcmp(comm_info.message_in, "OK SUBSCRIBE\r\n") == 0)
		{
			/*!
			 * Moving to the next topic to subscribe (or to 'CONCLUDED')
			 */
			subscriptions_state++;

			if(subscriptions_state == CONCLUDED)
			{
				comm_info.state = WAITING_FOR_CMD;
				console_write("COMMUNICATION", "Communication established with success!\n");
			}
		}
		else if(strcmp(comm_info.message_in, "NOT CONNECTED\r\n") == 0)
		{
			comm_info.state = CONNECT_MQTT;
		}
		else if(strcmp(comm_info.message_in, "ERROR SUBSCRIBE\r\n") == 0)
		{
			comm_info.state = WAITING_FOR_CMD;
		}

		break;
	}
	case PUBLISHING:
	{
		if(strcmp(comm_info.message_in, "OK PUBLISH\r\n") == 0)
		{
			comm_info.state = WAITING_FOR_CMD;
		}
		else if(strcmp(comm_info.message_in, "NOT CONNECTED\r\n") == 0)
		{
			comm_info.state = CONNECT_MQTT;
		}
		else if(strcmp(comm_info.message_in, "ERROR SUBSCRIBE\r\n") == 0)
		{
			comm_info.state = WAITING_FOR_CMD;
		}
	}
	}
}

/*!
 * @brief A function that processes a received message into the current
 * communication context.
 */
void comm_process_msg()
{
	comm_parse();
	comm_response();
	comm_clear_input_buffer();
}

/*! @brief A function that checks if there is a connection error
 *
 * This function is responsible for checking if the message
 * received is one of the global connection errors.
 */
bool comm_are_there_conn_errors()
{
	bool status = FALSE;
	if(strcmp(comm_info.message_in, "WIFI_DISCONNECTED\r\n") == 0)
	{
		comm_info.state = CONNECT_WIFI;
		status = TRUE;
	}
	else if(strcmp(comm_info.message_in, "MQTT_DISCONNECTED\r\n") == 0)
	{
		comm_info.state = CONNECT_MQTT;
		status = TRUE;
	}

	return status;
}


/*! @brief A function that publishes a message into a topic
 *
 * @param	topic	The topic on which to publish
 * @param	message The message to be published
 */
void comm_publish(char * topic, char * message)
{
	strcpy(comm_info.message_in, "PUBLISH ");
	strcat(comm_info.message_in, topic);
	strcat(comm_info.message_in, ",\"");
	strcat(comm_info.message_in, message);
	strcat(comm_info.message_in, "\"");
	strcat(comm_info.message_in, TERMINATING_CHARS);

	comm_process_msg();
}
