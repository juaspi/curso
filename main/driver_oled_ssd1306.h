/**
 * 
 * @file      driver_oled_ssd1306.h
 * @brief     driver ssd1306 basic header file
 * @version   1.0.0
 * @author    Juan I Spinetto
 * @date      14/07/2024
 *
 *
 */



/*---------------
--   include    -
-----------------
*/
#include "esp_err.h"
#include <stdbool.h>


/**
 * @brief     basic init de la pantalla oled
 * @return    ESP_OK
 *            ESP_FAIL
 * @note      inicializa el oled e ic2
 */
esp_err_t  oled_ssd1306_init(void);


/**
 * @brief  borra toda la información de la pantalla
 * @return none
 * @note   none
 */
void oled_ssd1306_clear_display(void);

/**
 * @brief  Escribe un caracter en una posición especifica 
 * @param[in] c es el caracter en ascii
 * @param[in] x es la coordenada x en pixels de 0 a 127
 * @param[in] y es la coordenada y en filas de 0 a 7
 * @return    none
 * @note      none
 */
void oled_ssd1306_draw_char(char c, int x, int y);

/**
 * @brief  escribe un string a partir de una posición especifica
 * @param[in] str es el string a escribir 
 * @param[in] x es la coordenada x en pixels de 0 a 127
 * @param[in] y es la coordenada y en filas de 0 a 7
 * @return    none
 * @note      none
 */
void oled_ssd1306_draw_string(const char *str, int x, int y);

/**
 * @brief  basic example clear
 * @param[in] x es la coordenada x en pixels de 0 a 127
 * @param[in] y es la coordenada 7 en pixels de 0 a 63
 * @return none
 * @note   none
 */
void oled_ssd1306_draw_pixel(int x, int y);


