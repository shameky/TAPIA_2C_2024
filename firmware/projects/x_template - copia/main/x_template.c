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

/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	struct leds my_leds = {TOGGLE, 1, 5, 500};
    control_led(&my_leds);
}
/*==================[end of file]============================================*/