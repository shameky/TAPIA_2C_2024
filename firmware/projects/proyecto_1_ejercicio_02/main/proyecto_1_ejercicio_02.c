/*! @mainpage proyecto 1 ejercicio 02
 *
 * @section Modificacion de la aplicaicon 1_blinking_switch
 *
 * Al apretar los botones 1 y 2, prenden sus correspondientes LEDs en modo ON.
 *
 * AL apretar al mismo tiempo los botones 1 y 2, titila el LED_3 en modo TOOGLE
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
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 300

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
void ControlLedsWithSwitches() {
    uint8_t teclas;
    LedsInit();
    SwitchesInit();
    while(1) {
        teclas = SwitchesRead();
        switch(teclas) {
            case SWITCH_1:
                LedToggle(LED_1);
                break;
            case SWITCH_2:
                LedToggle(LED_2);
                break;
            case SWITCH_1 | SWITCH_2:
                LedToggle(LED_3);
                break;
        }
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){
	ControlLedsWithSwitches();
}
/*==================[end of file]============================================*/