/*! @mainpage proyecto 1 ejercicio 03
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
 * @author Jorge Tapia (jorge.tapia@ingenieria.uner.edu.ar)
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
#define chosen_mode TOGGLE;
#define chosen_led 1;
#define chosen_cycle 5;
#define chosen_period 500;
/*==================[internal data definition]===============================*/
struct leds
{
    LedMode mode;      // ON, OFF, TOGGLE
	uint8_t n_led;       //	indica el número de led a controlar
	uint8_t n_ciclos;    //	indica la cantidad de ciclos de encendido/apagado
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

/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	struct leds my_leds = {chosen_mode, chosen_led, chosen_cycle, chosen_period};
    control_led(&my_leds);
}
/*==================[end of file]============================================*/