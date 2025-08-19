#include "perifericos.h"

uint32_t rgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

void alter_brightness(uint16_t lux, uint8_t *r, uint8_t *g, uint8_t *b)
{
    //Normaliza o valor do brilho para que fique entre 0 e 255
    uint8_t brightness = lux > 1000 ? 255 : (lux * 255 / 1000);

    //Ajusta as cores de acordo com o brilho obtido
    (*r) = ((*r) * brightness) / 255;
    (*g) = ((*g) * brightness) / 255;
    (*b) = ((*b) * brightness) / 255;
}

void draw_on_matrix(PIO pio, uint sm, uint16_t lux, uint8_t r, uint8_t g, uint8_t b)
{
    alter_brightness(lux, &r, &g, &b);

    for (int i = 0; i < NUM_PIXELS; i++)
    {
        pio_sm_put_blocking(pio, sm, rgb_u32(r, g, b));
    }
}