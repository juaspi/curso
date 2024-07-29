/* Main

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"

#include "driver_RYUW122.h"
#include "driver_oled_ssd1306.h"
#include "driver_touch.h"

void NumToStr (char *str, int numero){
    
  int aux;
  
  strcpy(str,"    0");
  for (int i=4; i>=0; i--)
  {
    if (numero>0){
      aux = numero % 10;
      str[i] = (char) aux+48;
      numero /=10;
    }
  }
}

void app_main(void) {

    int result;
    int filtered = 0;
    char distancia[12];
    bool midiendo = false;
    esp_err_t return_value;
    
    return_value = init_touch_button();
    if (return_value != ESP_OK) {
        printf("Init touch Button fail\n");
    }    

    if (oled_ssd1306_init()== ESP_OK)
    {
        printf ("Init OLED OK\n");
        oled_ssd1306_clear_display();
        oled_ssd1306_draw_string("En espera ...", 0, 4);
    }

    if (RYUW122_Init_Default() == ESP_OK){
      printf ("Init RYUW122 OK\n");
      while(1)
      {
        if (touch_button_press_release()){
          oled_ssd1306_clear_display(); 
          midiendo = !midiendo;
          if (midiendo){
            oled_ssd1306_draw_string("Distancia:      cm", 0, 4);            
          } else {
            oled_ssd1306_draw_string("En espera ...", 0, 4);
          };
        } 
        vTaskDelay(100 / portTICK_PERIOD_MS);
        if (midiendo){
          result = Tomar_Distancia_Actual();
          if (result==-1){
            midiendo =false;
            oled_ssd1306_clear_display(); 
            oled_ssd1306_draw_string("En espera ...", 0, 4);
            oled_ssd1306_draw_string("El TAG no responde", 0, 6);                
          } else{
            filtered = (filtered * 9 + result)/10;
            NumToStr(&distancia, result);
            oled_ssd1306_draw_string(distancia, 60, 4); 
          }            
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }  
    } else {printf ("Init RYUW122 Fallo\n");}
}
