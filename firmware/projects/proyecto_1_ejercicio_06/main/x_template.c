/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * The program allows you to view numerical data through an LCD screen.
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	LCD 	 	| 	GPIO_19     |
 * |				|	GPIO_18		|
 * |				|	GPIO_9		|
 * |	BCD INPUT	|	GPIO_20		|
 * |				|	GPIO_21		|
 * |				|	GPIO_22		|
 * |				|	GPIO_23		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Tapia Jorge Ignacio (jorge.tapia@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <gpio_mcu.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal functions declaration]=========================*/

/**
 @brief Function that allows a chosen number to be displayed on the LCD display.
 @param data 32-bit decimal number.
 @param digits Number of digits of the decimal number.
 @param ptr_bits Pointer to a gpioConf_t struct vector that stores the pins corresponding to the bits of
				each each digit to display.
 @param ptr_lcd Pointer to a gpioConf_t struct vector that stores the pins corresponding to every 7 segments
				of the lcd.
 * 
*/
void show_display (uint32_t data, uint8_t digits, gpioConf_t *ptr_bits, gpioConf_t *ptr_lcd )
{	
	for(int i = 0; i<4; i++)
	{
		GPIOInit(ptr_bits[i].pin, ptr_bits[i].dir);
	}

	for(int i = 0; i<3; i++)
	{
		GPIOInit(ptr_lcd[i].pin, ptr_lcd[i].dir);
	}

	uint8_t digito;
	uint8_t bcd[digits];

	for (int i=(digits-1); i>=0; i--)
	{
		digito = data%10;
		bcd[i] = digito;
		data = data/10;
	}

	for (int j = 0; j<3; j++)
	{
		GPIOOff(ptr_lcd[j].pin);
		GPIOOn(ptr_lcd[j].pin);

		for (int i = 0; i<4; i++)
		{
			if( (bcd[j] & (1<<i)) == 0)
			{
				GPIOOff(ptr_bits[i].pin);
			}
			else
			{
				GPIOOn(ptr_bits[i].pin);
			}
		}

		GPIOOff(ptr_lcd[j].pin);

	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	gpioConf_t bits[4] = {{GPIO_20,GPIO_OUTPUT},{GPIO_21,GPIO_OUTPUT},{GPIO_22,GPIO_OUTPUT},{GPIO_23,GPIO_OUTPUT}};
	gpioConf_t LCD[3] = {{GPIO_19,GPIO_OUTPUT},{GPIO_18,GPIO_OUTPUT},{GPIO_9,GPIO_OUTPUT}};

	uint32_t number 345;
	uint8_t digits = 3;

	show_display(number, digits, bits, LCD);
}
/*==================[end of file]============================================*/