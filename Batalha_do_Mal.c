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
#include "inc/adc.h"

// I2C defines
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

#define LED_COUNT 25
#define LED_PIN 7
#define NUM_LEDS 25
#define BRIGHTNESS 50
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

// UART defines
#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 4
#define UART_RX_PIN 5

int joys[2];
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
    leds[index].R = (r * BRIGHTNESS) / 255;
    leds[index].G = (g * BRIGHTNESS) / 255;
    leds[index].B = (b * BRIGHTNESS) / 255;
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
    printf("Função de inicializar botão\n");
    gpio_init(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_pull_up(BUTTON_B_PIN);
    adc_init();
    adc_gpio_init(26);  // X-axis pin
    adc_gpio_init(27);  // Y-axis pin
}

void inicializarDisplay() {
    printf("Função de inicializar display\n");
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();
}

void inicializarLed() {
    printf("Função de inicializar LED\n");
    npInit(LED_PIN);
    for (int i = 0; i < LED_COUNT; i++) {
        npSetLED(i, 250, 156, 28); // Laranja com brilho reduzido
    }
    npWrite();
}

void esperarBotaoPressionado() {
    printf("Função de esperar botão pressionado\n");
    while (gpio_get(BUTTON_B_PIN)) {
        sleep_ms(100);
    }
}

void atualizaTela(char *text1, char *text2, char *text3) {
    printf("Função de atualizar tela\n");
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

void read_joystick(int joys[2]) {
    adc_select_input(0);  // Select X-axis (GPIO 26)
    joys[0] = 4095 - adc_read();  // Invert X-axis value
    adc_select_input(1);  // Select Y-axis (GPIO 27)
    joys[1] = 4095 - adc_read();  // Invert Y-axis value
}

void atualizaLedsComTabuleiro(int player2[5][5], int qualtab, int joys[2]) {
    int linha_cursor = joys[0] * 5 / 4096;  // Map X-axis to 0-4
    int coluna_cursor = joys[1] * 5 / 4096; // Map Y-axis to 0-4

    if (qualtab == 1) {
        // Fase de posicionamento de barcos (mostra os barcos)
        for (int linha = 0; linha < 5; linha++) {
            for (int coluna = 0; coluna < 5; coluna++) {
                int index = linha * 5 + coluna; // Mapeia a posição para o índice do LED

                // Se tiver um barco, acende o LED com a cor vermelha
                if (player2[linha][coluna] == 1) {
                    npSetLED(index, 255, 0, 0);  // Vermelho com brilho reduzido
                } else {
                    npSetLED(index, 0, 255, 0);  // Verde com brilho reduzido
                }
            }
        }
    } else {
        // Fase de ataque (esconde os barcos, mostra apenas acertos e erros)
        // Garantir que o valor do joystick não ultrapasse o limite da matriz
        linha_cursor = linha_cursor < 0 ? 0 : (linha_cursor > 4 ? 4 : linha_cursor);
        coluna_cursor = coluna_cursor < 0 ? 0 : (coluna_cursor > 4 ? 4 : coluna_cursor);

        for (int linha = 0; linha < 5; linha++) {
            for (int coluna = 0; coluna < 5; coluna++) {
                int index = linha * 5 + coluna; // Mapeia a posição para o índice do LED

                // Se for a posição do cursor, muda para laranja
                if (linha == linha_cursor && coluna == coluna_cursor) {
                    npSetLED(index, 255, 165, 0);  // Laranja com brilho reduzido
                } else if (player2[linha][coluna] == -2) {
                    npSetLED(index, 255, 255, 255);  // Branco com brilho reduzido
                } else {
                    npSetLED(index, 0, 125, 255);  // Azul claro com brilho reduzido
                }
            }
        }
    }

    npWrite(); // Aplica as mudanças nos LEDs
}

void posicaoDeBarcos(int player2[5][5]) {
    printf("função de posiciona barco\n");

    for (int i = 0; i < 4; i++) {
        int linha, coluna1, linha1;
        char coluna;

        atualizaTela("Posicione", "seus barcos", "");
        imprimeTabuleiro(player2, 0);  // Atualiza na tela

        do {
            scanf("%d %c", &linha, &coluna);
            coluna1 = coluna - 'A';
            linha1 = linha - 1;

            if (!validaEntradaLinhaColuna(linha, coluna) || !validaPosicao(player2, 1, linha1, coluna1, 'V')) {
                printf("Posicao indisponivel! Tente novamente.\n");
            }
        } while (!validaEntradaLinhaColuna(linha, coluna) || !validaPosicao(player2, 1, linha1, coluna1, 'V'));

        // Marcar a área ao redor como indisponível (-1)
        // Como a matriz é 5x5, usamos limite < 5
        for (int m = linha1 - 1; m <= linha1 + 1; m++) {
            for (int n = coluna1 - 1; n <= coluna1 + 1; n++) {
                if (m >= 0 && m < 5 && n >= 0 && n < 5) {
                    player2[m][n] = -1;
                }
            }
        }
        // Definir a posição do barco
        player2[linha1][coluna1] = 1;

        atualizaLedsComTabuleiro(player2, 1, (int[]){-1, -1}); // Atualiza os LEDs com o estado do tabuleiro
    }

    imprimeTabuleiro(player2, 0);
}

bool botao_selecao_pressionado() {
    return !gpio_get(BUTTON_A_PIN);  // Retorna true se o botão foi pressionado
}

bool validaEntradaColunaLinha(int linha, int coluna) {
    return (linha >= 0 && linha < 5 && coluna >= 0 && coluna < 5);
}

void executarTurno(int *pts1, int *jogadas, int player2[5][5], int joys[2]) {
    char mensagem[16];
    sprintf(mensagem, "Pontos: %d", *pts1);
    atualizaTela("VEZ DO JOGADOR 1", "", mensagem);
    printf("função de executar turno\n");

    int linha_cursor, coluna_cursor;
    bool jogada_realizada = false;

    while (!jogada_realizada) {
        // Ler o joystick continuamente
        read_joystick(joys);
        linha_cursor = joys[0] * 5 / 4096;  // Map X-axis to 0-4
        coluna_cursor = joys[1] * 5 / 4096; // Map Y-axis to 0-4

        // Garantir que o cursor não ultrapasse os limites da matriz
        linha_cursor = linha_cursor < 0 ? 0 : (linha_cursor > 4 ? 4 : linha_cursor);
        coluna_cursor = coluna_cursor < 0 ? 0 : (coluna_cursor > 4 ? 4 : coluna_cursor);

        // Atualizar a tela e os LEDs com o estado atual do tabuleiro
        imprimeTabuleiro(player2, 1); // Atualiza na tela (oculta)
        atualizaLedsComTabuleiro(player2, 2, joys); // Atualiza os LEDs

        // Verificar se o botão de seleção foi pressionado
        if (botao_selecao_pressionado()) {
            // Verificar se a posição é válida e pode atirar
            if (validaEntradaColunaLinha(linha_cursor, coluna_cursor) && podeAtirar(player2, linha_cursor, coluna_cursor)) {
                atirar(player2, linha_cursor, coluna_cursor);

                // Marcar a célula com um acerto
                if (player2[linha_cursor][coluna_cursor] == 1 || player2[linha_cursor][coluna_cursor] == 2 || player2[linha_cursor][coluna_cursor] == 3) {
                    player2[linha_cursor][coluna_cursor] = -2;  // Marca o acerto (X)
                    *pts1 += calculaPontuacao(player2, linha_cursor, coluna_cursor);
                }
                jogada_realizada = true;  // Jogada foi realizada
            } else {
                atualizaTela("Posicao", "invalida!", "");
                sleep_ms(1000);
            }
        }

        sleep_ms(100);  // Pequeno delay para evitar leitura excessiva
    }

    (*jogadas)++;  // Incrementa o número de jogadas

    // Verificar estado da matriz após atualização
    printf("Matriz após atualização:\n");
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%d ", player2[i][j]);
        }
        printf("\n");
    }

    // Atualiza os LEDs após o turno
    atualizaLedsComTabuleiro(player2, 2, joys);
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
    printf("matriz finalizada :\n");

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%d ", player2[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("função de antes do while\n");
    while (jogadas < 41 && pts1 < 42) {
        executarTurno(&pts1, &jogadas, player2, joys);
    }

    atualizaTela("FIM DE JOGO!", "", "");
    return 0;
}