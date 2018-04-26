/*
 * iotGrupo4
 * Frederico Sales <frederico.sales@engenharia.ufjf.br> (201765803B)
 * Pedro Henrique Linhares Oliveira (201565117/AC)
 * Ronaldo
 * VH
 * Topicos de Redes e Processamento Distribuido I
 * Engenharia Computacional  - UFJF
 * mqtt_nodeMCU.cpp
 *
 */

 #ifndef main_h
 #define main_h

// include
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// define
#define SSID "iot"
#define PASSW "netdascoisas#"
#define BROKER_MQTT "iot.eclipse.org"
#define BROKER_PORT 1883
#define TOPIC_SUBSCRIBE "dcc091Envia"
#define TOPIC_PUBLISH "dcc091Recebe"
#define ID_MQTT "iotGrupo4"
#define ACCESSTOKEN "RrkxNM45ukp3rgbUh8lm"
#define THINGSBOARDSERVER ""
#define DEVICEID "fad77ad0-4733-11e8-93e0-c3b186e30863"

// delay
#define TTIME0 500
#define TTIME1 10
#define TTIME2 1000
#define TTIME3 2000
#define TTIME4 5000
#define TTIME5 10000

// serial
#define SOFSPD 36400
#define SERSPD 115200

// pin nodemcu
#define D0  16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15
#define D9  3
#define D10 1

// Edelberto saida do nodemcu
#define DCC091 5
#define LM35 A0

// var
char EstadoSaida = '0';
unsigned long lastSend;
int status = WL_IDLE_STATUS;

// objects
WiFiServer server(80);
WiFiClient espClient;
PubSubClient MQTT(espClient);


// prototypes
void gpio();
void setSerial(void);
void setWifi(void);
void setMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnMQTT(void);
void reconnWifi(void);
void checkConnWiFiMQTT(void);
void sendOutputStatMQTT(void);
float temp(int);
void getAndSendTemp(void);


// functions

// gPio
void gpio() {
  pinMode(DCC091, OUTPUT);
  digitalWrite(DCC091, LOW);
  return;
}

// setSerial
void setSerial() {
  Serial.begin(SERSPD);
  delay(TTIME1);
  return;
}

// setWifi
void setWifi() {
  delay(TTIME3);
  Serial.println("---------- WIFI CONECTION ----------");
  Serial.print("connecting: ");
  Serial.println(SSID);
  Serial.println("please wait...");
  reconnWifi();
}

// setMQTT
void setMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

// setCallback
void mqtt_callback(char* topic, byte* payload, unsigned int length)  {
  String msg;
  for (size_t i = 0; i < length; i++) {
    char c = (char)payload[i];
    msg += c;
  }

  if(msg.equals("L")) {
    digitalWrite(DCC091, HIGH);
    EstadoSaida = '1';
  }

  if(msg.equals("D")) {
    digitalWrite(DCC091, LOW);
    EstadoSaida = '0';
  }
}

// reconnMQTT
void reconnMQTT() {
  while(!MQTT.connected()) {
    Serial.print("*Trying reconnect to BROKER: ");
    Serial.println(BROKER_MQTT);

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Successfully connected to BROKER");
      MQTT.subscribe(TOPIC_SUBSCRIBE);
    } else {
      Serial.println("Fail to connect...");
      Serial.println("Retry in 2s...");
      delay(TTIME3);
    }
  }
}

// reconnWifi
void reconnWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  WiFi.begin(SSID, PASSW);

  while(WiFi.status() != WL_CONNECTED) {
    delay(TTIME3);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Successfully connected ");
  Serial.print(SSID);
  Serial.println("Ip acquired: ");
  Serial.println(WiFi.localIP());
}

// checkConnWiFi
void checkConnWiFiMQTT() {
  if (!MQTT.connected())
    reconnMQTT();
  reconnWifi();
}

// sendOutputStatMQTT
void sendOutputStatMQTT() {
  if(EstadoSaida == '0')
    MQTT.publish(TOPIC_PUBLISH, "D");

  if (EstadoSaida == '1')
    MQTT.publish(TOPIC_PUBLISH, "L");

  Serial.println("GPIO status send to BROKER");
  delay(TTIME2);
}

// temp lm35 adc 0 A0 NodeMCU (0 and 1024)
float temp(int debug) {
  /*
   * LM35 chanfer to ceil
   * pin 1 +Vs
   * pin 2 Vout
   * pin 3 GND
   * temp range(-55 to +150 Celsius)
   * +Vs 4vdc to 20vdc 10mV/Celsius
   * basic mode (2 - 150 Celsius)
   * 20C leitura confiavel
   * 30.27 leitura do sensor sem correcao
   */
  float t = 0;
  int input_val;
  input_val = analogRead(LM35);
  t = ((5.0 * input_val * 100.0) / 1024) * 0.660720;
  if (debug == 1){
      Serial.print("temperature (Celsius): ");
      Serial.println(t);
      delay(TTIME3);
  }
  delay(TTIME3);
  return t;
}

// getAndSendTemp
void getAndSendTemp() {
  /* code */
  String payload;
  String temperature;
  char attributes[100];

  temperature = (String)temp(0);
  payload = "{";
  payload += "\"temperature\":"; payload += temperature;
  payload += "}";

  // send payload
  payload.toCharArray(attributes, 100);
  MQTT.publish("v1/devices/me/telemetry", attributes);
}

#endif /* main_h */
