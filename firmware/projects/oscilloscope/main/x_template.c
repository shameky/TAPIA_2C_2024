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

/*==================[macros and definitions]=================================*/

#define CONFIG_MEASURE_PERIOD 2000
#define BUFFER_SIZE 256
#define ECG_FREQUENCY 3900
#define BAUD_RATE 115200

/*==================[internal data definition]===============================*/

TaskHandle_t covert_digital_task_handle = NULL;


TaskHandle_t convert_analog_task_handle = NULL;



/**
 * @brief This is the ECG signal data array. It contains 256 samples representing an ECG signal.
 * Each sample is an 8-bit unsigned integer.
 *
 * @note The ECG signal data is used in the 'convert_analog' function to simulate an ECG signal.
 *
 * @see convert_analog
 */
const char ecg[BUFFER_SIZE] = {
    17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
    18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
    10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
    38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
    133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
    24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
    17,17,17
};


timer_config_t timer_ecg = {
    	.timer = TIMER_B,
        .period = 0,
        .func_p = NULL,
        .param_p = NULL
    };


/*==================[internal functions declaration]=========================*/

/**
 * @brief This function is an interrupt service routine (ISR) for Timer A.
 * It notifies the 'convert_digital' task to perform its task using a task notification.
 *
 * @param param A pointer to the parameters passed to the ISR. In this case, it is not used.
 *
 * @return void.
 */
void FuncTimerA(void* param){

    vTaskNotifyGiveFromISR(covert_digital_task_handle, pdFALSE);   

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
 * @brief This function continuously reads an analog signal from a specified channel, converts it to digital, and sends the digital data through a UART.
 *
 * @param param A pointer to the parameters passed to the function. In this case, it is not used.
 *
 * @return void.
 */
static void convert_digital(void *param){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		uint16_t reading;
		AnalogInputReadSingle(CH1, &reading);
		UartSendString(UART_PC, (char *)UartItoa(reading, 10));
		UartSendString(UART_PC, "\r" );
	}
}

/**
 * @brief This function continuously reads an analog signal from the ECG buffer, converts it to digital, and writes the digital data to the analog output.
 *
 * @param param A pointer to the parameters passed to the function. In this case, it is not used.
 *
 * @return void.
 */
static void convert_analog(void *param){

	int index = 0;

	while (true){

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		if (index == BUFFER_SIZE){
			index = 0;
		}

		uint8_t data;
		data = ecg[index];

		AnalogOutputWrite(data);

		index++;
	}}



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

	timer_ecg.period = ECG_FREQUENCY;
	timer_ecg.func_p = FuncTimerB;

	TimerInit(&timer_measurement);
	TimerInit(&timer_ecg);
	xTaskCreate(&convert_digital, "Convertir señal a Digital", 1024, NULL, 5, &convert_analog_task_handle);
	xTaskCreate(&convert_analog, "Convertir señal a Analogica", 1024, NULL, 5, &convert_analog_task_handle);
	TimerStart(timer_measurement.timer);
	TimerStart(timer_ecg.timer);
}

/*==================[end of file]============================================*/