/**
 * 
 * @file      driver_touch.h
 * @brief     driver for a touch button
 * @version   1.0.0
 * @author    Juan I Spinetto
 * @date      20/07/2024
 *
 *
 */



/*---------------
--   include    -
-----------------
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/touch_pad.h"



/**
 * @brief     init button
 * @return    ESP_OK Success
 *            ESP_ERR_NO_MEM Touch pad init error
 *            ESP_ERR_NOT_SUPPORTED Touch pad is providing current to external XTAL
 * @note      Include init and config
 */
esp_err_t init_touch_button(void);


/**
 * @brief     indicates if the button is touched 
 * @return    TRUE   the button is touched (the value of touch_pad_read is bellow the threshold)
 *            FALSE  the button is not touched (the value of touch_pad_read is above the threshold)
 * @note     The threshold is a defined constant, and its value is obtained empirically
 */
bool touch_button_down (void);


/**
 * @brief     indicates if the button is touched for at least 300ms
 * @return    TRUE   the button is touched for 300ms or more (the value of touch_pad_read is bellow the threshold in two reads separated by 300ms)
 *            FALSE  the button is not touched for 300ms or more (the value of touch_pad_read is above the threshold)
 * @note     The threshold is a defined constant, and its value is obtained empirically
 */
bool touch_button_pressed_300 (void);


/**
 * @brief     indicates if the button is touched for more tha 50ms and then released 
 * @return    TRUE   the button is touched for 50ms or more and then released 
 *            FALSE  the button is not touched for 200ms or more (the value of touch_pad_read is above the threshold)
 * @note     if the button is touched, the function will return control to the program once the button is released
 */
bool touch_button_press_release (void);
