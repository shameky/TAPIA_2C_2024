/*! @mainpage proyecto 1 ejercicio 04
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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
#define number_to_convert uint32_t number = 20; // Número decimal a convertir
#define digits_number uint8_t digits = 2;	// Cantidad de dígitos en el número
#define array uint8_t bcd_array[8];	// Arreglo para almacenar los dígitos en BCD

/*==================[internal functions declaration]=========================*/
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
	
    // Convertir el número a formato BCD
    if (convertToBcdArray(number_to_convert, digits_number, array) == 0) {
        // Imprimir el arreglo en formato BCD
        printBcdArray(bcd_array, digits);
    } else {
        printf("Error: Número de dígitos no válido.\n");
    }
}
/*==================[end of file]============================================*/