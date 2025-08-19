#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


#include "lib/gy_33.h"
#include "lib/bh1750_light_sensor.h"
#include "lib/perifericos.h"
#include "lib/ssd1306.h"
#include "lib/buzzer.h"

// Leds

#define LED_RED       13
#define LED_BLUE      12
#define LED_GREEN     11

// Limites e Offset para o Lux e alerta sonoro do buzzer
#define MAX_LUX 1000
#define MIN_LUX 10
#define OFFSET_LUX 5

//Variáveis globais para utilização da matriz de leds
PIO pio = pio0;
uint offset;
uint sm;

// Varável para alternar LEDS

uint flag_leds = 0;

ssd1306_t ssd;

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
#define BUTTON_A      5
void gpio_irq_handler(uint gpio, uint32_t events); //Função de callback dos botões
void Control_Leds(uint flag); //Função de controle dos Leds

int main()
{
    stdio_init_all();

    //Inicia Leds
    gpio_init(LED_BLUE);
    gpio_init(LED_GREEN);
    gpio_init(LED_RED);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

     // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Fim do trecho para modo BOOTSEL com botão B

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

        sprintf(str_red, "%d", r);   // Converte o inteiro em string
        sprintf(str_green, "%d", g); 
        sprintf(str_blue, "%d", b);  
        sprintf(str_clear, "%d", c);

         Control_Leds(flag_leds);

        //Aciona a matriz de acordo com os valores lidos
        draw_on_matrix(pio, sm, lux, r, g, b);

        // Desenha os valores no display OLED
        draw_display(&ssd, str_red, str_green, str_blue, str_clear, str_lux);

        // Liga/Desliga alerta sonoro de limites do Lux
        handle_buzzer_warning(lux, MIN_LUX, MAX_LUX, OFFSET_LUX);

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
void Control_Leds(uint flag){
    switch (flag)
    {
    case 1:
        gpio_put(LED_BLUE, 0);
        gpio_put(LED_GREEN, 0);
        gpio_put(LED_RED, 1);
        break;
    case 2:
        gpio_put(LED_BLUE, 1);
        gpio_put(LED_GREEN, 0);
        gpio_put(LED_RED, 0);
        break;
    case 3:
        gpio_put(LED_BLUE, 0);
        gpio_put(LED_GREEN, 1);
        gpio_put(LED_RED, 0);
        break; 


    default:
        gpio_put(LED_BLUE, 0);
        gpio_put(LED_GREEN, 0);
        gpio_put(LED_RED, 0);
        break;
    }
}
void gpio_irq_handler(uint gpio, uint32_t events)
{
    static absolute_time_t last_time_A = 0;
    static absolute_time_t last_time_B = 0;
    absolute_time_t now = get_absolute_time();

    if (gpio == BUTTON_A) {
        if (absolute_time_diff_us(last_time_A, now) > 200000) { // 200ms debounce
            flag_leds = (flag_leds + 1)%4;
            }
            last_time_A = now;}

    if (gpio == botaoB) {
        if (absolute_time_diff_us(last_time_A, now) > 200000) { // 200ms debounce
            reset_usb_boot(0, 0);
            }
            last_time_B = now;}
}