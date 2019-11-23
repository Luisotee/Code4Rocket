//Apenas irá compilar se MASTER não estiver definido
#ifndef MASTER

//Mac Address do peer para o qual iremos enviar os dados
uint8_t peerMacAddress[6] = {0x24, 0x0A, 0xC4, 0x0E, 0x46, 0xCC};

void setup() {
  Serial.begin(115200);

  //Chama a função que inicializa o modo station
  modeStation();

  //Chama a função que inicializa o ESPNow
  InitESPNow();

  //Adiciona o peer
  addPeer(peerMacAddress);

  //Registra o callback que nos informará
  //que recebemos dados.
  //A função que será executada
  //é onDataRecv e está declarada mais abaixo
  esp_now_register_recv_cb(onDataRecv);

  //Registra o callback que nos informará 
  //sobre o status do envio.
  //A função que será executada
  //é onDataSent e está declarada mais abaixo
  esp_now_register_send_cb(onDataSent);

  //Colocamos o pino como saída
  pinMode(PIN, OUTPUT);
}

//Função que serve de callback para nos avisar
//que recebemos dados
void onDataRecv(const uint8_t *mac_addr, const uint8_t *value, int len) {
  //Coloca o valor recebido na saída do pino
  digitalWrite(PIN, *value);
  //Envia o valor lido para o próximo esp
  //Se este for o último, comente esta linha antes de compilar
  send(value, peerMacAddress);
}

//Função que serve de callback para nos avisar
//sobre a situação do envio que fizemos
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

//Não precisamos fazer nada no loop
//nós enviamos os dados assim que 
//recebemos do outro esp pelo callback
void loop() {
}
#endif