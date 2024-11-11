/*! @mainpage Parcial - Recuperatorio
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|	
 * | 	HcSr04	 	| 	GPIO_3, GPIO_2	|
 * | 	Barrera 	| 	GPIO_8		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 11/11/2024 | Document creation		                         |
 *
 * @author Jorge Ignacio Tapia (jorge.tapia@ingeneiria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <led.h>
#include <hc_sr04.h>
#include <timer_mcu.h>

/*==================[macros and definitions]=================================*/

#define CONFIG_MEASURE_PERIOD 10000

#define first_measurement_value 1000

#define velocidad_1 0
#define velocidad_2 8

#define CONFIG_MEASURE_PERIOD2 5000

#define BAUD_RATE 115200

/*==================[internal data definition]===============================*/

bool MEASURE_DISTANCE = false;
bool hold_measurement = false;

uint16_t distance = 0;

TaskHandle_t measure_distance_task_handle = NULL;

TaskHandle_t verificar_velocidad_task_handle = NULL;

TaskHandle_t covert_digital_task_handle = NULL;


/*==================[internal functions declaration]=========================*/

/*==================[internal functions declaration]=========================*/

/**
 * @brief Task to measure distance using HC-SR04 sensor
 * @param pvParameter Pointer to task parameters
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
 * @brief Verifica la velocidad del camion a partir de los 10 metros
 * 		  si velocidad = 0, se prende el LED 1
 *  	  si velocidad > 8, se prende el LED 3
 * 	  	  si 0 < velocidad < 8, se prende el LED 2
 * @param pvParameter Pointer to task parameters
*/
static void verificar_velocidad(void *pvParameter)
{

	while(true)
		{
		if (MEASURE_DISTANCE)
			{
			if (distance < first_measurement_value)
			{
				uint16_t velocidad = 0;
				velocidad = distance / CONFIG_MEASURE_PERIOD ;

				if(velocidad == velocidad_1)
				{
                    LedOn(LED_1);
                }
				else LedOff(LED_1);

				if(velocidad > velocidad_2)
				{
                    LedOn(LED_3);
                }
				else LedOff(LED_3);

				if(velocidad_1 <= velocidad <= velocidad_2)
				{
                    LedOn(LED_2);
                }
				else LedOff(LED_2);
			}
			
			}

		vTaskDelay(CONFIG_MEASURE_PERIOD / portTICK_PERIOD_MS);

	}	
}

/**
 * @brief This function is an interrupt service routine (ISR) for Timer B.
 * It notifies the 'convert_analog' task to perform its task using a task notification.
 *
 * @param param A pointer to the parameters passed to the ISR. In this case, it is not used.
 *
 * @return void.
 *
 */
void FuncTimerB(void* param){

    vTaskNotifyGiveFromISR(convert_analog_task_handle, pdFALSE);   

}

/**
 * @brief This function convert analog values to digital values and calculate velocities and weigh.
 *
 * @param param A pointer to the parameters passed to the function. In this case, it is not used.
 *
 * @return void.
 */
static void convert_digital(void *param){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		uint16_t reading1 = 0;
		uint16_t reading2 = 0;

		if(distance == 0)
		{	
			for(int i = 0; i <49, i++)
			{
				AnalogInputReadSingle(CH1, &reading1);
				reading1 += reading1;
				
				AnalogInputReadSingle(CH2, &reading2);
				reading2 += reading2;
			}
			prom1 = reading1 / 50 ;
			prom2 = reading2 / 50 ;
			peso = prom1 + prom2;
		}
		
		UartSendString(UART_PC, (char *)UartItoa(peso, 10));
		UartSendString(UART_PC, "Peso: \r" );
		UartSendString(UART_PC, (char *)UartItoa(distance / CONFIG_MEASURE_PERIOD, 10));
		UartSendString(UART_PC, "Velocidad: \r" );

		vTaskDelay(CONFIG_MEASURE_PERIOD2 / portTICK_PERIOD_MS);
	}
}


/**
 * @brief This function reads a byte from the specified serial port and performs specific actions based on the received byte.
 *
 * @param pvParameter A pointer to the parameters passed to the function. In this case, it is not used.
 *
 * @return void.
 */
void read_serial_port(void *pvParameter)
{
	uint8_t reading; 
	UartReadByte(UART_PC, &reading);

	if (reading == 'o')
        {
		    GPIO_8(1);
	    }

	else if (reading == 'c')
        {
		    GPIO_8(0);
	    } 

}


/*==================[external functions definition]==========================*/
void app_main(void)
{
	LedsInit();
	HcSr04Init(GPIO_3, GPIO_2);
	AnalogInputInit(&analog_input);
	BarreraInit(GPIO_8);

	analog_input_config_t analog_input ={
		.input = CH1,
		.mode = ADC_SINGLE,
	};

	analog_input_config_t analog_input ={
		.input = CH2,
		.mode = ADC_SINGLE,
	};

	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = BAUD_RATE,
		.func_p = NULL,
		.param_p = NULL,
	};

	UartInit(&serial_pc);

	xTaskCreate(&measure_distance, "Medir_Distancia", 512, NULL, 5, &measure_distance_task_handle);
	xTaskCreate(&verificar_velocidad, "Verificar la velocidad del camion", 512, NULL, 5, &verificar_velocidad_task_handle);
	xTaskCreate(&convert_digital, "Convertir señal a Digital", 1024, NULL, 5, &covert_digital_task_handle);
}
/*==================[end of file]============================================*/