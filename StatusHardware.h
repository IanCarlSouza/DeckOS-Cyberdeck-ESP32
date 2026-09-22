#ifndef STATUS_HARDWARE_H
#define STATUS_HARDWARE_H

#include <Arduino.h>
#include <Esp.h>

struct InformacoesHardware {
    uint32_t memoriaRamLivreBytes;
    uint32_t tamanhoMemoriaFlashBytes;
    uint32_t frequenciaCpuMHz;
    String modeloChip;
};

class MonitorHardware {
public:
    static InformacoesHardware obterStatus() {
        InformacoesHardware info;
        info.memoriaRamLivreBytes = ESP.getFreeHeap();
        info.tamanhoMemoriaFlashBytes = ESP.getFlashChipSize();
        info.frequenciaCpuMHz = ESP.getCpuFreqMHz();
        info.modeloChip = "ESP32-D0WD-V3";
        return info;
    }
};

#endif