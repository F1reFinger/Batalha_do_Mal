#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"
#include "ws2818b.pio.h"
#include <math.h>
#include "ships.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

#define LED_COUNT 25
#define LED_PIN 7
#define NUM_LEDS 25
#define BRIGHTNESS 255
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

int eixox = 2, eixoy = 2;
struct pixel_t {
    uint8_t G, R, B;
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t;

npLED_t leds[LED_COUNT];

PIO np_pio;
uint sm;

void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;

    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true);
    }

    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

    for (uint i = 0; i < LED_COUNT; ++i) {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);
}

void npWrite() {
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}

void set_led_color(int index, uint8_t r, uint8_t g, uint8_t b) {
    npSetLED(index, r, g, b);
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    printf("Ó o alarme!!!!\n");
    return 0;
}

void inicializarBotoes() {
    printf("função de inicia botão\n");
    gpio_init(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_pull_up(BUTTON_B_PIN);
    gpio_init(eixox);
    gpio_init(eixoy);
    gpio_pull_up(eixox);
    gpio_pull_up(eixoy);
}

void inicializarDisplay() {
    printf("função de inicia display\n");
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();
}

void inicializarLed() {
    printf("função de inicia led\n");
    npInit(LED_PIN);
    for (int i = 0; i < LED_COUNT; i++) {
        npSetLED(i, 250, 156, 28); // Orange for all LEDs
    }
    npWrite();
}

void esperarBotaoPressionado() {
    printf("função de esperar botão\n");
    while (gpio_get(BUTTON_B_PIN)) {
        sleep_ms(100);
    }
}

void atualizaTela(char *text1, char *text2, char *text3) {
    printf("função de atualiza tela\n");
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    ssd1306_draw_string(ssd, 5, 0, text1);
    ssd1306_draw_string(ssd, 5, 8, text2);
    ssd1306_draw_string(ssd, 5, 16, text3);
    render_on_display(ssd, &frame_area);
}

void posicaoDeBarcos(int player2[5][5]) {
    printf("função de posiciona barco\n");
    for (int i = 0; i < 4; i++) {
        atualizaTela("Posicione", "seus barcos", "");
        imprimeTabuleiro(player2, 0);
        posicionaBarco(player2, 1);
    }
}

void executarTurno(int *pts1, int *jogadas, int player2[5][5]) {
    char mensagem[16];
    sprintf(mensagem, "Pontos: %d", *pts1);
    atualizaTela("VEZ DO JOGADOR 1", "", mensagem);
    printf("função de executar turno\n");

    imprimeTabuleiro(player2, 1);

    int pos = 0, kleber = 0;
    while (gpio_get(BUTTON_A_PIN)) { sleep_ms(100); }
    pos = (pos + 1) % 5;

    while (gpio_get(BUTTON_B_PIN)) { sleep_ms(100); }
    kleber = (kleber + 1) % 5;

    if (validaEntradaLinhaColuna(pos, kleber) && podeAtirar(player2, pos, kleber)) {
        atirar(player2, pos, kleber);
        *pts1 += calculaPontuacao(player2, pos, kleber);
    } else {
        atualizaTela("Posicao", "invalida!", "");
        sleep_ms(1000);
    }
    (*jogadas)++;
}

int main() {
    printf("iniciou a main\n");
    stdio_init_all();

    inicializarBotoes();
    inicializarDisplay();
    inicializarLed();

    atualizaTela("Iniciando jogo...", "", "");
    sleep_ms(1000);

    npClear();
    npWrite();

    int player2[5][5] = {0};
    int jogadas = 0, pts1 = 0;

    esperarBotaoPressionado();
    atualizaTela("Iniciando Jogo!", "", "");

    posicaoDeBarcos(player2);
    printf("função de antes do while\n");
    while (jogadas < 41 && pts1 < 42) {
        executarTurno(&pts1, &jogadas, player2);
    }

    atualizaTela("FIM DE JOGO!", "", "");
    return 0;
}

