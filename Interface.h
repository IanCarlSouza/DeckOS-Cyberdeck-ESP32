#ifndef INTERFACE_H
#define INTERFACE_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Configuracao.h"
#include "StatusHardware.h"
#include "RedeWifi.h"
#include "PlayerMusica.h"
#include "ConexaoPC.h"

class InterfaceUsuario {
private:
    Adafruit_SSD1306 display;
    int itemMenuSelecionado = 0;
    const int TOTAL_ITENS_MENU = 6;

    const char* nomesApps[6] = {
        "1. Wi-Fi",
        "2. Jogos (GB)",
        "3. Musicas",
        "4. Conectar ao PC",
        "5. Status Hardware",
        "6. Firmware / QR"
    };

public:
    InterfaceUsuario() : display(LARGURA_TELA, ALTURA_TELA, &Wire, PINO_RESET_OLED) {}
    PlayerMusica playerMusica;
    ConexaoPC conexaoPC; 

    Adafruit_SSD1306& obterDisplay() {
        return display;
    }
    bool inicializar() {
        if (!display.begin(SSD1306_SWITCHCAPVCC, ENDERECO_OLED)) {
            return false;
        }
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.display();
        return true;
    }

    // --- TELA DE ABERTURA (SPLASH SCREEN) ---
    void desenharTelaAbertura() {
        display.clearDisplay();
        
        // Moldura externa decorativa
        display.drawRect(2, 2, LARGURA_TELA - 4, ALTURA_TELA - 4, SSD1306_WHITE);
        
        // Título e Subtítulo
        display.setTextSize(2);
        display.setCursor(22, 18);
        display.print("DeckOS");

        display.setTextSize(1);
        display.setCursor(20, 42);
        display.print("Iniciando...");

        display.display();
    }

    // --- TELA DE MENU PRINCIPAL ---
    void desenharMenuPrincipal() {
        display.clearDisplay();
        
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("=== DeckOS MENU ===");
        display.drawLine(0, 10, LARGURA_TELA, 10, SSD1306_WHITE);

        // Exibe 3 itens visíveis com rolagem
        int itemInicio = max(0, itemMenuSelecionado - 1);
        int yOffset = 16;

        for (int i = itemInicio; i < min(itemInicio + 3, TOTAL_ITENS_MENU); i++) {
            if (i == itemMenuSelecionado) {
                display.fillRect(0, yOffset - 1, LARGURA_TELA, 11, SSD1306_WHITE);
                display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            } else {
                display.setTextColor(SSD1306_WHITE);
            }
            display.setCursor(4, yOffset);
            display.print(nomesApps[i]);
            yOffset += 14;
        }

        display.setTextColor(SSD1306_WHITE);
        display.display();
    }

    void navegarMenuProximo() {
        itemMenuSelecionado = (itemMenuSelecionado + 1) % TOTAL_ITENS_MENU;
    }

    void navegarMenuAnterior() {
        itemMenuSelecionado = (itemMenuSelecionado - 1 + TOTAL_ITENS_MENU) % TOTAL_ITENS_MENU;
    }

    int obterAppSelecionado() {
        return itemMenuSelecionado;
    }

    // --- APLICATIVO: WI-FI ---
    void desenharAppWifi(int totalRedes) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(1);
        display.print("--- APP: WI-FI ---");
        display.drawLine(0, 10, LARGURA_TELA, 10, SSD1306_WHITE);

        if (totalRedes < 0) {
            display.setCursor(0, 25);
            display.print("Escaneando redes...");
        } else if (totalRedes == 0) {
            display.setCursor(0, 25);
            display.print("Nenhuma rede achada.");
        } else {
            int maxExibir = min(totalRedes, 3);
            for (int i = 0; i < maxExibir; i++) {
                display.setCursor(0, 16 + (i * 12));
                display.print(GerenciadorWifi::obterNomeRede(i).substring(0, 14));
                display.setCursor(95, 16 + (i * 12));
                display.print(GerenciadorWifi::obterSinalRede(i));
                display.print("dB");
            }
        }
        display.display();
    }

    // --- APLICATIVO: JOGOS  --- (será implementado futuramente)
    void desenharAppJogos() {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("--- APP: JOGOS ---");
        display.drawLine(0, 10, LARGURA_TELA, 10, SSD1306_WHITE);
        
        display.setCursor(0, 22);
        display.print("Emulador GameBoy");
        display.setCursor(0, 36);
        display.print("Aguardando ROMs...");
        display.display();
    }

    // --- APLICATIVO: MÚSICAS ---
    void desenharAppMusicas() {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("--- APP: MUSICAS ---");
        display.drawLine(0, 10, LARGURA_TELA, 10, SSD1306_WHITE);

        display.setCursor(0, 22);
        display.print("Player 8-Bit Chiptune");
        display.setCursor(0, 38);
        display.print("[>] Tocar Melodia");
        display.display();
    }

    // --- APLICATIVO: CONECTAR AO PC ---
    void desenharAppConectarPc() {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("--- CONEXAO PC ---");
        display.drawLine(0, 10, LARGURA_TELA, 10, SSD1306_WHITE);

        // Tela em branco para streaming visual futuro do PC
        display.drawRect(5, 15, 118, 45, SSD1306_WHITE);
        display.setCursor(15, 32);
        display.print("Aguardando PC...");
        display.display();
    }

    // --- APLICATIVO: HARDWARE STATUS ---
    void desenharAppHardware() {
        InformacoesHardware info = MonitorHardware::obterStatus();
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("--- STATUS HARDWARE ---");
        display.drawLine(0, 9, LARGURA_TELA, 9, SSD1306_WHITE);

        display.setCursor(0, 14);
        display.print("Chip: "); display.print(info.modeloChip);
        
        display.setCursor(0, 26);
        display.print("CPU: "); display.print(info.frequenciaCpuMHz); display.print(" MHz");

        display.setCursor(0, 38);
        display.print("RAM Livre: "); display.print(info.memoriaRamLivreBytes / 1024); display.print(" KB");

        display.setCursor(0, 50);
        display.print("Flash: "); display.print(info.tamanhoMemoriaFlashBytes / (1024 * 1024)); display.print(" MB");

        display.display();
    }

    // --- APLICATIVO: VERSÃO DO FIRMWARE & QR CODE ---
    void desenharAppFirmware() {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("--- FIRMWARE ---");
        display.drawLine(0, 10, LARGURA_TELA, 10, SSD1306_WHITE);

        display.setCursor(0, 18);
        display.print("DeckOS");
        display.setCursor(0, 30);
        display.print(VERSAO_FIRMWARE);

        int xQR = 85;
        int yQR = 18;
        display.drawRect(xQR, yQR, 36, 36, SSD1306_WHITE);
        display.fillRect(xQR + 4, yQR + 4, 10, 10, SSD1306_WHITE);
        display.fillRect(xQR + 22, yQR + 4, 10, 10, SSD1306_WHITE);
        display.fillRect(xQR + 4, yQR + 22, 10, 10, SSD1306_WHITE);
        display.fillRect(xQR + 18, yQR + 18, 8, 8, SSD1306_WHITE);

        display.display();
    }
};

#endif