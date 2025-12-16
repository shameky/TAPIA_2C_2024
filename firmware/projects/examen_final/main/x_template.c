/*! @mainpage EXAMEN FINAL
 *
 * @section Consigna de Aplicación: Alimentador automático de mascotas
 *
 * Se pretende diseñar un dispositivo basado en la ESP-EDU que se utilizará para suministrar alimento y agua a una mascota.
 * El sistema está compuesto por dos recipientes junto a dos depósitos (uno para el alimento y el otro para el agua).
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	BALANZA 	| 	CH1 		|
 * | VALVULA_AGUA	| 	GPIO_9		|
 * | VALVULA_COMIDA	| 	GPIO_10		|
 * | 	HcSr04	 	|GPIO_20, GPIO_21|
 * 
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 16/12/2025 | Creación del Documento                         |
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
#include <switch.h>
/*==================[macros and definitions]=================================*/
#define TIME_PERIOD_AGUA 5000000 // 5 segundos
#define TIME_PERIOD_PESO 5000000 // 5 segundos
#define TIME_PERIOD_INFORMAR 5000000 // 5 segundos

#define BALANZA CH1
#define VALVULA_AGUA GPIO_9
#define VALVULA_COMIDA GPIO_10

#define distancia_recipiente_lleno 31.5
#define distancia_recipiente_deseada 33

#define recipiente_lleno 500
#define recipiente_vacio 50

#define BUAD_RATE 96000

uint16_t cantidad_agua 
uint16_t cantidad_alimento


/*==================[internal data definition]===============================*/

TaskHandle_t task_handle_TAREA1 = NULL

TaskHandle_t task_handle_TAREA2 = NULL

TaskHandle_t task_handle_TAREA3 = NULL

bool  iniciar_sistema = false;

/*==================[internal functions declaration]=========================*/

/** @fn  void static read_switches(void *pvParameter)
 * @brief  
 * @param *pvParameter
 */
void static read_switches(void *pvParameter)
{
    bool *flags = (bool*) pvParameter;
	*flags = !*flags;
}


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

/** @fn  void Notify(void *param)
 * @brief  notifica a la tareas task_handle
 * @param *param
 */
void Notify_TAREA3(void *param)
{
	vTaskNotifyGiveFromISR(task_handle_TAREA3, pdFALSE);
}

uint16_t convertir_voltaje_peso(uint16_t voltaje)
{
	return ((voltaje * 1000)/ 3300);
}

/** @fn  void suministrar_comida(void *pvParameter)
 * @brief  
 * @param *param
 */
void suministrar_comida(void *pvParameter)
{
	uint16_t peso_mV;
	uint16_t peso;


	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		AnalogInputReadSingle(BALANZA, &peso_mV);
		peso = convertir_voltaje_peso(peso_mV);
		peso = cantidad_alimento

		if(iniciar_sistema){
			if (peso == recipiente_lleno)
		{
			desactivar_valvula_comida();
		}
		else if (peso < recipiente_vacio)
		{
			activar_valvula_comida();
		}

		desactivar_valvula_comida();
		}
		

	}
}

/** @fn  void suministrar_agua(void *pvParameter)
 * @brief  
 * @param *param
 */
void suministrar_agua(void *pvParameter)
{
	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // La tarea espera en este punto hasta recibir una notificación

		distancia = HcSr04ReadDistanceInCentimeters(); 
		volumen_medido = 3,14*10*10*(31.5 - distancia);
		cantidad_agua = volumen_medido;


		if(iniciar_sistema){

			if (distancia = distancia_recipiente_lleno)
		{
			desactivar_valvula_agua();
		}
		else if (distancia > distancia_recipiente_deseada)
		{
			activar_valvula_agua();
		}

		desactivar_valvula_agua();
		}
		
	
	}
}

	/** @fn  void activar_valvula_agua()
	 * @brief  
	 */
	void activar_valvula_agua()
	{
		GPIOOn(VALVULA_AGUA);
	}

	/** @fn  void desactivar_valvula_agua()
	 * @brief  
	 */
	void desactivar_valvula_agua()
	{
		GPIOOff(VALVULA_AGUA);
	}

	/** @fn  void activar_valvula_agua()
	 * @brief  
	 */
	void activar_valvula_comida()
	{
		GPIOOn(VALVULA_COMIDA);
	}

	/** @fn  void desactivar_valvula_comida()
	 * @brief  
	 */
	void desactivar_valvula_comida()
	{
		GPIOOff(VALVULA_COMIDA);
	}

	/** @fn  void notificacion_sistema_prendido()
	 * @brief  
	 */
	void notificacion_sistema_prendido()
	{
		if(iniciar_sistema = true)
		LedOn(LED_1);
		else 
		LedOff(LED_1);
	}

	/** @fn  void informar_estado_recipiente(void *pvParameter)
	 * @brief  
	 */
	void informar_estado_recipiente(void *pvParameter)
{
	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		UartSendString(UART_PC, "Agua: ");
		UartSendString(UART_PC, (char *) UartItoa(cantidad_agua, 10));
   	 	UartSendString(UART_PC, "cm3, ");
		UartSendString(UART_PC, "Alimento: ");
   		UartSendString(UART_PC, (char *) UartItoa(cantidad_alimento, 10));
   		UartSendString(UART_PC, "gr\r\n");
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){

	// TIMER1 CONFIG
	timer_config_t timer_agua = {
		.timer = TIMER_A,
		.period = TIME_PERIOD_AGUA,
		.func_p = Notify_TAREA1,
		.param_p = NULL};
	
	// TIMER2 CONFIG
	timer_config_t timer_peso = {
		.timer = TIMER_B,
		.period = TIME_PERIOD_PESO,
		.func_p = Notify_TAREA2,
		.param_p = NULL};

	// TIMER1 CONFIG
	timer_config_t timer_informar = {
		.timer = TIMER_A,
		.period = TIME_PERIOD_INFORMAR,
		.func_p = Notify_TAREA3,
		.param_p = NULL};

	TimerInit(&timer_agua); // inicializo timer
	TimerInit(&timer_peso); // inicializo timer
	TimerInit(&timer_informar); // inicializo timer

	TimerStart(timer_agua.timer); // para que comience el timer
	TimerStart(timer_peso.timer); // para que comience el timer
	TimerStart(timer_informar.timer); // para que comience el timer

	//ANALOG INPUT CONFIG
	analog_input_config_t analog_input_balanza ={
		.input = BALANZA,
		.mode = ADC_SINGLE,
	};

	// UART CONFIG
	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = BUAD_RATE,
		.func_p = NULL,
		.param_p = NULL,
	};

	HcSr04Init(GPIO_20, GPIO_21); 
	GPIOInit(VALVULA_AGUA, GPIO_OUTPUT);
	GPIOInit(VALVULA_COMIDA, GPIO_OUTPUT);
	LedsInit();
	UartInit(&serial_pc);
	AnalogInputInit(&analog_input_balanza); //INICIALIZO

	SwitchesInit();

	SwitchActivInt(SWITCH_1, read_switches, &iniciar_sistema);

	xTaskCreate(&suministrar_agua, "suministrar agua", 2048, NULL, 5, &task_handle_TAREA1); 
	xTaskCreate(&suministrar_comida, "suministrar comida", 2048, NULL, 5, &task_handle_TAREA2); 
	xTaskCreate(&informar_estado_recipiente, "informar estado del recipiente", 2048, NULL, 5, &task_handle_TAREA3);


}
/*==================[end of file]============================================*/