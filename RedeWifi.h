#ifndef REDE_WIFI_H
#define REDE_WIFI_H

#include <Arduino.h>
#include <WiFi.h>

class GerenciadorWifi {
public:
    static void inicializar() {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
    }

    static int realizarVarredura() {
        return WiFi.scanNetworks();
    }

    static String obterNomeRede(int indice) {
        return WiFi.SSID(indice);
    }

    static int32_t obterSinalRede(int indice) {
        return WiFi.RSSI(indice);
    }
};

#endif