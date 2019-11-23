  //Apenas irá compilar se MASTER estiver definido
 #ifdef MASTER

//Mac Address do peer para o qual iremos enviar os dados
uint8_t peerMacAddress[] = {0x24, 0x0A, 0xC4, 0x0E, 0x3F, 0xD0};

void setup() {
  Serial.begin(115200);

  //Chama a função que inicializa o modo station
  modeStation();

  //Chama a função que inicializa o ESPNow
  InitESPNow();

  //Adiciona o peer
  addPeer(peerMacAddress);

  //Registra o callback que nos informará 
  //sobre o status do envio.
  //A função que será executada é onDataSent
  //e está declarada mais abaixo
  esp_now_register_send_cb(OnDataSent);
  
  //Colocamos o pino em modo de leitura
  pinMode(PIN, INPUT);

  //Lê o valor do pino e envia
  readAndSend();
}

//Função responsável pela
//leitura do pino e envio
//do valor para o peer
void readAndSend(){
  //Lê o valor do pino
  uint8_t value = digitalRead(PIN);
  //Envia o valor para o peer
  send(&value, peerMacAddress);
}

//Função que serve de callback para nos avisar
//sobre a situação do envio que fizemos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");

  //Quando recebemos o resultado do último envio
  //já podemos ler e enviar novamente
  readAndSend();
}

//Não precisamos fazer nada no loop
//pois sempre que recebemos o feedback
//do envio através da função OnDataSent
//nós enviamos os dados novamente,
//fazendo com que os dados estejam sempre
//sendo enviados em sequência
void loop() {
}
#endif