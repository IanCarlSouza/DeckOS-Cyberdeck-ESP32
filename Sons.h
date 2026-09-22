#ifndef SONS_H
#define SONS_H

#include <Arduino.h>
#include "Configuracao.h"

class GerenciadorSom {
public:
    static void inicializar() {
        pinMode(PINO_BUZZER, OUTPUT);
        digitalWrite(PINO_BUZZER, LOW);
    }

    static void emitirSomClique() {
        tone(PINO_BUZZER, 2000, 30);
    }

    
    static void emitirSomConfirmar() {
        tone(PINO_BUZZER, 2800, 50);
    }

    static void emitirSomVoltar() {
        tone(PINO_BUZZER, 1200, 50);
    }
};

#endif