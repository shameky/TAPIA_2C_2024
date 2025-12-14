/*! @mainpage Osciloscopie
 *
 * @section genDesc General Description
 *
 * Program allows you to read an analog signal and convert it to digital.
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	CH1 	 	| 	GPIO1		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Creación del Documento                         |
 * | 			| Finalizacion y Documentacion					 |
 *
 * @author Jorge Ignacio Tapia (jorgeignacio.tapia@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "hc_sr04.h"
#include "led.h" 
#include "analog_io_mcu.h"
#include "uart_mcu.h"

/*==================[macros and definitions]=================================*/

#define TIME_PERIOD 1000000 // 1 segundo
#define TIME_PERIOD2 100000 // 100 ms

#define SENSOR_TEMPERATURA CH1
#define GPIO_ALARMA GPIO_9


#define VOLTAJE_MIN 0.0
#define VOLTAJE_MAX 3300
#define TEMPERATURA_MIN 20.0
#define TEMPERATURA_MAX 50.0
#define TEMP_OBJEIVO 37.5


uint16_t distancia;
float promedio;
float suma;


/*==================[internal data definition]===============================*/

TaskHandle_t task_handle_TAREA1 = NULL

TaskHandle_t task_handle_TAREA2 = NULL

/*==================[internal functions declaration]=========================*/

/** @fn  void Notify(void *param)
 * @brief  notifica a la tareas task_handle
 * @param *param
 */
void Notify_TAREA1(void *param)
{
	vTaskNotifyGiveFromISR(task_handle_TAREA1, pdFALSE);
}

/** @fn  void Notify(void *param)
 * @brief  notifica a la tareas task_handle
 * @param *param
 */
void Notify_TAREA2(void *param)
{
	vTaskNotifyGiveFromISR(task_handle_TAREA2, pdFALSE);
}

/** @fn  void medirDistancia(void *pvParameter)
 * @brief  Tarea que mide la distancia y prende el led necesario dependiendo de la distancia
 * @param .void *pvParameter
 */
void medirDistancia(void *pvParameter)
{
	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // La tarea espera en este punto hasta recibir una notificación

		distancia = HcSr04ReadDistanceInCentimeters(); 
		
		LedOff(LED_1);
		LedOff(LED_2);
		LedOff(LED_3);

		if (distancia < 8)
		{
			LedOn(LED_1);
		}
		else if (distancia >=8 && distancia <=12)
		{
			LedOn(LED_2);
		}	
		else if (distancia > 12 && distancia < 140)
		{
	        LedOn(LED_3);
		}
		else if (distancia > 140) // se reiniciar el ciclo de medidas
		{
			suma=0.0;
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
	}
}	


/*==================[external functions definition]==========================*/
void app_main(void){

	// TIMER1 CONFIG
	timer_config_t timer_1 = {
		.timer = TIMER_A,
		.period = TIME_PERIOD,
		.func_p = Notify_TAREA1,
		.param_p = NULL};
	
	// TIMER2 CONFIG
	timer_config_t timer_2 = {
		.timer = TIMER_B,
		.period = TIME_PERIOD2,
		.func_p = Notify_TAREA2,
		.param_p = NULL};

	TimerInit(&timer_1); // inicializo timer1
	TimerInit(&timer_2); // inicializo timer 2

	xTaskCreate(&medirDistancia, "medir distancias", 2048, NULL, 5, &task_handle_TAREA1); 
	xTaskCreate(&medirTemperatura, "medir temperatura", 2048, NULL, 5, &task_handle_TAREA2); 
}
/*==================[end of file]============================================*/