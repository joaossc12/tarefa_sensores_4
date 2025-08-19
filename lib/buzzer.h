#ifndef BUZZER
#define BUZZER

#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

// Parâmetros para PWM
#define PWM_WRAP 1000
#define PWM_DIV 250.0

// Configura o PWM e inicializa o buzzer
void init_buzzer();

// Acionar/desligar buzzer
void buzzer_turn_off();
void buzzer_turn_on();

// Função para lidar com os alertas sonoros para os limites de brilho
void handle_buzzer_warning(uint16_t lux, uint16_t limite_min, uint16_t limite_max, uint16_t offset);

#endif