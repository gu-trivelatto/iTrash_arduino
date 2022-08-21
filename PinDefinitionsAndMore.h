#include "secrets.h"

#define SOUND_VELOCITY 0.034
#define ABERTO 135
#define FECHANDO 90
#define FECHADO 45

const int trigPin = 14;          // Trigger - Sonar
const int echoPin = 4;           // Echo - Sonar
const int greenLed = 2;          // Led Verde
const int yellowLed = 0;         // Led Amarelo
const int redLed = 13;           // Led Vermelho
const int whiteLed = 5;          // Led Branco
const int yellowButton = 15;     // Botão do fio amarelo
const int orangeButton = 12;     // Botão do fio laranja
const int servo = 16;            // Sinal de controle do Servomotor

const long sonarInterval = 1000;      // Intervalo de medição do sonar
const long lidWarningInterval = 15000; // Intervalo para acender a luz de alerta da tampa
const long lidClosingInterval = 8000; // Intervalo para fechar a tampa após acender a luz de alerta

unsigned long myChannel = SECRET_CH_ID;
const char * myWriteAPI = SECRET_WRITE_APIKEY;
const char * myReadAPI = SECRET_READ_APIKEY;

long duration;                     // Duração do sinal Echo do Sonar
float distanceCm = 0;              // Distância medida em cm pelo sonar
float capacity = 0;                 // Porcentagem utilizada da lixeira
unsigned long previousMillisSonar = 0;  // Contagem de tempo anterior do Sonar
unsigned long previousMillisTampa = 0;  // Contagem de tempo anterior da abertura da tampa

const int binSize = 7;           // Altura da lixeira em centímetros

int ybState = 0;        // Estado atual do botão amarelo
int lastYbState = 0;    // Último estado do botão amarelo
int obState = 0;        // Estado atual do botão laranja
int lastObState = 0;    // Último estado do botão laranja
int mqttBState = 0;     // Estado atual do botão do MQTT
int lastMqttBState = 0; // Último estado do botão do MQTT
int pos = FECHADO;      // Posição da tampa
bool buttonAtiv = false;// Var de ativação do botão
