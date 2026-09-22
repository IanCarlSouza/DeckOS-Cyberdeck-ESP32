#ifndef BOTOES_H
#define BOTOES_H

#include <Arduino.h>
#include "Configuracao.h"
#include "Sons.h"

// Tipos de eventos de clique possíveis
enum class TipoClique : uint8_t {
    NENHUM,
    SIMPLES,
    DUPLO,
    LONGO
};

struct ControleBotao {
    uint8_t pino;
    bool estadoAnterior = false;            // Estado debouncado (true = LOW/pressionado)
    bool ultimaLeituraBruta = false;        // Leitura crua do pino para debounce
    unsigned long ultimoTempoDebounce = 0;   // Timer do debounce

    // Variáveis para clique duplo e longo
    unsigned long tempoPressionado = 0;     // Momento em que o botão foi apertado
    unsigned long tempoSolto = 0;           // Momento em que o botão foi solto
    uint8_t contagemCliques = 0;            // Contador de cliques para o duplo clique
    bool longoProcessado = false;           // Evita repetir o clique longo
    TipoClique eventoPendente = TipoClique::NENHUM; // Guarda o evento detectado
};

class GerenciadorBotoes {
private:
    ControleBotao botaoAnterior;
    ControleBotao botaoSelecionar;
    ControleBotao botaoProximo;
    ControleBotao botaoVoltar;

    // Configurações de tempo (em milissegundos)
    const unsigned long atrasoDebounce = 50;     // 50ms para estabilizar a leitura
    const unsigned long tempoCliqueLongo = 800;   // 800ms pressionado = Clique Longo
    const unsigned long janelaCliqueDuplo = 250;  // 250ms após soltar para esperar 2º clique

    void processarBotao(ControleBotao &botao) {
        bool leituraBruta = (digitalRead(botao.pino) == LOW);
        unsigned long agora = millis();

        // 1. Tratamento de Debounce
        if (leituraBruta != botao.ultimaLeituraBruta) {
            botao.ultimoTempoDebounce = agora;
            botao.ultimaLeituraBruta = leituraBruta;
        }

        if ((agora - botao.ultimoTempoDebounce) > atrasoDebounce) {
            // Mudança de estado confirmada
            if (leituraBruta != botao.estadoAnterior) {
                botao.estadoAnterior = leituraBruta;

                if (botao.estadoAnterior) {
                    // BOTÃO FOI APERTADO
                    botao.tempoPressionado = agora;
                    botao.longoProcessado = false;
                    botao.contagemCliques++;
                    GerenciadorSom::emitirSomClique();
                } else {
                    // BOTÃO FOI SOLTO
                    botao.tempoSolto = agora;
                    if (botao.longoProcessado) {
                        // Se já foi tratado como longo ao segurar, reseta a contagem
                        botao.contagemCliques = 0;
                    }
                }
            }
        }

        // 2. Detecção de Clique Longo (enquanto ainda está pressionado)
        if (botao.estadoAnterior && !botao.longoProcessado) {
            if (agora - botao.tempoPressionado >= tempoCliqueLongo) {
                botao.eventoPendente = TipoClique::LONGO;
                botao.longoProcessado = true;
                botao.contagemCliques = 0; // Cancela clique simples/duplo
            }
        }

        // 3. Detecção de Clique Simples ou Duplo (após o botão ser solto)
        if (!botao.estadoAnterior && botao.contagemCliques > 0) {
            if (botao.contagemCliques == 1) {
                // Aguarda a janela do duplo clique expirar para confirmar que foi apenas 1 clique
                if (agora - botao.tempoSolto >= janelaCliqueDuplo) {
                    botao.eventoPendente = TipoClique::SIMPLES;
                    botao.contagemCliques = 0;
                }
            } else if (botao.contagemCliques >= 2) {
                // Dois cliques dentro do tempo limite confirmam o duplo clique
                botao.eventoPendente = TipoClique::DUPLO;
                botao.contagemCliques = 0;
            }
        }
    }

    // Função genérica para obter e consumir o evento do botão
    TipoClique obterEvento(ControleBotao &botao) {
        TipoClique ev = botao.eventoPendente;
        botao.eventoPendente = TipoClique::NENHUM; // Consome o evento
        return ev;
    }

public:
    void inicializar() {
        pinMode(PINO_BOTAO_ANTERIOR, INPUT_PULLUP);
        pinMode(PINO_BOTAO_SELECIONAR, INPUT_PULLUP);
        pinMode(PINO_BOTAO_PROXIMO, INPUT_PULLUP);
        pinMode(PINO_BOTAO_VOLTAR, INPUT_PULLUP);

        botaoAnterior = {PINO_BOTAO_ANTERIOR};
        botaoSelecionar = {PINO_BOTAO_SELECIONAR};
        botaoProximo = {PINO_BOTAO_PROXIMO};
        botaoVoltar = {PINO_BOTAO_VOLTAR};

        GerenciadorSom::inicializar();
    }

    // Método obrigatório para atualizar o estado de todos os botões no loop()
    void atualizar() {
        processarBotao(botaoAnterior);
        processarBotao(botaoSelecionar);
        processarBotao(botaoProximo);
        processarBotao(botaoVoltar);
    }

    // --- MÉTODOS DE CONSULTA DE EVENTOS ---

    // Métodos para verificar qualquer tipo de clique de um botão específico
    TipoClique eventoAnterior()   { return obterEvento(botaoAnterior); }
    TipoClique eventoSelecionar() { return obterEvento(botaoSelecionar); }
    TipoClique eventoProximo()    { return obterEvento(botaoProximo); }
    TipoClique eventoVoltar()     { return obterEvento(botaoVoltar); }

    // Atalhos booleanos para compatibilidade com código existente (Clique Simples)
    bool pressionouAnterior()   { return obterEvento(botaoAnterior) == TipoClique::SIMPLES; }
    bool pressionouSelecionar() { return obterEvento(botaoSelecionar) == TipoClique::SIMPLES; }
    bool pressionouProximo()    { return obterEvento(botaoProximo) == TipoClique::SIMPLES; }
    bool pressionouVoltar()     { return obterEvento(botaoVoltar) == TipoClique::SIMPLES; }

    // Atalhos para Clique Longo
    bool pressionouLongoAnterior()   { return obterEvento(botaoAnterior) == TipoClique::LONGO; }
    bool pressionouLongoSelecionar() { return obterEvento(botaoSelecionar) == TipoClique::LONGO; }
    bool pressionouLongoProximo()    { return obterEvento(botaoProximo) == TipoClique::LONGO; }
    bool pressionouLongoVoltar()     { return obterEvento(botaoVoltar) == TipoClique::LONGO; }

    // Atalhos para Duplo Clique
    bool pressionouDuploAnterior()   { return obterEvento(botaoAnterior) == TipoClique::DUPLO; }
    bool pressionouDuploSelecionar() { return obterEvento(botaoSelecionar) == TipoClique::DUPLO; }
    bool pressionouDuploProximo()    { return obterEvento(botaoProximo) == TipoClique::DUPLO; }
    bool pressionouDuploVoltar()     { return obterEvento(botaoVoltar) == TipoClique::DUPLO; }
};

#endif