#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"

#include "pio_matrix.pio.h"

#include "lib/gy_33.h"
#include "lib/bh1750_light_sensor.h"
#include "lib/perifericos.h"

//Definição para uso do segundo sensor (conectado ao i2c1)
#define I2C1_PORT i2c1
#define I2C1_SDA 2
#define I2C1_SCL 3

//Variáveis globais para utilização da matriz de leds
PIO pio = pio0;
uint offset;
uint sm;

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

int main()
{
    stdio_init_all();

     // Para ser utilizado o modo BOOTSEL com botão B
     gpio_init(botaoB);
     gpio_set_dir(botaoB, GPIO_IN);
     gpio_pull_up(botaoB);
     gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Fim do trecho para modo BOOTSEL com botão B

    // Inicializa o I2C0
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

     // Inicializa o I2C1
     i2c_init(I2C1_PORT, 400 * 1000);
     gpio_set_function(I2C1_SDA, GPIO_FUNC_I2C);
     gpio_set_function(I2C1_SCL, GPIO_FUNC_I2C);
     gpio_pull_up(I2C1_SDA);
     gpio_pull_up(I2C1_SCL);


    //Inicializa o o sensor de luz BH1750
    bh1750_power_on(I2C1_PORT);
    char str_lux[10];  // Buffer para armazenar a string
    //Incializa o sensor de cor gy33
    gy33_init();
    char str_red[5];   // Buffer para armazenar a string
    char str_green[5]; 
    char str_blue[5];  
    char str_clear[5]; 

    //Inicializa a matriz de leds
    init_matrix();

    while (true) {

        uint16_t lux = bh1750_read_measurement(I2C1_PORT);
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

        //Aciona a matriz de acordo com os valores lidos
        draw_on_matrix(pio, sm, lux, r, g, b);

        sleep_ms(500);
    }
}

//Inicializa a matriz de LEDS
void init_matrix()
{
    offset = pio_add_program(pio, &pio_matrix_program);
    sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, MATRIX);
}
