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
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*==================[macros and definitions]=================================*/
typedef enum {
    OFF = 0,
    ON = 1,
    TOGGLE = 2
} LedMode;

/*==================[internal data definition]===============================*/
struct leds
{
    LedMode mode;      // ON, OFF, TOGGLE
	uint8_t n_led;       //	indica el número de led a controlar
	uint8_t n_ciclos;    //	indica la cantidad de ciclos de ncendido/apagado
	uint16_t periodo;    //	indica el tiempo de cada ciclo
} my_leds;

/*==================[internal functions declaration]=========================*/

void control_led(struct leds *my_leds) {
    if (my_leds->mode == ON)
	{
		switch(my_leds->n_led){

    		case 1:
				LedOn(LED_1);
    		break;

    		case 2:
				LedOn(LED_2);
    		break;

			case 3:
				LedOn(LED_3);
			break;
			}
    }
	else if (my_leds->mode == OFF)
	{	
		switch(my_leds->n_led){

    		case 1:
				LedOff(LED_1);
    		break;

    		case 2:
				LedOff(LED_2);
    		break;

			case 3:
				LedOff(LED_3);
			break;
			}
    }
	else if (my_leds->mode == TOGGLE)
	{
		for (uint8_t i = 0; i < my_leds->n_ciclos; i++) {
			LedToggle(my_leds->n_led);
            vTaskDelay(my_leds->periodo/portTICK_PERIOD_MS);
		}

	}
}

// Función para convertir un número decimal a formato BCD
int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number) {
    // Verificar si el número de dígitos es suficiente para el número dado
    if (digits < 1) {
        return -1; // Error: número de dígitos no válido
    }
    
    // Inicializar el arreglo con ceros
    for (int i = 0; i < digits; i++) {
        bcd_number[i] = 0;
    }

    // Convertir el número a BCD
    for (int i = 0; i < digits; i++) {
        if (data == 0) {
            break;
        }
        // Extraer el dígito menos significativo y almacenarlo en el arreglo
        bcd_number[digits - 1 - i] = data % 10;
        // Eliminar el dígito menos significativo del número
        data /= 10;
    }

    return 0; // Éxito
}

// Función para imprimir un número en formato binario de 4 bits
void printBcdArray(const uint8_t *bcd_number, uint8_t digits) {
    printf("BCD array: ");
    for (int i = 0; i < digits; i++) {
        // Imprimir cada dígito en formato binario de 4 bits
        for (int j = 3; j >= 0; j--) {
            printf("%d", (bcd_number[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}



/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	struct leds my_leds = {TOGGLE, 1, 5, 500}; //definir las variables bien
    control_led(&my_leds);

	uint32_t number = 20;  // Número decimal a convertir
    uint8_t digits = 2;          // Cantidad de dígitos en el número
    uint8_t bcd_array[8];        // Arreglo para almacenar los dígitos en BCD

    // Convertir el número a formato BCD
    if (convertToBcdArray(number, digits, bcd_array) == 0) {
        // Imprimir el arreglo en formato BCD
        printBcdArray(bcd_array, digits);
    } else {
        printf("Error: Número de dígitos no válido.\n");
    }
}
/*==================[end of file]============================================*/