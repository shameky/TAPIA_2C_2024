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
#include <timer_mcu.h>
#include <uart_mcu.h>
#include <analog_io_mcu.h>
#include <switch.h>
#include <led.h>
#include <timer_mcu.h>

/*==================[macros and definitions]=================================*/

#define CONFIG_MEASURE_PERIOD 2000
#define BUFFER_SIZE 256
#define ECG_FREQUENCY 3900
#define BAUD_RATE 115200
#define GSR_TRESHOLD 250

/*==================[internal data definition]===============================*/

TaskHandle_t covert_digital_task_handle = NULL;
TaskHandle_t turnon_LEDs_GSR_task_handle = NULL;
uint16_t reading = 0;
bool measure_reading = false;

/*==================[internal functions declaration]=========================*/
void FuncTimerA(void* param){

    vTaskNotifyGiveFromISR(covert_digital_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(turnon_LEDs_GSR_task_handle, pdFALSE); 

}

static void convert_digital(void *param){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputReadSingle(CH1, &reading);
		UartSendString(UART_PC, (char *)UartItoa(reading, 10));
		UartSendString(UART_PC, "\r" );
	}
}

static void turnon_LEDs_GSR(void *pvParameter)
{ 
	while(true)
	{	
		if (measure_reading)
			{
				if(reading < GSR_TRESHOLD)
				{	
                    LedOn(LED_1);
                }
				else {
						LedToggle(LED_2);
						LedOff(LED_1);
					 }
					
			}
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}	
}

void static read_switches(void *pvParameter)
{
    bool *flags = (bool*) pvParameter;
	*flags = !*flags;
}

/*==================[external functions definition]==========================*/
void app_main(void){

	analog_input_config_t analog_input ={
		.input = CH1,
		.mode = ADC_SINGLE,
	};
	
	AnalogInputInit(&analog_input);
	AnalogOutputInit();

	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = BAUD_RATE,
		.func_p = NULL,
		.param_p = NULL,
	};

	UartInit(&serial_pc);

	timer_config_t timer_measurement = {
    	.timer = TIMER_A,
        .period = CONFIG_MEASURE_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

	LedsInit();
	SwitchesInit();

	SwitchActivInt(SWITCH_1, read_switches, &measure_reading);
	SwitchActivInt(SWITCH_2, LedsOffAll, NULL);

	TimerInit(&timer_measurement);

	xTaskCreate(&convert_digital, "Convertir señal a Digital", 1024, NULL, 5, &covert_digital_task_handle);
	xTaskCreate(&turnon_LEDs_GSR, "Prender los LEDs segun los eventos", 1024, NULL, 5, &turnon_LEDs_GSR_task_handle);

	TimerStart(timer_measurement.timer);

	}
/*==================[end of file]============================================*/