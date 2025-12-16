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
#define BUAD_RATE 96000


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


 /** @fn  ConvertirVoltajeATemperatura(uint16_t voltaje)
 * @brief  convierte los valores de voltaje a temperatura
 * @param voltaje
 */
uint16_t ConvertirVoltajeATemperatura(uint16_t voltaje)
{
	return TEMPERATURA_MIN + (voltaje - VOLTAJE_MIN) * (TEMPERATURA_MAX - TEMPERATURA_MIN) / (VOLTAJE_MAX - VOLTAJE_MIN);
}

/** @fn  void activarAlarma()
 * @brief  activa la alarma
 */
void activarAlarma()
{
	GPIOOn(GPIO_ALARMA);
}

void mensaje(uint16_t promedio, uint16_t distancia){
	UartSendString(UART_PC, (char *) UartItoa(promedio, 10));
    UartSendString(UART_PC, "Cº");
    UartSendString(UART_PC, (char *) UartItoa(distancia, 10));
    UartSendString(UART_PC, "cm\r\n");
}

/** @fn  void desactivarAlarma()
 * @brief  activa la alarma
 */
void desactivarAlarma()
{
	GPIOOff(GPIO_ALARMA);
}

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

/** @fn  void medirTemperatura(void *pvParameter)
 * @brief  Tarea que mide la temperatura cuando se encuentra en la distancia correcta. 
 *         Ademas, calcula el promedio de las 10 mediciones y las informa por UART junto con su distancia
 * @param .void *pvParameter
 */

void medirTemperatura(void *pvParameter)
{
	uint16_t temperatura_mV;
	uint16_t temperatura;

	int numMediciones = 10;
    float mediciones[numMediciones];
	float suma = 0.0;

	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    if (distancia >=8 && distancia <=12)
	{
		AnalogInputReadSingle(SENSOR_TEMPERATURA, &temperatura_mV);
		temperatura = ConvertirVoltajeATemperatura(temperatura_mV);

		 for(int i = 0; i < numMediciones; i++)
		{
        mediciones[i] = temperatura;
		suma += mediciones[i];
		float promedio = suma / numMediciones;
	    }	

	    // MESAJES POR LA UART
		mensaje(promedio, distancia);

		if (temperatura < TEMP_OBJEIVO )
		{
			desactivarAlarma();
		}
		else if (temperatura > TEMP_OBJEIVO)
		{
			activarAlarma();
		}
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

	//ANALOG INPUT CONFIG
	analog_input_config_t analog_input ={
		.input = SENSOR_TEMPERATURA,
		.mode = ADC_SINGLE,
	};

	// UART CONFIG
	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = BUAD_RATE,
		.func_p = NULL,
		.param_p = NULL,
	};


	

	TimerInit(&timer_1); // inicializo timer1
	TimerInit(&timer_2); // inicializo timer 2

	TimerStart(timer_1.timer); // para que comience el timer 1
	TimerStart(timer_2.timer); // para que comience el timer 2

	HcSr04Init(GPIO_20, GPIO_21); 
	GPIOInit(GPIO_ALARMA, GPIO_OUTPUT);
	LedsInit();
	UartInit(&serial_pc);
	AnalogInputInit(&analog_input); //INICIALIZO


	xTaskCreate(&medirDistancia, "medir distancias", 2048, NULL, 5, &task_handle_TAREA1); 
	xTaskCreate(&medirTemperatura, "medir temperatura", 2048, NULL, 5, &task_handle_TAREA2); 
}
/*==================[end of file]============================================*/