//Libs do espnow e wifi
#include <esp_now.h>
#include <WiFi.h>

//Pino utilizado para leitura
//e que será enviado o valor
#define PIN 2

//Canal usado para conexão
#define CHANNEL 1

//Se MASTER estiver definido 
//o compilador irá compilar o Master.ino
//Se quiser compilar o Slave.ino remova ou
//comente a linha abaixo
#define MASTER

//Estrutura com informações
//sobre o próximo peer
esp_now_peer_info_t peer;

//Função para inicializar o modo station
void modeStation(){
    //Colocamos o ESP em modo station
    WiFi.mode(WIFI_STA);
    //Mostramos no Monitor Serial o Mac Address 
    //deste ESP quando em modo station
    Serial.print("Mac Address in Station: "); 
    Serial.println(WiFi.macAddress());
}

//Função de inicialização do ESPNow
void InitESPNow() {
    //Se a inicialização foi bem sucedida
    if (esp_now_init() == ESP_OK) {
        Serial.println("ESPNow Init Success");
    }
    //Se houve erro na inicialização
    else {
        Serial.println("ESPNow Init Failed");
        ESP.restart();
    }
}

//Função que adiciona um novo peer
//através de seu endereço MAC
void addPeer(uint8_t *peerMacAddress){
    //Informamos o canal
    peer.channel = CHANNEL;
    //0 para não usar criptografia ou 1 para usar
    peer.encrypt = 0;
    //Copia o endereço do array para a estrutura
    memcpy(peer.peer_addr, peerMacAddress, 6);
    //Adiciona o slave
    esp_now_add_peer(&peer);
}

//Função que irá enviar o valor para 
//o peer que tenha o mac address especificado
void send(const uint8_t *value, uint8_t *peerMacAddress){
    esp_err_t result = esp_now_send(peerMacAddress, value, sizeof(value));
    Serial.print("Send Status: ");
    //Se o envio foi bem sucedido
    if (result == ESP_OK) {
        Serial.println("Success");
    }
    //Se aconteceu algum erro no envio
    else {
        Serial.println("Error");
    }
}