#include <MPU6050_tockn.h>   //Biblioteca MPU
#include <Adafruit_BMP085.h>  //Biblioteca BMP 180
#include <Wire.h> //BIBLIOTECA PARA CONEXAO I2C
#include <SPI.h>        // BIBLIOTECA PARA CONEXAO SPI
#include <mySD.h>  //BIBLIOTECA CARTAO SD
#include <WiFi.h>
#include <Servo.h>

//===================== OBJETOS =====================
File SDfile;
Adafruit_BMP085 BMP180;  //NOME DO BMP180 PARA O CODIGO
MPU6050 MPU6050(Wire);  //NOME DO MPU PARA O CODIGO
Servo MyServo;
//===================== PINOS =====================
#define BUTTON_PIN1 2  //VERIFICAR COM A RECUPERAÇÃO
#define BUTTON_PIN2 4  
#define MOTOR_PIN 12
//===================== CONSTANTE =====================
const int GROUND = 0;
const int FLYING = 1;
const int DROPPING = 2;
const int LANDED = 3;
//===================== VARIAVEIS =====================
double currentAltitude = 0, initialAltitude = 0, apogee = 0;
double xAccel, yAccel, zAccel, peakAccel = 0, avgAccel = 0;
int flightStage = GROUND;

void setup() 
{
  Serial.begin(115200);
  Wire.begin();  //COMECO DA CONEXAO I2C
  InitializeAltimeter();
  for (int i = 0; i < 10; i++)
  {
    initialAltitude += BMP180.readAltitude(102000);
    initialAltitude /= 10;
  }
  Serial.println("Initial altitude = " + String(initialAltitude));
  InitializeSDCard();
  SDfile = SD.open("Data.txt", FILE_WRITE);
  SDfile.print("Altura Inicial = ");
  SDfile.println(String(initialAltitude));  //PRINTA OS DADOS NO CARTAO SD
  pinMode(BUTTON_PIN1, INPUT);
  pinMode(BUTTON_PIN2, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  MyServo.attach(2);
  MyServo.write(90);
  delay(15);
  xTaskCreatePinnedToCore(startCore0, "startCore0", 10000, NULL, 1, NULL, 0);   //INICIA O CORE 0
}

void loop() 
{
  PrintInSerial();  
  Serial.println("Flight Stage = " + String(flightStage));
  switch(flightStage)
  {
    case GROUND:  //Enquanto no chão
    if (currentAltitude - initialAltitude >= 10)
    {
      flightStage = FLYING;
      break;
    } //Checa se começou a voar
    case FLYING:
    if (AmIFalling())
    {
      flightStage = DROPPING;
    } //Checa se está caindo
//    averageAccel();
//    highestAccel();
    break;
    
    case DROPPING:
    Recovery();
    break;

    case LANDED:
    APMode();
    break;
  }
}
void startCore0(void *z)
{
while(1)
{
  PrintInSD();
  switch(flightStage)
  {
    case GROUND: 
    GetDataAltimeter();
    break;

    case FLYING:
    GetDataAltimeter();
    GetDataGyroscope();
    break;

    case DROPPING:
    GetDataAltimeter();
    GetDataGyroscope();
    break;

    case LANDED:
    break;
  }
}
}

//===================== SENSORES =====================
void InitializeSDCard()
{ 
  if (!SD.begin(5, 23, 19, 18))  //ATIVA O CARTAO SD NOS PINOS 5, 23, 19, 18
  {  
    Serial.println("SD card nao esta respondendo!!");  
  }
  else
  {
    Serial.println("SD card OK"); 
  }
  SDfile = SD.open("Data.txt", FILE_WRITE);  //Abre aquivo Data.txt no SD
  if (SDfile) 
  { 
    SDfile.println("Gravando no SD...");  
  }
  else 
  {
    Serial.println("Erro ao abrir o arquivo!!");  
  }
} //Inicializa o SD
void InitializeGyroscope()
{
  MPU6050.begin();  
  MPU6050.calcGyroOffsets(true); 
} //Inicia o MPU
void InitializeAltimeter()
{
  if (!BMP180.begin())
  {
    Serial.println("Erro ao iniciar o altimetro!!"); 
  }
  else
  {
    Serial.println("BMP180 OK");  
  }
} //Inicia o BMP
void GetDataGyroscope()
{
  MPU6050.update();
  xAccel = MPU6050.getAccX();  
  yAccel = MPU6050.getAccY();  
  zAccel = MPU6050.getAccZ();  
} //Obtem dados do MPU
void GetDataAltimeter()
{
  currentAltitude = BMP180.readAltitude(102000) - initialAltitude;
} //Obtem dados do BMP
//===================== ATUADORES =====================
int Recovery() //FUNÇÕES DA RECUPERAÇÃO
{
  bool firstPart = false;
  bool secondPart = false;
  if (!firstPart)
  {
    firstPart = true;
    for (int pos = 90; pos <= 180; pos++)
    {
      MyServo.write(pos);
      delay(15); 
    }
    if (SDfile = SD.open("Data.txt", FILE_WRITE)) //ABRE O ARQUIVO "Data" NO CARTAO SD 
    {
      SDfile.print("\n\n Primeira parte do recovery acionado\n\n");
      SDfile.close();
    }
    delay(10000);
  } //Primeira parte do recovery
  if (apogee <= 500)
  {
    if (!secondPart)
    {
      secondPart = true;
      do
      {
        digitalWrite(MOTOR_PIN, HIGH);
      }while(digitalRead(BUTTON_PIN2) == LOW);
      digitalWrite(MOTOR_PIN, LOW);
    }
    if (SDfile = SD.open("Data.txt", FILE_WRITE)) //ABRE O ARQUIVO "Data" NO CARTAO SD 
    {
      SDfile.print("\n\n Segunda parte do recovery acionado\n\n");
      SDfile.close();
    }
    return LANDED;
  } //Acionar segunda parte do recovery se Apogeu foi menor que 500
  else
  {
    if (currentAltitude <= 500)
    {
      if (!secondPart)
      {
        secondPart = true;
        do
        {   
          digitalWrite(MOTOR_PIN, HIGH);
        }while(digitalRead(BUTTON_PIN2) == LOW);
        digitalWrite(MOTOR_PIN, LOW);
      }
      if (SDfile = SD.open("Data.txt", FILE_WRITE)) //ABRE O ARQUIVO "Data" NO CARTAO SD 
      {
        SDfile.print("\n\n Segunda parte do recovery acionado\n\n");
        SDfile.close();
      }
      return LANDED;
    }
  }
  return DROPPING;
} //Acionar segunda parte do recovery se Apogeu foi maior que 500

//===================== LOGICA =====================
void GetApogee()  
{
  if (currentAltitude > apogee)
  {
    apogee = currentAltitude;
  }
} //PEGA VALOR DO APOGEU
bool AmIFalling() 
{
  GetApogee();
  if (apogee - currentAltitude >= 5)
  {
    return true;
  }
  return false;
} //VERIFICAR SE ESTA CAINDO
void highestAccel()
{
  if (xAccel > peakAccel)
  {
    peakAccel = xAccel;
  }
}
void averageAccel()
{
  if (avgAccel == 0)
  {
    avgAccel = xAccel;
  }
  avgAccel = (avgAccel + xAccel) /2;
}
//===================== WIFI =====================
void APMode() //ROTEADOR
{
  const char* ssid     = "ESP32 Facens Rockets";
  const char* password = "12345678";

  WiFi.softAP(ssid, password);
}

//===================== INTERFACE =====================
unsigned long Time()  //CRONOMETRO
{
  unsigned long now = millis() / 1000;
  return now;
}
void PrintInSerial()  //PRINTAR
{
  Serial.print("Tempo decorrido: ");
  Serial.println(Time());    
  Serial.print("Altitude: ");
  Serial.println(currentAltitude);
  Serial.println("Aceleração maxima = " + String(peakAccel));
  Serial.println("Aceleração media = " + String(avgAccel));
}
void PrintInSD()  //PRINTA NO SD
{
  bool startDropping = false;
  SDfile = SD.open("Data.txt", FILE_WRITE);   //ABRE O ARQUIVO "Data" NO CARTAO SD 
  SDfile.print("Tempo decorrido: ");
  SDfile.println(String(Time()));  //PRINTA OS DADOS NO CARTAO SD
  if (flightStage == DROPPING && !startDropping)
  {
    startDropping = true;
    SDfile.print("\n\nCAINDO\n\n");
  }
  else if (flightStage == DROPPING)
  {
    SDfile.print("Apogeu : " + String(apogee));
  }
  SDfile.print("Altitude: ");           
  SDfile.println(String(currentAltitude)); 
  SDfile.println("Aceleração maxima = " + String(peakAccel));
  SDfile.println("Aceleração media = " + String(avgAccel));
  SDfile.close();  
}
