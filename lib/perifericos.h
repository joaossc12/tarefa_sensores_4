#ifndef PERIFERICOS_H_INCLUDED
#define PERIFERICOS_H_INCLUDED

#include "hardware/pio.h"
#include "pico/stdlib.h"

#define MATRIX 7
#define NUM_PIXELS 25

/**
 * @brief Inicializa a matriz de leds rgb
 */
void init_matrix();

/**
 * @brief Reorganiza os bytes dos valores de r,g,b para um único valor de 32 bits
 */
uint32_t rgb_u32(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Altera o brilho de acordo com a luminosidade
 */
void alter_brightness(uint16_t lux, uint8_t *r, uint8_t *g, uint8_t *b);

/**
 * @brief Acende a matriz de LEDS
 */
void draw_on_matrix(PIO pio, uint sm, uint16_t lux, uint8_t r, uint8_t g, uint8_t b);

#endif //PERIFERICOS_H_INCLUDED