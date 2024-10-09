/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Jorge Ignacio Tapia (jorge.tapia@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <led.h>
#include <switch.h>
#include <hc_sr04.h>
#include <lcditse0803.h>

/*==================[macros and definitions]=================================*/

#define CONFIG_MEASURE_PERIOD 1000
#define CONFIG_LED_PERIOD 1000
#define CONFIG_LCD_PERIOD 1000
#define CONFIG_SWITCH_PERIOD 200
#define first_measurement_value 10
#define second_measurement_value 20
#define third_measurement_value 30

/*==================[internal data definition]===============================*/

bool MEASURE_DISTANCE = false;
bool hold_measurement = false;

uint16_t distance = 0;

TaskHandle_t measure_distance_task_handle = NULL;

TaskHandle_t turnon_LEDs_distance_task_handle = NULL;

TaskHandle_t show_distance_LCD_task_handle = NULL;

TaskHandle_t read_switches_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

/**
 * @brief add description
*/

static void measure_distance(void *pvParameter)
{
	while(true)
	{
		if (MEASURE_DISTANCE)
			{
			distance = HcSr04ReadDistanceInCentimeters();
			}
		vTaskDelay(CONFIG_MEASURE_PERIOD / portTICK_PERIOD_MS);
	}
}

/**
 * @brief add description
*/
static void turnon_LEDs_distance(void *pvParameter)
{

	while(true)
		{
		if (MEASURE_DISTANCE)
			{
			if (distance < first_measurement_value)
				{
				LedOff(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
				}

			else if (first_measurement_value <= distance && distance < second_measurement_value)
				{
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
				}

			else if(second_measurement_value <= distance && distance < third_measurement_value)
				{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
				}

			else 
				{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);
				}
			}

		else
			{
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
			}

		vTaskDelay(CONFIG_LED_PERIOD / portTICK_PERIOD_MS);

	}	
}

/**
 * @brief Add description
*/

void static show_distance_LCD(void *pvParameter)
{
	while(true)
	{	
		if(MEASURE_DISTANCE)
			{
			if(!hold_measurement)
				{
				LcdItsE0803Write(distance);
				}
			}

		else
			{ 
			LcdItsE0803Write(0);
			}
		vTaskDelay(CONFIG_LCD_PERIOD / portTICK_PERIOD_MS);
	}
}

/**
 * @brief Add description
*/

void static read_switches(void *pvParameter)
{
	while(true)
	{
	uint8_t keys = SwitchesRead();
		switch (keys)
		{
			case SWITCH_1:
				MEASURE_DISTANCE = !MEASURE_DISTANCE;
			break;

			case SWITCH_2:
				hold_measurement = !hold_measurement;
			break;
		}
	vTaskDelay(CONFIG_SWITCH_PERIOD / portTICK_PERIOD_MS); 

	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();

	xTaskCreate(&read_switches, "Leer_Teclas", 512, NULL, 5, &read_switches_task_handle);
	xTaskCreate(&measure_distance, "Medir_Distancia", 512, NULL, 5, &measure_distance_task_handle);
	xTaskCreate(&turnon_LEDs_distance, "Encender/Apagar_Leds", 512, NULL, 5, &turnon_LEDs_distance_task_handle);
	xTaskCreate(&show_distance_LCD, "Mostrar_distancia_por_LCD", 512, NULL, 5, &show_distance_LCD_task_handle);


}
/*==================[end of file]============================================*/