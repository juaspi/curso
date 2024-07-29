/**
 * 
 * @file      driver_oled_ssd1306.c
 * @brief     driver ssd1306 basic header file
 * @version   1.0.0
 * @author    Andres Machtey / modificado: Juan I Spinetto
 * @date      14/07/2024
 *
 */


/*********************
 *      INCLUDES
 *********************/

#include "driver_oled_ssd1306.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "font.h"

/*********************
 *      DEFINES
 *********************/

// Constants for I2c
#define I2C_MASTER_SDA_IO       0x03 
#define I2C_MASTER_SCL_IO       0X04
#define I2C_MASTER_NUM          I2C_NUM_0 // I2C_NUM_1 es una macro definida en ESP-IDF que representa el segundo controlador I2C disponible en el microcontrolador ESP32. Este controlador se utiliza para comunicaciones I2C adicionales junto con el primer controlador, I2C_NUM_0.

// Constants for oled
#define SSD1306_I2C_ADDR        0x3C
#define SSD1306_COMMAND         0x00
#define SSD1306_DATA            0x40



/*============================
 * LOCAL FUNTIONS PROTOPYTES
 *============================
 */

esp_err_t init_i2c(int sda,int scl);
esp_err_t oled_ssd1306_write_commands(uint8_t *commands, size_t size);
esp_err_t oled_ssd1306_write_data(uint8_t *data, size_t size);


/*=====================
 * FUNTIONS 
 *=====================
 */


/**
 * @brief     basic init de la pantalla oled
 * @return    ESP_OK
 *            ESP_FAIL
 * @note      inicializa el oled e ic2
 */

esp_err_t oled_ssd1306_init(void) 
{

    esp_err_t err;

    err = init_i2c(I2C_MASTER_SDA_IO,I2C_MASTER_SCL_IO);
    if (err== ESP_FAIL)
    {
        printf("Erro init_i2c");
        return ESP_FAIL;
    };
    printf("Success init_i2c");
    
    /*uint8_t init_commands[] = {
        0xAE, // Display Off
        0xD5, // Set Display Clock Divide Ratio/Oscillator Frequency
        0x80, // Suggested ratio
        0xA8, // Set Multiplex Ratio
        0x3F, // 1/64 duty
        0xD3, // Set Display Offset
        0x00, // No offset
        0x40, // Set Start Line
        0x8D, // Charge Pump Setting
        0x14, // Enable charge pump
        0x20, // Memory Addressing Mode
        0x00, // Horizontal addressing mode
        0xA1, // Set Segment Re-map
        0xC8, // Set COM Output Scan Direction
        0xDA, // Set COM Pins Hardware Configuration
        0x12,
        0x81, // Set Contrast Control
        0xCF,
        0xD9, // Set Pre-charge Period
        0xF1,
        0xDB, // Set VCOMH Deselect Level
        0x40,
        0xA4, // Entire Display ON (Resume)
        0xA6, // Normal Display
        0xAF  // Display ON
    };*/

        uint8_t init_commands[] = {
//         0xAE, // Display Off
        0xA8, // Set Multiplex Ratio
        0x3F, // 1/64 duty
        0xD3, // Set Display Offset
        0x00, // No offset 
        0x40, // Set Start Line
        0xA1, // Set Segment Re-map
        0xC8, // Set COM Output Scan Direction
        0xDA, // Set COM Pins Hardware Configuration
        0x12,   // ---------- DECI 0x12, segun hoja de datos es 02 
        0x81, // Set Contrast Control
        0xCF,   // ----------  DECIA 0xCF segun hoja de dato 7Fh
        0xA4, // Entire Display ON (Resume)
        0xA6, // Normal Display
        0xD5, // Set Display Clock Divide Ratio/Oscillator Frequency
        0x80, // Suggested ratio
        0x8D, // Charge Pump Setting
        0x14, // Enable charge pump
        0xAF  // Display ON

      /*  0x20, // Memory Addressing Mode
        0x00, // Horizontal addressing mode

        0xD9, // Set Pre-charge Period
        0xF1,
        0xDB, // Set VCOMH Deselect Level
        0x40,*/


    };

    oled_ssd1306_write_commands(init_commands, sizeof(init_commands));

    return ESP_OK;
}


/**
 * @brief  borra toda la información de la pantalla
 * @return none
 * @note   none
 */

void oled_ssd1306_clear_display(void)
{
    uint8_t set_addressing_Comands[] = {
            0x20,       //   Set Memory Addressing Mode
            0x02,       //   A[1:0] = 10b, Page Addressing Mode (RESET) 
            0x21,       //   Set Column Address
            0x00,       //   A[6:0] : Column start address, range : 0-127d
            0x7F,       //   Column end address, range : 0-127d
            0x00,       //   Set Lower Column Start Address for Page Addressing Mode
            0x10        //   Set Higher Column Start Address for Page Addressing Mode            
            };

    oled_ssd1306_write_commands(set_addressing_Comands, sizeof(set_addressing_Comands));

    uint8_t clear_data[128] = {0}; // Tamaño adecuado para una página (128 bytes)

    for (uint8_t page = 0; page < 8; page++)
    {
        uint8_t page_address_commands[] = {0xB0 | page }; // Set Page Start Address for Page Addressing Mode
        oled_ssd1306_write_commands(page_address_commands, sizeof(page_address_commands));  // Por ser 128 datos el contador puntero vuelve a 0 por lo que no es necesario enviar cmd 0x00 y 0x10 
        oled_ssd1306_write_data(clear_data, sizeof(clear_data));
    }  

}


/**
 * @brief  Escribe un caracter en una posición especifica 
 * @param[in] c es el caracter en ascii
 * @param[in] x es la coordenada x en pixels de 0 a 127
 * @param[in] y es la coordenada y en filas de 0 a 7
 * @return    none
 * @note      none
 */
void oled_ssd1306_draw_char(char c, int x, int y)
{
    if (c < 0x20 || c > 0x7F)
    {
        c = 0x20; // carácter inválido, usar espacio
    }
    uint8_t index = c - 0x20;
    uint8_t char_data[CHARS_COLS_LENGTH];

    for (uint8_t i = 0; i < CHARS_COLS_LENGTH; i++)
    {
        char_data[i] = FONTS[index][i];
    }

    uint8_t column_address_commands[] = {
        0x21,                     // Set Column Address
        x,                        // Start column address
        x + CHARS_COLS_LENGTH - 1 // End column address
    };
    oled_ssd1306_write_commands(column_address_commands, sizeof(column_address_commands));

    uint8_t page_address_commands[] = {
        0x22, // Set Page Address
        y,    // Start page address
        y     // End page address
    };
    oled_ssd1306_write_commands(page_address_commands, sizeof(page_address_commands));

    oled_ssd1306_write_data(char_data, CHARS_COLS_LENGTH);
}


/**
 * @brief  escribe un string a partir de una posición especifica
 * @param[in] str es el string a escribir 
 * @param[in] x es la coordenada x en pixels de 0 a 127
 * @param[in] y es la coordenada y en filas de 0 a 7
 * @return    none
 * @note      none
 */
void oled_ssd1306_draw_string(const char *str, int x, int y)
{
    // Dibujar cada carácter de la cadena
    while (*str)
    {
        oled_ssd1306_draw_char(*str++, x, y); // Dibujar el carácter actual y avanzar el puntero de la cadena
        x += CHARS_COLS_LENGTH;          // Avanzar la posición horizontal para el siguiente carácter
    }
}

/**
 * @brief  basic example clear
 * @param[in] x es la coordenada x en pixels de 0 a 127
 * @param[in] y es la coordenada 7 en pixels de 0 a 63
 * @return none
 * @note   none
 */
void oled_ssd1306_draw_pixel(int x, int y) {
    if (x >= 0 && x < 128 && y >= 0 && y < 64) {
        uint8_t page = y / 8;
        uint8_t bit_position = y % 8;

        uint8_t pixel_data[] = {0xB0 | page, (x & 0x0F), 0x10 | (x >> 4)};
        oled_ssd1306_write_commands(pixel_data, sizeof(pixel_data));

        uint8_t pixel[] = {1 << bit_position};
        oled_ssd1306_write_data(pixel, sizeof(pixel));
    }
}


esp_err_t init_i2c(int sda,int scl)
{

    #define I2C_MASTER_FREQ_HZ      400000          //ssd datasheet min clock cycle time 2.5 microS    
    #define I2C_MASTER_TX_BUF_DISABLE 0
    #define I2C_MASTER_RX_BUF_DISABLE 0

    esp_err_t err;

    int i2c_master_port = 0;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,            // select GPIO specific to your project
        .scl_io_num = I2C_MASTER_SCL_IO,            // select GPIO specific to your project    
        .sda_pullup_en = GPIO_PULLUP_ENABLE,        // type gpio_pullup_t: GPIO_PULLUP_DISABLE = 0x0,< Disable GPIO pull-up resistor>. GPIO_PULLUP_ENABLE = 0x1,< Enable GPIO pull-up resistor */
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ      // select frequency specific to your project
        // .clk_flags = 0,                          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };


    err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK)
    {
        printf("Fail - i2c_param_config");
        return ESP_FAIL;
    };
    printf("Success - i2c_param_config");

    err = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (err != ESP_OK)
    {
        printf("Fail - i2c_driver_install");
        return ESP_FAIL;
    };
    printf("Success - i2c_driver_install");   



        return ESP_OK;
}

esp_err_t oled_ssd1306_write_commands(uint8_t *commands, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, SSD1306_COMMAND, true);
    for (size_t i = 0; i < size; i++)
    {
        i2c_master_write_byte(cmd, commands[i], true);
    }
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t oled_ssd1306_write_data(uint8_t *data, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, SSD1306_DATA, true);
    //i2c_master_write(cmd, data, size, true);
   
     for (size_t i = 0; i < size; i++)
    {
        i2c_master_write_byte(cmd, data[i], true);
    }
      
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}