//exemplo base
//referência: https://raw.githubusercontent.com/vinicius0082/esp32-tagoIO/main/exemplos/mpu_esp32_mqtt_tagoIO


//Envio de Dados para tagoIO via MQTT


#include <ArduinoJson.h>
#include "EspMQTTClient.h"
#define sensorEntrada 26
#define sensorSaida 20
#define sensorBuzz 33

//pinos de entrada e saída
const int pino_botao = 25; //input
const int pino_led = 22; //output

//variáveis para Json
char json_btn[100];
char json_pessoas[1000];

//variáveis internas
int valor_btn;
int pessoasDentro = 0;


//configurações da conexão MQTT
EspMQTTClient client
(
  "FIESC_IOT", //nome da sua rede Wi-Fi
  "C6qnM4ag81", //senha da sua rede Wi-Fi
  "mqtt.tago.io",  // MQTT Broker server ip padrão da tago
  "Token",   // username
  "ef4c94f8-a16a-46a5-8141-07f572c073b8",   // Código do Token
  "TestClient",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

//configuração dos pinos
void setup()
{
  
    Serial.begin(9600);
    pinMode(sensorBuzz, OUTPUT);
    pinMode(sensorEntrada, INPUT_PULLUP);
    pinMode(sensorSaida, INPUT_PULLUP);
    pinMode(3, OUTPUT);
  //configurações dos pinos
  pinMode(pino_botao, INPUT); //configura o pino como entrada
  //pinMode(pino_led, OUTPUT); //configura o pino como entrada
  digitalWrite(pino_botao, HIGH); //ativa pull-up no pino de entrada
}

void leitura_sinais()
{
  valor_btn = digitalRead(pino_botao);
}

void converte_json()
{
  StaticJsonDocument<300> sjson_btn;

  sjson_btn["variable"] = "botao";
  sjson_btn["value"] = valor_btn;
  serializeJson(sjson_btn, json_btn);

  StaticJsonDocument<300> sjson_pessoas;

  sjson_pessoas["variable"] = "Convidados";
  sjson_pessoas["value"] = pessoasDentro;
  serializeJson(sjson_pessoas, json_pessoas);
}


void envia_msg()
{
  client.publish("/botao", json_pessoas); // You can activate the retain flag by setting the third parameter to true
}

//loop do programa
void loop()
{
    
      contador();
   delay(100);
      contador();
   delay(100);
      contador();
   delay(100);
      contador();
   delay(100);
    leitura_sinais();
    converte_json();
    envia_msg();

  

  client.loop();
}

void processa_msg(const String payload)
{
  StaticJsonDocument<300> msg;

  DeserializationError err = deserializeJson(msg, payload);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.f_str());
  }
  Serial.print("var:");  
  String var = msg["variable"];
  Serial.println(var);
  if(var == "status")
  {
    Serial.print("value:");
    String val = msg["value"];
    Serial.println(val);
    if(val == "on")
      digitalWrite(pino_led, LOW);
    else
      digitalWrite(pino_led, HIGH);
  }
 
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
   client.subscribe("node/status", [] (const String &payload)  {
   Serial.println(payload);
   processa_msg(payload);
  });
}

void contador()
{
    if (digitalRead(sensorEntrada) == 0)
    {
      
        pessoasDentro++;
        Serial.print("chegou alguem\n");
        Serial.print(pessoasDentro);
        analogWrite(sensorBuzz, HIGH);
        delay(200);
        analogWrite(sensorBuzz, LOW);
        delay(200);
        while(digitalRead(sensorEntrada) == 0);
          
        
    }

}
