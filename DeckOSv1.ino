#include "Configuracao.h"
#include "Botoes.h"
#include "Interface.h"
#include "RedeWifi.h"

GerenciadorBotoes botoes;
InterfaceUsuario interfaceUI;

EstadoSistema estadoAtual = ESTADO_TELA_ABERTURA;
unsigned long tempoInicioAbertura = 0;
const unsigned long DURACAO_ABERTURA_MS = 3000; 

void setup() {
    Serial.begin(115200);

    pinMode(PINO_LED_VERMELHO, OUTPUT);
    pinMode(PINO_LED_VERDE, OUTPUT);
    pinMode(PINO_LED_AZUL, OUTPUT);
    pinMode(PINO_BUZZER, OUTPUT);

    botoes.inicializar();
    GerenciadorWifi::inicializar();

    if (!interfaceUI.inicializar()) {
        Serial.println(F("Falha ao inicializar o Display OLED!"));
        for (;;); 
    }

    tempoInicioAbertura = millis();
    interfaceUI.desenharTelaAbertura();
}

void loop() {
    botoes.atualizar();
    // --- MÁQUINA DE ESTADOS DO DECKOS ---
    switch (estadoAtual) {
        
        case ESTADO_TELA_ABERTURA:
            if ((millis() - tempoInicioAbertura >= DURACAO_ABERTURA_MS) || botoes.pressionouSelecionar()) {
                estadoAtual = ESTADO_MENU_PRINCIPAL;
                interfaceUI.desenharMenuPrincipal();
            }
            break;

        case ESTADO_MENU_PRINCIPAL:
            if (botoes.pressionouProximo()) {
                interfaceUI.navegarMenuProximo();
                interfaceUI.desenharMenuPrincipal();
            } 
            else if (botoes.pressionouAnterior()) {
                interfaceUI.navegarMenuAnterior();
                interfaceUI.desenharMenuPrincipal();
            } 
            else if (botoes.pressionouSelecionar()) {
                int appSelecionado = interfaceUI.obterAppSelecionado();

                switch (appSelecionado) {
                    case 0: 
                        estadoAtual = ESTADO_APP_WIFI; 
                        interfaceUI.desenharAppWifi(-1); 
                        break;
                    case 1: 
                        estadoAtual = ESTADO_APP_JOGOS; 
                        interfaceUI.desenharAppJogos();
                        break;
                    case 2: 
                        estadoAtual = ESTADO_APP_MUSICAS; 
                        interfaceUI.desenharAppMusicas();
                        break;
                    case 3: 
                        estadoAtual = ESTADO_APP_CONECTAR_PC; 
                        interfaceUI.conexaoPC.iniciarAP(interfaceUI.obterDisplay());
                        break;
                    case 4: 
                        estadoAtual = ESTADO_APP_HARDWARE; 
                        interfaceUI.desenharAppHardware();
                        break;
                    case 5: 
                        estadoAtual = ESTADO_APP_FIRMWARE; 
                        interfaceUI.desenharAppFirmware();
                        break;
                }
            }
            break;

        case ESTADO_APP_WIFI:
            static bool varreduraFeita = false;
            if (!varreduraFeita) {
                int totalRedes = GerenciadorWifi::realizarVarredura();
                interfaceUI.desenharAppWifi(totalRedes);
                varreduraFeita = true;
            }

            if (botoes.pressionouVoltar()) {
                varreduraFeita = false;
                estadoAtual = ESTADO_MENU_PRINCIPAL;
                interfaceUI.desenharMenuPrincipal();
            }
            break;

        case ESTADO_APP_JOGOS:
        case ESTADO_APP_MUSICAS:
            interfaceUI.playerMusica.atualizar();
            interfaceUI.playerMusica.desenharInterface(interfaceUI.obterDisplay());

            if (botoes.pressionouAnterior()) {
                interfaceUI.playerMusica.faixaAnterior();
            }

            if (botoes.pressionouSelecionar()) {
                interfaceUI.playerMusica.alternarPlayPause();
            }

            if (botoes.pressionouProximo()) {
                interfaceUI.playerMusica.proximaFaixa();
            }

            if (botoes.pressionouVoltar()) {
                interfaceUI.playerMusica.parar(); 
                estadoAtual = ESTADO_MENU_PRINCIPAL;
                interfaceUI.desenharMenuPrincipal();
            }
            break;

        case ESTADO_APP_CONECTAR_PC:
            interfaceUI.conexaoPC.atualizar();
            interfaceUI.conexaoPC.desenharInterface(interfaceUI.obterDisplay());

            if (botoes.pressionouVoltar()) {
                interfaceUI.conexaoPC.pararAP();
                estadoAtual = ESTADO_MENU_PRINCIPAL;
                interfaceUI.desenharMenuPrincipal();
            }
            break;

        case ESTADO_APP_HARDWARE:
        case ESTADO_APP_FIRMWARE:
            if (botoes.pressionouVoltar()) {
                estadoAtual = ESTADO_MENU_PRINCIPAL;
                interfaceUI.desenharMenuPrincipal();
            }
            break;
    }
}