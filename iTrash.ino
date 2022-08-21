#include <ESP8266WiFi.h>
#include <stdio.h>
#include "PinDefinitionsAndMore.h"
#include "ServoEasing.hpp"
#include "ThingSpeak.h"

ServoEasing myservo;

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(whiteLed, OUTPUT);
  pinMode(yellowButton, INPUT);
  pinMode(orangeButton, INPUT);
  pinMode(servo, OUTPUT);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  myservo.attach(servo, pos);
}

void loop(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  unsigned long currentMillis = millis();

  /* Tratamento do controle da tampa *******************/

  //Definição do estado de ativação do botão  
  obState = digitalRead(orangeButton);
  mqttBState = ThingSpeak.readLongField(myChannel, 2, myReadAPI);
  
  if (mqttBState != lastMqttBState){
    if (mqttBState == HIGH){
      buttonAtiv = true;
      previousMillisTampa = currentMillis;
    }
    lastMqttBState = mqttBState;
  }

  if (obState != lastObState){
    if (obState == HIGH){
      buttonAtiv = true;
      previousMillisTampa = currentMillis;
    }
    lastObState = obState;
  }

  //Se a tampa estiver fechada e o botão ativo, muda o estado e abre
  if (pos == FECHADO && buttonAtiv){
    buttonAtiv = false;
    pos = ABERTO;
    myservo.easeTo(pos, 120);       // myservo.easeTo(degree, speed)
    Serial.println(pos);
  }

  //Se a tampa estiver aberta e o botão ativo, muda o estado e fecha
  if (pos == ABERTO && buttonAtiv){
    buttonAtiv = false;
    pos = FECHADO;
    myservo.easeTo(pos, 120);
    Serial.println(pos);
  }

  //Se o estado estiver em fechando e o botão ativo, reseta a contagem do tempo, volta o estado para aberto e apaga o led
  if (pos == FECHANDO && buttonAtiv){
    buttonAtiv = false;
    pos = ABERTO;
    digitalWrite(whiteLed, LOW);
  }

  //Se estiver aberto e o timer acabar muda para o estado fechando e liga o led
  if (pos == ABERTO && (currentMillis - previousMillisTampa >= lidWarningInterval)) {
    previousMillisTampa = currentMillis;
    pos = FECHANDO;
    digitalWrite(whiteLed, HIGH);
  }

  //Se estiver no estado fechando e acabar o timer, muda o estado, fecha e apaga o led
  if (pos == FECHANDO && (currentMillis - previousMillisTampa >=lidClosingInterval)) {
    previousMillisTampa = currentMillis;
    pos = FECHADO;
    digitalWrite(whiteLed, LOW);
    myservo.easeTo(pos, 120);
    Serial.println(pos);
  }

  /*****************************************************/
  /* Tratamento dos leds de indicação de capacidade ****/

  if (pos == ABERTO || pos == FECHANDO) {
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(redLed, LOW);
  }
  else if (capacity < 30) {
    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(redLed, LOW);
  }
  else if (capacity > 70) {
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(redLed, HIGH);
  }
  else{
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(redLed, LOW);
  }

  /*****************************************************/

  /* Tratamento do Sonar *******************************/
  
  if (pos == FECHADO && (currentMillis - previousMillisSonar >= sonarInterval)){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    
    // Calculate the distance
    distanceCm = duration * SOUND_VELOCITY/2;

    capacity = (1 - (distanceCm/binSize))*100;
    if (capacity > 85){
      capacity = 100;
    } else if (capacity < 0){
      capacity = 0;
    }

    String capString = String(capacity, 0);
    
    // Prints the distance on the Serial Monitor
    Serial.print("Capacity (%): ");
    Serial.println(capString);
  
    int x = ThingSpeak.writeField(myChannel, 1, capString, myWriteAPI);
    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    
    previousMillisSonar = currentMillis;
  }

  /********************************************************************/
}
