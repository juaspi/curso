/**
 * @file driver_RYUW122.h
 *
 */

#ifndef DRIVER_RYUW122_H
#define DRIVER_RYUW122_H

/*********************
 *      INCLUDES
 *********************/
#include "freertos/FreeRTOS.h"

/*********************
 *      DEFINES
 *********************/
#define My_Network_ID "NTJIS001";          	
#define My_Address_ID "ADJIS001";   

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    RYUW122_UART_CONFIG_0,         // Set RYUW122 uses UART_0
    RYUW122_UART_CONFIG_1,         // Set RYUW122 uses UART_1
    RYUW122_UART_CONFIG_2,         // Set RYUW122 uses UART_2
} ryuw122_UART_conf_t;

typedef enum {
    RYUW122_MODO_TAG,         // Set/get RYUW122 TAG mode
    RYUW122_MODO_ANCHOR,      // Set/get RYUW122 ANCHOR mode  
    RYUW122_MODO_SLEEP       // Set/get RYUW122 SLEEP mode
} ryuw122_modo_t;

typedef enum {
    RYUW122_IPR_9600 ,      // Set/get RYUW122 baud rate to 9600
    RYUW122_IPR_57600,      // Set/get RYUW122 baud rate to 57600
    RYUW122_IPR_115200,     // Set/get RYUW122 baud rate to 115200 
    RYUW122_IPR_DEFAULT     // Set deafaul value to 115200
} ryuw122_ipr_t;

typedef enum {
    RYUW122_CHANNEL_6489,     // Set/get RYUW122 Channel to 6489.6MHz
    RYUW122_CHANNEL_7987,     // Set/get RYUW122 Channel to 7987.2 MHz 
    RYUW122_CHANNEL_DEFAULT   // Set Default Channel to 6489.6MHz
} ryuw122_channel_t;

typedef enum {
    RYUW122_BANDWIDTH_850Kb,     // Set/get RYUW122 Bandwidth to 850 Kbps
    RYUW122_BANDWIDTH_68Mb,      // Set/get RYUW122 Bandwidth to 6.8 Mbps 
    RYUW122_BANDWIDTH_DEFAULT    // Set Default Bandwidth to 850 Kbps
} ryuw122_bandwidth_t;

typedef struct {
    ryuw122_modo_t rModo;             /*Modo*/
    ryuw122_ipr_t rIpr;               /*Baud Rate*/
    ryuw122_channel_t rChannel;       /*Channel*/
    ryuw122_bandwidth_t rBandwidth;   /*Bandwidth*/
    char rNetworkId[20];              /*Network ID*/
    char rAddressId[20];              /*Address ID*/
} ryuw122_config_t;

typedef struct {
    ryuw122_UART_conf_t rUartConf;      /*UART connection*/
    int resetGPIO;                    /*Reset GPIO*/
} ryuw122_hardward_config_t;


/*======================
 * FUNTIONS
 *======================*/

/**
------------------------------------------------------------------------------------------+
-  RYUW122_Init_Default
-  @brief  Inicializa Transsiver RYUW122 
-
-  @note
-  Hardward - Por defecto establece la la UART_1 y el GPIO 13 para reset
-    Para usar otra configuración debe llamarse previamente a "Set_Hardward_Config" 
-  @param  none
-  @return
-     - ESP_OK   Success
-     - ESP_FAIL Parameter error
------------------------------------------------------------------------------------------+
*/
esp_err_t RYUW122_Init_Default (void);

/**
------------------------------------------------------------------------------------------+
-  RYUW122_Init
-  @brief  Inicializa Transsiver RYUW122 
-
-  @note
-  Hardward - Por defecto establece la la UART_1 y el GPIO 13 para reset
-    Para usar otra configuración debe llamarse previamente a "Set_Hardward_Config" 
-  @param  rConfigSetting Incluye la configuracion completa del transiver 
-                         debe definise antes de llamar a la función Init
-  @return
-     - ESP_OK   Success
-     - ESP_FAIL Parameter error
------------------------------------------------------------------------------------------+
*/
esp_err_t RYUW122_Init (ryuw122_config_t rConfigSetting);

/**
------------------------------------------------------------------------------------------+
-  Start_Continue_Measurement
-  @brief  Inicial el uso de la interrupciónX. Cada X ms obtendrá un nuevo valor de distacia
-          y calculará la velocidad en función de la distancia anterior y el intervalo.
-          Los valores de Distancia y Velocidad se actualizarán en las posiciones de memoria
-          enviadas como parametros 
-
-  @note   Esta función utiliza la interrupción X
-          Los valores de distancia son filtrados o promediados para disminuir el ruido 
-          generado por la variacion de la medicion.

-  @param  interval El intervalo de activación de la interrupcion en ms
-  @param  dist  Direccion de memoria donde se actulizarán el valor de distancia
-  @param  Vel  Direccion de memoria donde se actulizarán el valor de Velocidad        
-  @param  newData  Direccion de memoria a la bandera de nuevo dato (0 - No hay nuevo dato; 1 hay nuevo Dato)
-  @return
-     - ESP_OK   Success
-     - ESP_FAIL Parameter error
------------------------------------------------------------------------------------------+
*/
esp_err_t Start_Continue_Measurement (int interval, int* dist, int* Vel, int* newData);

/**
------------------------------------------------------------------------------------------+
-  Stop_Continue_Measurement
-  @brief  Deshabilita la interrupción iniciada en "Start_Continue_Measurement"
-  @note   Los valores de Distancia y velocidad, ya no se actualizarán
-  @return
-     - ESP_OK   Success
-     - ESP_FAIL Parameter error
------------------------------------------------------------------------------------------+
*/
esp_err_t Stop_Continue_Measurement (void);


/**
------------------------------------------------------------------------------------------+
-  Tomar_Distancia_Actual
-  @brief  Realiza una medición puntual de distancia
-  @note   Para que esta medición arroje un valor valido, el ANCHOR y TAG deben estar 
-          encencidos, y con capacidad de comunicarse, con la misma configuracion de 
-          BR, Channel, Bandwidth, Network ID y Address ID.
           Un valor de retorno 0 corresponde a un error
-  @return  Devuelve el valor de distacia expresado en cm
-           Si el Valor es 0, corresponde a un error.
------------------------------------------------------------------------------------------+
*/
int Tomar_Distancia_Actual (void);

/**
------------------------------------------------------------------------------------------+
-  Get_Config_Setting
-  @brief  Lee los valores de configuración del RYUW122
-  @note   Para leer estos valores el transiver debe inicializarse, si no es así ésta 
-          función lo inicializará.
-  @return
-     - ESP_OK   Success
-     - ESP_FAIL Parameter error
------------------------------------------------------------------------------------------+
*/
esp_err_t Get_Config_Setting(ryuw122_config_t *rConfigSetting); 

/**
------------------------------------------------------------------------------------------+
-  Set_Config_Setting
-  @brief  Reescribe los valores de configuración del RYUW122
-  @note   Para escribir estos valores el transiver debe inicializarse, si no es así ésta 
-          función lo inicializará.
-  @return
-     - ESP_OK   Success
-     - ESP_FAIL Parameter error
------------------------------------------------------------------------------------------+
*/
esp_err_t Set_Config_Setting (ryuw122_config_t rConfigSetting);

/**
------------------------------------------------------------------------------------------+
-  Set_Hardward_Config
-  @brief  Define los valores de configuración de hardware
-  @return
-     - ESP_OK   Success
-     - ESP_FAIL Parameter error
-  @note Se usa en el caso que no sequieran usar los valores por defecto. 
         UART_1 y el GPIO 13 para reset
------------------------------------------------------------------------------------------+
*/
esp_err_t Set_Hardward_Config (void);

/**
------------------------------------------------------------------------------------------+
-  @brief  Se usa para tester la comunicación con el periferico
-  @return
-     - ESP_OK   Success
-     - ESP_FAIL Parameter error
------------------------------------------------------------------------------------------+
*/
esp_err_t Test_Communication (void);

#endif /*DRIVER_RYUW122_H*/
