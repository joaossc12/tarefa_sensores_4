#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


#include "lib/gy_33.h"
#include "lib/bh1750_light_sensor.h"


int main()
{
    stdio_init_all();

    // Inicializa o I2C0
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);


    //Inicializa o o sensor de luz BH1750
    bh1750_power_on(I2C_PORT);
    char str_lux[10];  // Buffer para armazenar a string
    //Incializa o sensor de cor gy33
    gy33_init();
    char str_red[5];   // Buffer para armazenar a string
    char str_green[5]; 
    char str_blue[5];  
    char str_clear[5]; 



    while (true) {

        uint16_t lux = bh1750_read_measurement(I2C_PORT);
        printf("Lux = %d\n", lux);
        sprintf(str_lux, "%d Lux", lux);  // Converte o inteiro em string
        sleep_ms(100);

        uint16_t r, g, b, c;
        gy33_read_color(&r, &g, &b, &c);
        printf("Cor detectada - R: %d, G: %d, B: %d, Clear: %d\n", r, g, b, c);

        sprintf(str_red, "%d R", r);   // Converte o inteiro em string
        sprintf(str_green, "%d G", g); 
        sprintf(str_blue, "%d B", b);  
        sprintf(str_clear, "%d C", c);

        sleep_ms(500);
    }
}
