#include "buzzer.h"

/**Variáveis globais**********/

// Pino e slice PWM
static const uint8_t buzzer_pin = 21;
static uint slice;

// Rotina do buzzer / Temporizador
static struct repeating_timer timer;
static volatile bool buzzer_on = false;
static volatile bool buzzer_routine_on = false;

// Armazenamento do estado da iluminação a partir dos limites definidos
typedef enum lux_state
{
    ALTO,
    OK,
    BAIXO
} lux_state;
static lux_state last = OK;

/**Funções auxiliares**********/

// Obtém o estado do lux de acordo aos limites estabelecidos
static lux_state get_lux_state(uint16_t lux, uint16_t limite_min, uint16_t limite_max, uint16_t offset)
{
    // Retorna ao estado OK apenas após Lux ultrapassar o offset definido.
    // Isso evita que flutuações ao redor dos limites não gerem mudanças desnecessárias no estado do alarme.
    if (last != OK)
    {   
        // Se Lux ultrapassou offset do limite máximo
        if (last == ALTO && (limite_max - lux > offset))
        {   
            return OK;
        }

        // Se Lux ultrapassou offset do limite mínimo
        if (last == BAIXO && (lux - limite_min > offset))
        {   
            return OK;
        }
        return last; // Mantém alarme atual caso Lux esteja dentro do offset
    }

    // Aciona o estado de alarme, caso Lux tenha ultrapassado algum dos limites
    if (lux > limite_max)
    {   
        return ALTO;
    }
    if (lux < limite_min)
    {   
        return BAIXO;
    }
    return OK;
}

// Rotina de temporizador periódico para o buzzer
bool buzzer_routine(struct repeating_timer *t)
{
    buzzer_on = !buzzer_on;
    if (buzzer_on)
    {   
        pwm_set_gpio_level(buzzer_pin, PWM_WRAP / 2);
    }
    else
    {   
        pwm_set_gpio_level(buzzer_pin, 0);
    }

    return true;
}

/**Funções principais**********/

void init_buzzer()
{
    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(buzzer_pin);

    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_clkdiv(slice, PWM_DIV);
    pwm_set_gpio_level(buzzer_pin, 0);
    pwm_set_enabled(slice, true);
}

void buzzer_turn_off()
{   
    // Só desliga se houver rotina em execução
    if (buzzer_routine_on)
    {   
        cancel_repeating_timer(&timer);
        pwm_set_gpio_level(buzzer_pin, 0); // Garante que o buzzer está desligado
        buzzer_on = false;
        buzzer_routine_on = false;
    }
}

void buzzer_turn_on()
{   
    // Interrompe a rotina atual, se houver
    buzzer_turn_off();

    // Iniciar rotina de temporização
    buzzer_on = false;
    buzzer_routine_on = true;
    add_repeating_timer_ms(250, buzzer_routine, NULL, &timer);
}

void handle_buzzer_warning(uint16_t lux, uint16_t limite_min, uint16_t limite_max, uint16_t offset)
{
    lux_state curr = get_lux_state(lux, limite_min, limite_max, offset);
    if (curr != last)
    {
        last = curr;

        if (curr == ALTO)
        {
            buzzer_turn_on();
        }
        else if (curr == OK)
        {
            buzzer_turn_off();
        }
        else if (curr == BAIXO)
        {
            buzzer_turn_on();
        }
    }
}