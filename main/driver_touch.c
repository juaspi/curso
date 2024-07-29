/**
 * 
 * @file      driver_touch.c
 * @brief     driver for a touch button
 * @version   1.0.0
 * @author    Juan I Spinetto
 * @date      20/07/2024
 *
 *
 */



/*********************
 *      INCLUDES
 *********************/
#include "driver_touch.h"
#include "driver/touch_pad.h"

/*********************
 *      DEFINES
 *********************/
#define T_BUTTON_PIN  TOUCH_PAD_NUM3   //  Para esta apleación se usará en TOUCH 3 correspondiente al GPIO15
#define UMBRAL 600      // el valor del umbral se define en función del hardware implementado y las pruebas realizadas sobre este

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *      VARIABLES
 **********************/


/*============================
 * LOCAL FUNTIONS PROTOPYTES
 *============================
 */

/*============================
 * FUNTIONS
 *============================
 */


/**
 * @brief     init button
 * @return    ESP_OK Success
 *            ESP_ERR_NO_MEM Touch pad init error
 *            ESP_ERR_NOT_SUPPORTED Touch pad is providing current to external XTAL
 * @note      Include init and config
 */

esp_err_t init_touch_button(void){
    
    esp_err_t return_value;

    return_value = touch_pad_init();
    if (return_value != ESP_OK) {
        printf("Touch pad init error\n");
        return return_value;
    }
    
    return_value = touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    if (return_value != ESP_OK) {
        printf("Touch pad init error\n");
        return return_value;
    }

    return_value = touch_pad_config(T_BUTTON_PIN, -1);
    if (return_value != ESP_OK) {
        printf("Touch pad config fail\n");
        return return_value;
    }

    return return_value;
}


/**
 * @brief     indicates if the button is touched 
 * @return    TRUE   the button is touched (the value of touch_pad_read is bellow the threshold)
 *            FALSE  the button is not touched (the value of touch_pad_read is above the threshold)
 * @note     The threshold is a defined constant, and its value is obtained empirically
 */
bool touch_button_down (void){

    uint16_t lectura;
    esp_err_t return_value;

    return_value = touch_pad_read(T_BUTTON_PIN, &lectura);
    if (return_value != ESP_OK) {
        printf("error: %d\n", return_value);
        return false;
    }  else {
        return (lectura < UMBRAL);
    }  

}



/**
 * @brief     indicates if the button is touched for at least 300ms
 * @return    TRUE   the button is touched for 300ms or more (the value of touch_pad_read is bellow the threshold in two reads separated by 300ms)
 *            FALSE  the button is not touched for 300ms or more (the value of touch_pad_read is above the threshold)
 * @note     The threshold is a defined constant, and its value is obtained empirically
 */
bool touch_button_pressed_300 (void){
    
    #define ESPERA 300

    uint16_t lectura;
    esp_err_t return_error;

    return_error = touch_pad_read(T_BUTTON_PIN, &lectura);
    if (return_error != ESP_OK) {
        printf("error: %d\n", return_error);
        return false;
    }  
    else {
        if (lectura > UMBRAL)
        {
            return false;
        }
        else
        { 
            vTaskDelay(ESPERA / portTICK_PERIOD_MS);
            return_error = touch_pad_read(T_BUTTON_PIN, &lectura);
            if (return_error != ESP_OK) {
                printf("error: %d\n", return_error);
                return false;
            }  else {
                return (lectura < UMBRAL);
            }
        }
    };
}  


/**
 * @brief     indicates if the button is touched for more tha 50ms and then released 
 * @return    TRUE   the button is touched for 50ms or more and then released
 *            FALSE  the button is not touched for 50ms or more (the value of touch_pad_read is above the threshold)
 * @note     if the button is touched, the function will return control to the program once the button is released
 */
bool touch_button_press_release (void)
{
    #define ESPERA 50

    uint16_t lectura;
    esp_err_t return_error;

    return_error = touch_pad_read(T_BUTTON_PIN, &lectura);
    if (return_error != ESP_OK) {
        printf("error: %d\n", return_error);
        return false;
    }  
    else {
        if (lectura > UMBRAL)
        {
            return false;
        }
        else
        { 
            vTaskDelay(ESPERA / portTICK_PERIOD_MS);
            return_error = touch_pad_read(T_BUTTON_PIN, &lectura);
            if (return_error != ESP_OK) {
                printf("error: %d\n", return_error);
                return false;
            }  else {
                if (lectura > UMBRAL){
                    return false;
                } else{
                    while (lectura < UMBRAL)
                    {
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                        return_error = touch_pad_read(T_BUTTON_PIN, &lectura);
                    }                
                return true;                    
                }

            }
        }
    };
}  
