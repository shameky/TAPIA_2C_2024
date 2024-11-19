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
 * | 	LCD 	 	| 	GPIO_19     |
 * |				|	GPIO_18		|
 * |				|	GPIO_9		|
 * |	HC-SR04		|	GPIO_03		|
 * |				|	GPIO_02		|
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
#include <timer_mcu.h>

/*==================[macros and definitions]=================================*/

#define CONFIG_MEASURE_PERIOD 1000000
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
 * @brief Timer callback function to give a task notification.
 */
void FuncTimerA(void *pvParameter)
{
    vTaskNotifyGiveFromISR(measure_distance_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(show_distance_LCD_task_handle, pdFALSE); 
	vTaskNotifyGiveFromISR(turnon_LEDs_distance_task_handle, pdFALSE);
}


/**
 * @brief Task to measure distance using HC-SR04 sensor
 * @param pvParameter Pointer to task parameters
*/
static void measure_distance(void *pvParameter)
{
	while (true)
    {   
	    if (MEASURE_DISTANCE)
            {
	        distance = HcSr04ReadDistanceInCentimeters();
            }
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

/**
 * @brief Function that turns on the LEDs according to the distance measurement.
 * 		  If distance < 10cm, all the LEDs turn off.
 * 		  If 10<=distance<20, LED 1 turns on.
 * 		  If 20<=distance<30, LEDs 1 and 2 light up.
 * 		  If distance => 30 , all the LEDs light up.
 * @param pvParameter Pointer to task parameters
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
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}	
}


/**
 * @brief Task to show distance in LCD.
 * @param pvParameter Pointer to task parameters.
*/
void static show_distance_LCD(void *pvParameter)
{
	while(true)
    {
	    if(MEASURE_DISTANCE)
        {

		    if (!hold_measurement)
                {
			    LcdItsE0803Write(distance);
	            }
        }
	    else
            { 
		    LcdItsE0803Write(0);
            }
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

/**
 * @brief Task to read switches input
 * @param pvParameter Pointer to task parameters
 */

void static read_switches(void *pvParameter)
{
    bool *flags = (bool*) pvParameter;
	*flags = !*flags;
}
 
/*==================[external functions definition]==========================*/
void app_main(void)
{
	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();

	SwitchActivInt(SWITCH_1, read_switches, &MEASURE_DISTANCE);
	SwitchActivInt(SWITCH_2, read_switches, &hold_measurement);

	timer_config_t timer_tareas = 
    {
    	.timer = TIMER_A,
        .period = CONFIG_MEASURE_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

    TimerInit(&timer_tareas);
	xTaskCreate(&measure_distance, "Medir_Distancia", 512, NULL, 5, &measure_distance_task_handle);
	xTaskCreate(&turnon_LEDs_distance, "Encender/Apagar_Leds", 512, NULL, 5, &turnon_LEDs_distance_task_handle);
	xTaskCreate(&show_distance_LCD, "Mostrar_distancia_por_LCD", 512, NULL, 5, &show_distance_LCD_task_handle);
	TimerStart(timer_tareas.timer);


}
/*==================[end of file]============================================*/