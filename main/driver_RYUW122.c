/**
 * @file Driver_RYUW122.c
 *
 */


/*********************
 *      INCLUDES
 *********************/
#include "driver_RYUW122.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define UART_RYUW UART_NUM_1
#define BUF_SIZE_RY (1024)


/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *      VARIABLES
 **********************/
bool hwInitated = false;
ryuw122_hardward_config_t ryuwHwConfig = {
    RYUW122_UART_CONFIG_1,
    13};


/*============================
 * LOCAL FUNTIONS PROTOPYTES
 *============================
 */

static void RYUW_UART_init (void);
static void RYUW_Reset_init (void);
int Send_AT_to_RYUW(char* Mensage_AT, int wait);
esp_err_t RYUW22_Set_MODE (ryuw122_modo_t modo);           // Set Modo as TAG, ANCHOR or Sleep
esp_err_t RYUW22_Set_Baud_Rate (ryuw122_ipr_t baud);       // Set Baud Rate
esp_err_t RYUW22_Set_Channel (ryuw122_channel_t channel);  // Set RF Channel.
esp_err_t RYUW22_Set_Bandwidth (ryuw122_channel_t band);   // Set Bandwidth
esp_err_t RYUW22_Set_Network_ID (char* networkId);         // Set Network ID
esp_err_t RYUW22_Set_Address_ID (char* addressId);         // Set Address ID

/*======================
 * FUNTIONS
 *======================*/

esp_err_t RYUW122_Init_Default (void){

     ryuw122_config_t ryuwConfig = {
        RYUW122_MODO_ANCHOR,
        RYUW122_IPR_DEFAULT,
        RYUW122_CHANNEL_DEFAULT,
        RYUW122_BANDWIDTH_DEFAULT,
        "Anchor12",
        "TAG12345"
        };
    
    return RYUW122_Init(ryuwConfig);
}

esp_err_t RYUW122_Init (ryuw122_config_t rConfigSetting){

    if (!hwInitated){
        if (Set_Hardward_Config () == ESP_FAIL){
            return ESP_FAIL;
        };
    };
    printf ("1- Hardware Init  OK\n");

    if (RYUW22_Set_MODE(rConfigSetting.rModo) == ESP_FAIL){
        return ESP_FAIL;
    };
    printf ("2- Set Modo OK\n");

    if (RYUW22_Set_Baud_Rate(rConfigSetting.rIpr) == ESP_FAIL){
        return ESP_FAIL;
    };
    printf ("3- Set BR OK\n");

    if (RYUW22_Set_Bandwidth (rConfigSetting.rBandwidth) == ESP_FAIL){
       return ESP_FAIL;
    };
    printf ("4- Set Bandwidth OK\n");

    if (RYUW22_Set_Network_ID (rConfigSetting.rNetworkId) == ESP_FAIL){
        return ESP_FAIL;
    };
    printf ("5- Set Network_ID OK\n");

    if (RYUW22_Set_Address_ID (rConfigSetting.rAddressId) == ESP_FAIL){
        return ESP_FAIL;
    };
    printf ("6- Set ADDRESS_ID OK\n");



    printf ("******* INIT TERMINADO **********\n");

    return ESP_OK;
}

esp_err_t Start_Continue_Measurement (int interval, int* dist, int* Vel, int* newData){
    return ESP_FAIL;
}

int Tomar_Distancia_Actual (void){
    
    uint8_t data[100];
    int length;
    int i = 0; 
    int dist_res;
    char mesgAT[50];
    char dist[5];
    
    strcpy((char*)mesgAT,"AT+ANCHOR_SEND=TAG12345,1,T");
 
    if (Send_AT_to_RYUW((char*) mesgAT, 10) != 1){
        return -2;
    };
          
    vTaskDelay(40 / portTICK_PERIOD_MS);

    length = uart_read_bytes(UART_RYUW, data, BUF_SIZE_RY, pdMS_TO_TICKS(20));

    if (length > 0) {
        data[length] = '\0';
                   // Imprime los datos recibidos
//        printf("Re: %s\n", data);  
        
        while (data[i+24]!=' ')
        {
           dist[i]=data[i+24];   
           i++;
        }
        
        dist [i+1] = '\0';

        dist_res = atoi(dist);
        printf("estracto:  %d\n", dist_res); 
        
        return dist_res;   
    } else { printf("No hay Resp del TAG");}
 
    return -1;
}

esp_err_t Get_Config_Setting(ryuw122_config_t *rConfigSetting){
    return ESP_FAIL;
}

esp_err_t Set_Config_Setting (ryuw122_config_t rConfigSetting){
    return ESP_FAIL;
}

esp_err_t Set_Hardward_Config (){

    RYUW_Reset_init();                              // Set Reset GPIO
    gpio_set_level(ryuwHwConfig.resetGPIO, 0);      // Reset RYUW122
    printf ("1- Init Reset OK\n");

    RYUW_UART_init();                               // Init UART
    printf ("2- Init UART OK\n");

    vTaskDelay(20 / portTICK_PERIOD_MS);
    gpio_set_level(ryuwHwConfig.resetGPIO, 1);      // Reset RYUW122
    vTaskDelay(20 / portTICK_PERIOD_MS);

    hwInitated = true;

    return ESP_OK;
}

int Send_AT_to_RYUW(char* Mensage_AT, int wait){
    
    uint8_t respuesta[BUF_SIZE_RY];
    int length = 0;

    strcpy((char*)respuesta,"vacio");
    
    strcat((char*)Mensage_AT,"\r\n");

//    printf ("\n1- Mg:   %s", Mensage_AT);

    uart_write_bytes(UART_RYUW, (char*)Mensage_AT, strlen((char*)Mensage_AT));

//    printf ("2- Mg enviado\n");
    vTaskDelay(wait / portTICK_PERIOD_MS);

    length = uart_read_bytes(UART_RYUW, respuesta, BUF_SIZE_RY, pdMS_TO_TICKS(20));

//    printf ("3- Respuesta: %s", respuesta);

    if (length > 0) {
        respuesta[length] = '\0';

        if (strcmp ((char*)respuesta,"+OK\r\n")==0) {
            printf("\nresultado de send es 1\n");
            return 1;
        }
        else {
            printf("\nresultado de send es: %s\n", respuesta);
            return 2;//(respuesta[5] - 48);
        }
    };
    printf("\nresultado de send es 0\n");
    return 0;
}

esp_err_t Test_Communication (void){
    
    char mesgAT[50];

    strcpy((char*)mesgAT,"AT");
    
    if (Send_AT_to_RYUW((char*)mesgAT, 10) == 1){
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }

}

static void RYUW_UART_init (void){
        
    // Configuración de la UART

    const uart_config_t uart_config_2 = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    // Instala el controlador de la UART con los parámetros de configuración para RYUW
     uart_param_config(UART_RYUW, &uart_config_2);    
     uart_driver_install(UART_RYUW, BUF_SIZE_RY * 2, 0, 0, NULL, 0);    
     uart_set_pin(UART_RYUW, 10, 9, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);    // Configura los pines de la UART (puedes cambiar los pines según tu hardware)

}

static void RYUW_Reset_init (void){
    
    gpio_reset_pin(ryuwHwConfig.resetGPIO);
    gpio_set_direction(ryuwHwConfig.resetGPIO, GPIO_MODE_OUTPUT);// Set the GPIO as a push/pull output 

}

esp_err_t RYUW22_Set_MODE (ryuw122_modo_t modo){

    char mesgAT[50];

    switch (modo)
    {
    case RYUW122_MODO_TAG:
        strcpy((char*)mesgAT,"AT+MODE=0");
        break;
    case RYUW122_MODO_ANCHOR:
        strcpy((char*)mesgAT,"AT+MODE=1");
        break;
    case RYUW122_MODO_SLEEP:
        strcpy((char*)mesgAT,"AT+MODE=2");
        break;
    }

    if (Send_AT_to_RYUW((char*)mesgAT, 200) == 1){
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t RYUW22_Set_Baud_Rate (ryuw122_ipr_t baud){
        
    char mesgAT[50];

    strcpy((char*)mesgAT,"AT+IPR=");
    switch (baud)
    {
    case RYUW122_IPR_9600:
        strcat((char*)mesgAT,"9600");
        break;
    case RYUW122_IPR_57600:
        strcat((char*)mesgAT,"57600");
        break;
    case RYUW122_IPR_115200:
        strcat((char*)mesgAT,"115200");
        break;
    case RYUW122_IPR_DEFAULT:
        strcat((char*)mesgAT,"115200");
        break;
    }

    if (Send_AT_to_RYUW((char*) mesgAT, 200) == 1){
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t RYUW22_Set_Channel (ryuw122_channel_t channel){
        
    char mesgAT[50];

    strcpy((char*)mesgAT,"AT+CHANNEL=");
    switch (channel)
    {
    case RYUW122_CHANNEL_6489:
        strcat((char*)mesgAT,"5");
        break;
    case RYUW122_CHANNEL_7987:
        strcat((char*)mesgAT,"9");
        break;
    case RYUW122_CHANNEL_DEFAULT:
        strcat((char*)mesgAT,"5");
        break;
    }

    if (Send_AT_to_RYUW((char*) mesgAT, 200) == 1){
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }    
}

esp_err_t RYUW22_Set_Bandwidth (ryuw122_channel_t band){
        
    char mesgAT[50];

    strcpy((char*)mesgAT,"AT+BANDWIDTH=");
    switch (band)
    {
    case RYUW122_BANDWIDTH_850Kb:
        strcat((char*)mesgAT,"0");
        break;
    case RYUW122_BANDWIDTH_68Mb:
        strcat((char*)mesgAT,"1");
        break;
    case RYUW122_BANDWIDTH_DEFAULT:
        strcat((char*)mesgAT,"0");
        break;
    }

    if (Send_AT_to_RYUW((char*) mesgAT, 200) == 1){
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }    
}

esp_err_t RYUW22_Set_Network_ID (char* networkId){
        
    char mesgAT[50];

    strcpy((char*)mesgAT,"AT+NETWORKID=");
    strcat((char*)mesgAT, networkId);

    if (Send_AT_to_RYUW((char*) mesgAT, 200) == 1){
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }    
}


esp_err_t RYUW22_Set_Address_ID (char* addressId){
        
    char mesgAT[50];

    strcpy((char*)mesgAT,"AT+ADDRESS=");
    strcat((char*)mesgAT, addressId);

    if (Send_AT_to_RYUW((char*) mesgAT, 200) == 1){
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }    
}