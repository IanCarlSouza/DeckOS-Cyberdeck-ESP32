#ifndef CONFIGURACAO_H
#define CONFIGURACAO_H

#include <Arduino.h>

// --- CONFIGURAÇÕES DO DISPLAY OLED ---
#define LARGURA_TELA 128
#define ALTURA_TELA 64
#define ENDERECO_OLED 0x3C
#define PINO_RESET_OLED -1

// --- MAPEAMENTO DE PINOS (PORTUGUÊS BRASIL) ---
// LED RGB
#define PINO_LED_VERMELHO 19
#define PINO_LED_VERDE    18
#define PINO_LED_AZUL     4

// Botoes de Navegação
#define PINO_BOTAO_ANTERIOR  33  
#define PINO_BOTAO_SELECIONAR 25  
#define PINO_BOTAO_PROXIMO   26  
#define PINO_BOTAO_VOLTAR    27  

// Buzzer
#define PINO_BUZZER          14

// --- MÁQUINA DE ESTADOS ---
enum EstadoSistema {
    ESTADO_TELA_ABERTURA,
    ESTADO_MENU_PRINCIPAL,
    ESTADO_APP_WIFI,
    ESTADO_APP_JOGOS,
    ESTADO_APP_MUSICAS,
    ESTADO_APP_CONECTAR_PC,
    ESTADO_APP_HARDWARE,
    ESTADO_APP_FIRMWARE
};

// --- VERSÃO DO DECKOS ---
const String VERSAO_FIRMWARE = "v1.0.0 Alpha";
const String URL_GITHUB = "https://github.com/usuario/DeckOS";

#endif