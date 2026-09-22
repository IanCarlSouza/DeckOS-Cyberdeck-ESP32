#ifndef CONEXAO_PC_H
#define CONEXAO_PC_H

#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include <vector>
#include "Configuracao.h"

class ConexaoPC {
private:
    WebServer server;
    Preferences prefs;
    bool apAtivo = false;
    bool telaDesenhoAtiva = false;
    bool interfaceDesenhada = false;
    Adafruit_SSD1306* displayPtr = nullptr;

    String obterListaIDs() {
        prefs.begin("mural_db", true);
        String lista = prefs.getString("ids", "");
        prefs.end();
        return lista;
    }

    // Salva a lista atualizada de IDs
    void salvarListaIDs(const String& lista) {
        prefs.begin("mural_db", false);
        prefs.putString("ids", lista);
        prefs.end();
    }

    // Obtém o próximo ID livre para salvamento
    int obterProximoID() {
        prefs.begin("mural_db", false);
        int nextId = prefs.getInt("next_id", 1);
        prefs.putInt("next_id", nextId + 1);
        prefs.end();
        return nextId;
    }

    // Grava os 1024 bytes de um desenho em um ID específico
    bool gravarDesenhoFlash(int id, uint8_t* buffer) {
        prefs.begin("mural_db", false);
        String chave = "slot_" + String(id);
        size_t n = prefs.putBytes(chave.c_str(), buffer, 1024);
        prefs.end();

        if (n == 1024) {
            String lista = obterListaIDs();
            String idStr = String(id);
            if (lista.length() == 0) {
                salvarListaIDs(idStr);
            } else if (("," + lista + ",").indexOf("," + idStr + ",") == -1) {
                salvarListaIDs(lista + "," + idStr);
            }
            return true;
        }
        return false;
    }

    // Deleta um desenho específico da Flash
    void deletarDesenhoFlash(int id) {
        prefs.begin("mural_db", false);
        String chave = "slot_" + String(id);
        prefs.remove(chave.c_str());
        prefs.end();

        String lista = obterListaIDs();
        String idStr = String(id);
        String novaLista = "";
        
        int start = 0;
        int comma = lista.indexOf(',');
        while (comma != -1) {
            String item = lista.substring(start, comma);
            if (item != idStr && item.length() > 0) {
                if (novaLista.length() > 0) novaLista += ",";
                novaLista += item;
            }
            start = comma + 1;
            comma = lista.indexOf(',', start);
        }
        if (start < lista.length()) {
            String item = lista.substring(start);
            if (item != idStr && item.length() > 0) {
                if (novaLista.length() > 0) novaLista += ",";
                novaLista += item;
            }
        }
        salvarListaIDs(novaLista);
    }

    // HTML / CSS / JS Gerados por IA
    const char* htmlPagina = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>DeckOS Canvas & Mural</title>
    <style>
        * { box-sizing: border-box; }
        /* CSS CORRIGIDO: Permite scroll normal na página */
        body { background: #121212; color: #fff; font-family: system-ui, -apple-system, sans-serif; text-align: center; margin: 0; padding: 12px; }
        h2 { margin: 5px 0 10px 0; font-size: 20px; color: #00ffcc; }
        h3 { margin: 25px 0 10px 0; font-size: 16px; color: #ffb86c; border-bottom: 1px solid #333; padding-bottom: 6px; }
        
        /* Canvas Principal - touch-action restrito apenas aqui */
        canvas.main-canvas { background: #000; border: 2px solid #00ffcc; border-radius: 8px; width: 100%; max-width: 384px; aspect-ratio: 2/1; image-rendering: pixelated; cursor: crosshair; touch-action: none; }
        
        .status-edicao { margin: 8px 0; font-size: 13px; font-weight: bold; color: #ffa500; min-height: 18px; }
        .btn-group { margin-top: 8px; display: flex; justify-content: center; gap: 8px; flex-wrap: wrap; }
        .btn { border: none; padding: 10px 14px; font-size: 14px; border-radius: 6px; cursor: pointer; font-weight: bold; color: white; display: inline-flex; align-items: center; gap: 5px; }
        .btn-clear { background: #555; }
        .btn-save { background: #2ed573; }
        .btn-update { background: #ffa500; }
        .btn:active { opacity: 0.8; }
        
        /* Mural Grid Dinâmico */
        .mural-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(140px, 1fr)); gap: 12px; max-width: 500px; margin: 12px auto; }
        .mural-card { background: #1e1e2e; border: 1px solid #333; border-radius: 8px; padding: 8px; display: flex; flex-direction: column; align-items: center; }
        .mural-card.editando { border: 2px solid #ffa500; background: #2a2415; }
        .mural-card h4 { margin: 0 0 6px 0; font-size: 13px; color: #aaa; }
        .mural-thumb { background: #000; border: 1px solid #555; border-radius: 4px; width: 100%; aspect-ratio: 2/1; image-rendering: pixelated; }
        .mural-actions { margin-top: 8px; display: flex; gap: 4px; flex-wrap: wrap; justify-content: center; width: 100%; }
        .btn-sm { padding: 6px 8px; font-size: 11px; border-radius: 4px; border: none; font-weight: bold; cursor: pointer; color: white; }
        .btn-display { background: #1e90ff; }
        .btn-edit { background: #ffa500; }
        .btn-delete { background: #ff4757; }
    </style>
</head>
<body>
    <h2>DeckOS Canvas</h2>
    <canvas id="cv" class="main-canvas" width="128" height="64"></canvas>
    
    <div id="statusEdicao" class="status-edicao"></div>

    <div class="btn-group">
        <button class="btn btn-clear" onclick="limpar()">Limpar Tela</button>
        <button id="btnSalvar" class="btn btn-save" onclick="salvarAtual()">Salvar Novo</button>
    </div>

    <h3>Mural de Desenhos Salvos</h3>
    <div class="mural-grid" id="muralContainer"></div>

    <script>
        const cv = document.getElementById('cv');
        const ctx = cv.getContext('2d');
        let desenhando = false;
        let slotEmEdicao = null; // Guarda o ID do desenho sendo editado

        // Inicializa Canvas
        ctx.fillStyle = "black";
        ctx.fillRect(0, 0, 128, 64);
        ctx.strokeStyle = "white";
        ctx.lineWidth = 1;
        ctx.lineCap = "round";

        function getPos(e) {
            const rect = cv.getBoundingClientRect();
            const clientX = (e.touches && e.touches.length > 0) ? e.touches[0].clientX : e.clientX;
            const clientY = (e.touches && e.touches.length > 0) ? e.touches[0].clientY : e.clientY;
            return {
                x: Math.floor((clientX - rect.left) / (rect.width / 128)),
                y: Math.floor((clientY - rect.top) / (rect.height / 64))
            };
        }

        function iniciar(e) { if (e.cancelable) e.preventDefault(); desenhando = true; desenhar(e); }
        function parar(e) { if (e && e.cancelable) e.preventDefault(); desenhando = false; ctx.beginPath(); }

        function desenhar(e) {
            if (!desenhando) return;
            if (e.cancelable) e.preventDefault();
            const pos = getPos(e);
            ctx.lineTo(pos.x, pos.y);
            ctx.stroke();
            ctx.beginPath();
            ctx.moveTo(pos.x, pos.y);
            enviarFrame();
        }

        cv.addEventListener('mousedown', iniciar);
        cv.addEventListener('mouseup', parar);
        cv.addEventListener('mousemove', desenhar);

        cv.addEventListener('touchstart', iniciar, { passive: false });
        cv.addEventListener('touchend', parar, { passive: false });
        cv.addEventListener('touchmove', desenhar, { passive: false });

        function atualizarUIEdicao() {
            const statusDiv = document.getElementById('statusEdicao');
            const btnSalvar = document.getElementById('btnSalvar');

            if (slotEmEdicao !== null) {
                statusDiv.innerText = `Editando o Desenho #${slotEmEdicao}`;
                btnSalvar.className = 'btn btn-update';
                btnSalvar.innerText = `Atualizar #${slotEmEdicao}`;
            } else {
                statusDiv.innerText = '';
                btnSalvar.className = 'btn btn-save';
                btnSalvar.innerText = 'Salvar Novo';
            }
        }

        function limpar() {
            ctx.fillStyle = "black";
            ctx.fillRect(0, 0, 128, 64);
            ctx.beginPath();
            slotEmEdicao = null;
            atualizarUIEdicao();
            enviarFrame();
            carregarMural();
        }

        let enviando = false;
        function obterHexBuffer() {
            const imgData = ctx.getImageData(0, 0, 128, 64).data;
            const buffer = new Uint8Array(1024);

            for (let y = 0; y < 64; y++) {
                for (let x = 0; x < 128; x++) {
                    const idx = (y * 128 + x) * 4;
                    if (imgData[idx] > 128) {
                        buffer[x + Math.floor(y / 8) * 128] |= (1 << (y % 8));
                    }
                }
            }

            let hex = "";
            for (let i = 0; i < 1024; i++) {
                hex += buffer[i].toString(16).padStart(2, '0');
            }
            return hex;
        }

        function enviarFrame() {
            if (enviando) return;
            enviando = true;
            const hex = obterHexBuffer();

            fetch('/frame', { 
                method: 'POST', 
                headers: { 'Content-Type': 'text/plain' },
                body: hex 
            })
            .then(() => { enviando = false; })
            .catch(() => { enviando = false; });
        }

        function renderHexParaCanvas(hex, targetCtx) {
            const imgData = targetCtx.createImageData(128, 64);
            const data = imgData.data;

            for (let i = 0; i < data.length; i += 4) {
                data[i] = 0; data[i+1] = 0; data[i+2] = 0; data[i+3] = 255;
            }

            if (hex && hex.length >= 2048) {
                for (let i = 0; i < 1024; i++) {
                    const byteVal = parseInt(hex.substr(i * 2, 2), 16) || 0;
                    const x = i % 128;
                    const page = Math.floor(i / 128);

                    for (let bit = 0; bit < 8; bit++) {
                        if (byteVal & (1 << bit)) {
                            const y = page * 8 + bit;
                            const pxIdx = (y * 128 + x) * 4;
                            data[pxIdx] = 0;
                            data[pxIdx + 1] = 255;
                            data[pxIdx + 2] = 204;
                            data[pxIdx + 3] = 255;
                        }
                    }
                }
            }
            targetCtx.putImageData(imgData, 0, 0);
        }

        function carregarMural() {
            fetch('/gallery')
            .then(res => res.json())
            .then(dados => {
                const container = document.getElementById('muralContainer');
                container.innerHTML = '';

                const ids = Object.keys(dados);
                if (ids.length === 0) {
                    container.innerHTML = '<p style="color:#888; grid-column:1/-1;">Nenhum desenho salvo ainda.</p>';
                    return;
                }

                ids.forEach(id => {
                    const hexData = dados[id];
                    const card = document.createElement('div');
                    const estaEditando = (slotEmEdicao == id);
                    card.className = `mural-card ${estaEditando ? 'editando' : ''}`;
                    
                    card.innerHTML = `
                        <h4>Desenho #${id}</h4>
                        <canvas id="thumb_${id}" class="mural-thumb" width="128" height="64"></canvas>
                        <div class="mural-actions">
                            <button class="btn-sm btn-display" onclick="exibirNoOled(${id})">Exibir</button>
                            <button class="btn-sm btn-edit" onclick="editarNoCanvas(${id}, '${hexData}')">Editar</button>
                            <button class="btn-sm btn-delete" onclick="deletarSlot(${id})"></button>
                        </div>
                    `;
                    container.appendChild(card);

                    const thumbCv = document.getElementById(`thumb_${id}`);
                    renderHexParaCanvas(hexData, thumbCv.getContext('2d'));
                });
            });
        }

        // Salva novo desenho ou atualiza o desenho existente sendo editado
        function salvarAtual() {
            const hex = obterHexBuffer();
            const url = (slotEmEdicao !== null) ? `/save_slot?slot=${slotEmEdicao}` : '/save_slot';

            fetch(url, { method: 'POST', body: hex })
            .then(res => {
                if (res.ok) {
                    carregarMural();
                    if (slotEmEdicao !== null) {
                        alert(`Desenho #${slotEmEdicao} atualizado com sucesso!`);
                    }
                } else {
                    alert('Erro ao salvar desenho.');
                }
            });
        }

        function editarNoCanvas(id, hex) {
            slotEmEdicao = id;
            atualizarUIEdicao();
            renderHexParaCanvas(hex, ctx);
            enviarFrame(); // Transmite imediatamente para a tela OLED
            carregarMural();
            window.scrollTo({ top: 0, behavior: 'smooth' }); // Sobe a página para o Canvas
        }

        function deletarSlot(id) {
            if (!confirm(`Deseja apagar o Desenho #${id}?`)) return;
            fetch(`/delete_slot?slot=${id}`, { method: 'POST' })
            .then(() => {
                if (slotEmEdicao == id) {
                    slotEmEdicao = null;
                    atualizarUIEdicao();
                }
                carregarMural();
            });
        }

        function exibirNoOled(id) {
            fetch(`/load_oled?slot=${id}`, { method: 'POST' });
        }

        carregarMural();
    </script>
</body>
</html>
    )rawliteral";

public:
    ConexaoPC() : server(80) {}

    void iniciarAP(Adafruit_SSD1306 &display) {
        if (apAtivo) return;

        displayPtr = &display;
        telaDesenhoAtiva = false;
        interfaceDesenhada = false;

        WiFi.mode(WIFI_AP);
        WiFi.softAP("DeckOS-WiFi", "");
        server.on("/", [this]() {
            server.send(200, "text/html", htmlPagina);
        });

        // Guarda o desenho em tempo real
        server.on("/frame", HTTP_POST, [this]() {
            if (server.hasArg("plain") && displayPtr != nullptr) {
                String hex = server.arg("plain");
                if (hex.length() >= 2048) {
                    uint8_t* displayBuffer = displayPtr->getBuffer();
                    for (int i = 0; i < 1024; i++) {
                        char h1 = hex[i * 2];
                        char h2 = hex[i * 2 + 1];
                        uint8_t v1 = (h1 >= 'a') ? (h1 - 'a' + 10) : ((h1 >= 'A') ? (h1 - 'A' + 10) : (h1 - '0'));
                        uint8_t v2 = (h2 >= 'a') ? (h2 - 'a' + 10) : ((h2 >= 'A') ? (h2 - 'A' + 10) : (h2 - '0'));
                        displayBuffer[i] = (v1 << 4) | v2;
                    }
                    displayPtr->display();
                    telaDesenhoAtiva = true;
                }
            }
            server.send(200, "text/plain", "OK");
        });

        // Eetorna o JSON de todos os desenhos salvos
        server.on("/gallery", HTTP_GET, [this]() {
            String lista = obterListaIDs();
            prefs.begin("mural_db", true);

            String json = "{";
            if (lista.length() > 0) {
                int start = 0;
                int comma = lista.indexOf(',');
                bool primeiro = true;

                while (comma != -1) {
                    String idStr = lista.substring(start, comma);
                    String chave = "slot_" + idStr;
                    if (prefs.isKey(chave.c_str())) {
                        if (!primeiro) json += ",";
                        json += "\"" + idStr + "\":\"";
                        uint8_t temp[1024];
                        prefs.getBytes(chave.c_str(), temp, 1024);
                        for (int b = 0; b < 1024; b++) {
                            if (temp[b] < 16) json += "0";
                            json += String(temp[b], HEX);
                        }
                        json += "\"";
                        primeiro = false;
                    }
                    start = comma + 1;
                    comma = lista.indexOf(',', start);
                }
                if (start < lista.length()) {
                    String idStr = lista.substring(start);
                    String chave = "slot_" + idStr;
                    if (prefs.isKey(chave.c_str())) {
                        if (!primeiro) json += ",";
                        json += "\"" + idStr + "\":\"";
                        uint8_t temp[1024];
                        prefs.getBytes(chave.c_str(), temp, 1024);
                        for (int b = 0; b < 1024; b++) {
                            if (temp[b] < 16) json += "0";
                            json += String(temp[b], HEX);
                        }
                        json += "\"";
                    }
                }
            }
            json += "}";
            prefs.end();
            server.send(200, "application/json", json);
        });

        // Rota para salvar (Novo ou Atualizar existente)
        server.on("/save_slot", HTTP_POST, [this]() {
            if (server.hasArg("plain")) {
                int id = server.hasArg("slot") ? server.arg("slot").toInt() : obterProximoID();
                String hex = server.arg("plain");

                if (hex.length() >= 2048) {
                    uint8_t tempBuffer[1024];
                    for (int i = 0; i < 1024; i++) {
                        char h1 = hex[i * 2];
                        char h2 = hex[i * 2 + 1];
                        uint8_t v1 = (h1 >= 'a') ? (h1 - 'a' + 10) : ((h1 >= 'A') ? (h1 - 'A' + 10) : (h1 - '0'));
                        uint8_t v2 = (h2 >= 'a') ? (h2 - 'a' + 10) : ((h2 >= 'A') ? (h2 - 'A' + 10) : (h2 - '0'));
                        tempBuffer[i] = (v1 << 4) | v2;
                    }
                    if (gravarDesenhoFlash(id, tempBuffer)) {
                        server.send(200, "text/plain", "OK");
                        return;
                    }
                }
            }
            server.send(400, "text/plain", "Erro");
        });

        // Rota para deletar um desenho da Flash
        server.on("/delete_slot", HTTP_POST, [this]() {
            if (server.hasArg("slot")) {
                int id = server.arg("slot").toInt();
                deletarDesenhoFlash(id);
                server.send(200, "text/plain", "OK");
            } else {
                server.send(400, "text/plain", "Erro");
            }
        });

        // Rota para carregar o desenho direto na tela OLED
        server.on("/load_oled", HTTP_POST, [this]() {
            if (server.hasArg("slot") && displayPtr != nullptr) {
                int id = server.arg("slot").toInt();
                prefs.begin("mural_db", true);
                String chave = "slot_" + String(id);
                if (prefs.isKey(chave.c_str())) {
                    prefs.getBytes(chave.c_str(), displayPtr->getBuffer(), 1024);
                    displayPtr->display();
                    telaDesenhoAtiva = true;
                }
                prefs.end();
                server.send(200, "text/plain", "OK");
            } else {
                server.send(400, "text/plain", "Erro");
            }
        });

        server.begin();
        apAtivo = true;
    }

    void pararAP() {
        if (!apAtivo) return;
        server.stop();
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_OFF);
        apAtivo = false;
        telaDesenhoAtiva = false;
        interfaceDesenhada = false;
    }

    void atualizar() {
        if (!apAtivo) return;
        server.handleClient();
    }

    void desenharInterface(Adafruit_SSD1306 &display) {
        if (telaDesenhoAtiva || interfaceDesenhada) return;

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);

        display.setCursor(12, 0);
        display.print(F("=== CONEXAO PC ==="));

        display.setCursor(0, 16);
        display.print(F("1. Conecte no Wi-Fi:"));
        display.setCursor(0, 26);
        display.print(F("   DeckOS-WiFi"));

        display.setCursor(0, 40);
        display.print(F("2. Acesse no navegador:"));
        display.setCursor(0, 50);
        display.print(F("   192.168.4.1"));

        display.display();
        interfaceDesenhada = true;
    }
};

#endif