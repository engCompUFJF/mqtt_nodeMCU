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

// include
#include "mqtt_nodeMCU.h"

// setup
void setup() {
    // NodeMCU MQTT
    setSerial();
    gpio();
    setWifi();
    setMQTT();
}

// loop
void loop() {
    // NodeMCU MQTT
    checkConnWiFiMQTT();
    sendOutputStatMQTT();
    //temp(1);                            // teste do sensor de temperatura
    if (millis() - lastSend > TTIME2) {   // send temperature to mqtt broker
      getAndSendTemp();
      lastSend = millis();
    }
    MQTT.loop();
}
