#ifndef PLAYER_MUSICA_H
#define PLAYER_MUSICA_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "Configuracao.h"

// ============================================================================
// DEFINIÇÃO DAS FREQUÊNCIAS DAS NOTAS MUSICAIS (Hz)
// ============================================================================
#define REST     0

// Oitava 3
#define NOTE_AS3 233
#define NOTE_B3  247

// Oitava 4
#define NOTE_C4  261
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  329
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  391
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 455
#define NOTE_B4  466

// Oitava 5
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 830
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

// Oitava 6
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175

// Estrutura de cada faixa
struct Faixa {
    const char* titulo;
    const char* artista;
    const int* notas;
    const int* duracoes;
    int totalNotas;
    int bpm;
};

// ============================================================================
// BANCO DE DADOS DE MÚSICAS
// ============================================================================

// 1. Super Mario Bros
const int melodia_mario[] = { NOTE_E5, NOTE_E5, REST, NOTE_E5, REST, NOTE_C5, NOTE_E5, REST, NOTE_G5, REST, NOTE_G4 };
const int duracao_mario[] = { 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4 };

// 2. Tetris Theme
const int melodia_tetris[] = { NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4 };
const int duracao_tetris[] = { 4, 8, 8, 4, 8, 8, 4, 8, 8, 4, 8, 8, 4 };

// 3. Imperial March (Star Wars - Darth Vader)
const int melodia_imperial[] = {

    NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4, REST,
    NOTE_E5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_C5, NOTE_GS4, NOTE_F4, NOTE_C5, NOTE_A4, REST,

    NOTE_A5, NOTE_A4, NOTE_A4, NOTE_A5, NOTE_GS5, NOTE_G5, NOTE_FS5, NOTE_F5, NOTE_FS5, REST,
    NOTE_AS4, NOTE_DS5, NOTE_D5, NOTE_CS5, NOTE_C5, NOTE_B4, NOTE_C5, REST,

    NOTE_F4, NOTE_GS4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, NOTE_E5, REST,

    NOTE_A5, NOTE_A4, NOTE_A4, NOTE_A5, NOTE_GS5, NOTE_G5, NOTE_FS5, NOTE_F5, NOTE_FS5, REST,

    NOTE_F4, NOTE_GS4, NOTE_F4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4, REST
};

const int duracao_imperial[] = {
    4, 4, 4, 6, 16, 4, 6, 16, 2, 4,
    4, 4, 4, 6, 16, 4, 6, 16, 2, 4,

    4, 6, 16, 4, 6, 16, 16, 16, 8, 8,
    8, 4, 6, 16, 16, 16, 8, 8,

    8, 4, 6, 16, 4, 6, 16, 2, 4,

    4, 6, 16, 4, 6, 16, 16, 16, 8, 8,
    8, 4, 6, 16, 16, 16, 8, 8,

    8, 4, 6, 16, 4, 6, 16, 2, 4
};

// 4. Megalovania 
const int melodia_megalovania[] = {
    NOTE_D4, NOTE_D4, NOTE_D5, REST, NOTE_A4, REST, NOTE_GS4, REST, NOTE_G4, REST, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4,
    NOTE_C4, NOTE_C4, NOTE_D5, REST, NOTE_A4, REST, NOTE_GS4, REST, NOTE_G4, REST, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4,
    NOTE_B3, NOTE_B3, NOTE_D5, REST, NOTE_A4, REST, NOTE_GS4, REST, NOTE_G4, REST, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4,
    NOTE_AS3, NOTE_AS3, NOTE_D5, REST, NOTE_A4, REST, NOTE_GS4, REST, NOTE_G4, REST, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4,
    
    NOTE_D5, NOTE_D5, NOTE_D6, REST, NOTE_A5, REST, NOTE_GS5, REST, NOTE_G5, REST, NOTE_F5, NOTE_D5, NOTE_F5, NOTE_G5,
    NOTE_C5, NOTE_C5, NOTE_D6, REST, NOTE_A5, REST, NOTE_GS5, REST, NOTE_G5, REST, NOTE_F5, NOTE_D5, NOTE_F5, NOTE_G5,
    NOTE_B4, NOTE_B4, NOTE_D6, REST, NOTE_A5, REST, NOTE_GS5, REST, NOTE_G5, REST, NOTE_F5, NOTE_D5, NOTE_F5, NOTE_G5,
    NOTE_AS4, NOTE_AS4, NOTE_D6, REST, NOTE_A5, REST, NOTE_GS5, REST, NOTE_G5, REST, NOTE_F5, NOTE_D5, NOTE_F5, NOTE_G5
};

const int duracao_megalovania[] = {
    16, 16, 16, 16, 16, 8, 16, 16, 16, 16, 8, 16, 16, 16,
    16, 16, 16, 16, 16, 8, 16, 16, 16, 16, 8, 16, 16, 16,
    16, 16, 16, 16, 16, 8, 16, 16, 16, 16, 8, 16, 16, 16,
    16, 16, 16, 16, 16, 8, 16, 16, 16, 16, 8, 16, 16, 16,

    16, 16, 16, 16, 16, 8, 16, 16, 16, 16, 8, 16, 16, 16,
    16, 16, 16, 16, 16, 8, 16, 16, 16, 16, 8, 16, 16, 16,
    16, 16, 16, 16, 16, 8, 16, 16, 16, 16, 8, 16, 16, 16,
    16, 16, 16, 16, 16, 8, 16, 16, 16, 16, 8, 16, 16, 16
};


class PlayerMusica {
private:
    Faixa playlist[4] = {
        {"Super Mario",     "Koji Kondo",    melodia_mario,       duracao_mario,       sizeof(melodia_mario)/sizeof(int),       150},
        {"Tetris Theme",    "Hirokazu",      melodia_tetris,      duracao_tetris,      sizeof(melodia_tetris)/sizeof(int),      140},
        {"Imperial March",  "John Williams", melodia_imperial,    duracao_imperial,    sizeof(melodia_imperial)/sizeof(int),    120},
        {"Megalovania",     "Toby Fox",      melodia_megalovania, duracao_megalovania, sizeof(melodia_megalovania)/sizeof(int), 120}
    };

    const int TOTAL_FAIXAS = 4;
    int faixaAtual = 0;
    bool tocando = false;
    
    int notaAtual = 0;
    unsigned long ultimoTempoNota = 0;

public:
    void inicializar() {
        pinMode(PINO_BUZZER, OUTPUT);
        faixaAtual = 0;
        tocando = false;
        notaAtual = 0;
        noTone(PINO_BUZZER);
    }

    void alternarPlayPause() {
        tocando = !tocando;
        if (!tocando) {
            noTone(PINO_BUZZER);
        }
    }

    void proximaFaixa() {
        faixaAtual = (faixaAtual + 1) % TOTAL_FAIXAS;
        notaAtual = 0;
        noTone(PINO_BUZZER);
    }

    void faixaAnterior() {
        faixaAtual = (faixaAtual - 1 + TOTAL_FAIXAS) % TOTAL_FAIXAS;
        notaAtual = 0;
        noTone(PINO_BUZZER);
    }

    void parar() {
        tocando = false;
        notaAtual = 0;
        noTone(PINO_BUZZER);
    }

    void atualizar() {
        if (!tocando) return;

        Faixa &musica = playlist[faixaAtual];
        unsigned long tempoAtualMillis = millis();

        int duracaoNotaMs = (60000 / musica.bpm) / (musica.duracoes[notaAtual] / 4.0);

        if (tempoAtualMillis - ultimoTempoNota >= (unsigned long)duracaoNotaMs) {
            ultimoTempoNota = tempoAtualMillis;

            int freq = musica.notas[notaAtual];
            if (freq > 0) {
                tone(PINO_BUZZER, freq);
            } else {
                noTone(PINO_BUZZER);
            }

            notaAtual = (notaAtual + 1) % musica.totalNotas;
        }
    }

    void desenharInterface(Adafruit_SSD1306 &display) {
        display.clearDisplay();

        // Cabeçalho
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(12, 0);
        display.print(F("=== PLAYER DE MUSICA ==="));

        // Nome da Música
        display.setCursor(0, 14);
        display.printf("[%d/%d] ", faixaAtual + 1, TOTAL_FAIXAS);
        display.println(playlist[faixaAtual].titulo);

        // Artista
        display.setCursor(0, 26);
        display.print(F("Art: "));
        display.println(playlist[faixaAtual].artista);

        // Status
        display.setCursor(0, 38);
        if (tocando) {
            display.print(F("Status: [>] TOCANDO"));
        } else {
            display.print(F("Status: [||] PAUSADO"));
        }

        // Barra de Progresso
        int larguraBarra = map(notaAtual, 0, playlist[faixaAtual].totalNotas - 1, 0, 124);
        display.drawRect(0, 50, 128, 8, SSD1306_WHITE);
        display.fillRect(2, 52, larguraBarra, 4, SSD1306_WHITE);

        display.display();
    }
};

#endif