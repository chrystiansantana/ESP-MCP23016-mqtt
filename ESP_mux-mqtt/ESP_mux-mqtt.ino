#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <Wire.h>

#include <ArduinoOTA.h>
//#include <ESP8266mDNS.h>
//#include <WiFiUdp.h>

#include <PubSubClient.h>
#define host_name "ESP8266"
#define PUBLICAR "ESP8266/sinais"

#define SDA 13 // talves no GPIO2?
#define SCL 12 // talves colocar no GPIO14?

byte MCP_leitura = 0x20; // Endereço do MCP23017 que lê a saida dos reles ( A2=0, A1=0, A0=0)
byte MCP_controle = 0x21; // ,,     ,,    ,,    que dispara os reles    ( A2=0, A1=0, A0=1)
#define IODIRA 0x00 // Registrador IO PORTA
#define IODIRB 0x01 // Registrador IO PORTB
#define IOA 0x12 // IOs A
#define IOB 0x13 // IOs B
byte leituras=0;
long unsigned int tempo;
char menssagem[50];

const char* mqtt_server = "192.168.1.205";

WiFiClient espClient;
PubSubClient client(espClient);

void OTA() {
  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(host_name);

  ArduinoOTA.onStart([]() {
    Serial.println("Iniciando OTA");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("Finalizando OTA");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("%u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Falha no inicio");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Falha coneccao");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Falha ao receber");
    else if (error == OTA_END_ERROR) Serial.println("Falha ao finalizar");
  });
  ArduinoOTA.begin();
}
///############   Setup   ############# 
void setup() {
  Serial.begin(115200); // inicia porta serial para debug
  Serial.println("\n\r**(Setup)**\n\r");

  WiFiManager wifimanager; // Carrega informaçoes wifi da eeprom
  wifimanager.setTimeout(180);
  if(!wifimanager.autoConnect("ESP_Wifi_conf", "espmcp23016")) {
    Serial.println("Falha ao conectar, reset...");// Resetar o ESP caso nao consiga conectar apos o timeout
    delay(3000);
    ESP.reset();
    delay(1000);
  }
  
//  client.setServer(mqtt_server, 1883); // Configura conexão com oservidor mqtt
//  client.setCallback(callback);

  OTA(); // Abilita atualização do codigo via rede

  Wire.begin(5,4); // ativa canal i2c
  /*Wire.beginTransmission(MCP_controle); // inicia transmissao com mcp de controle
  Wire.write(IODIRA); // IODIRA registrador A
  Wire.write(0x00); // seta todas as IOs A para OUTPUT
  Wire.endTransmission(); // finaliza transmissao*/
    
  Serial.println("\n\r**(Setup Concluido)**\n");
}
///#############   MQTT   ##############
void callback(char* topic, byte*payload, unsigned int length) {
  String comando = "";
  String msgrecebida = "Menssagem recebida [" + String(topic) + "]";
  Serial.print(msgrecebida);
  for (int i = 0; i < length; i++) {
    comando += (char)payload[i];
    Serial.println((char)payload[i]);
  }
  Serial.println();
  if (comando == "teste") {
    client.publish(PUBLICAR, "Teste recebido");
  } else  if (comando == "luz1on") {
    client.publish(PUBLICAR, "Luz 1 on");
  } else  if (comando == "luz 1 on") {
    client.publish(PUBLICAR, "Luz 1 on");
  }
  else {
    Serial.println("Recebido o comando: " + comando);
  }
}

void reconnect() {
  Serial.println("Conectando ao MQTT Server");
  if (client.connect(host_name)) {
    Serial.println("Conectado");
    client.subscribe(host_name);
  } else {
    Serial.print("\n\rFalha, rc= ");
    Serial.print(client.state());
  }
}
///#############   ####   ##############
void loop() {
  tempo = millis();
  ArduinoOTA.handle();
/*  if (!client.connected()) {
    reconnect();
  }
*/  //snprintf (menssagem, 75, "hello world #%1d", tempo);
  //client.publish(PUBLICAR, menssagem);
  Wire.beginTransmission(0x20); // Trasmite para MCP de leitura
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x20);
  Wire.write(0x12);
  Wire.write(11000000);
  Wire.endTransmission();
  Serial.println(11000000);
  delay(3000);
  
  Wire.beginTransmission(0x20);
  Wire.write(0x12);
  Wire.write(1000000);
  Wire.endTransmission();
  delay(2000);
  Serial.println(1000000);
  
  Wire.beginTransmission(0x20);
  Wire.write(0x12);
  Wire.write(01000000);
  Wire.endTransmission();
  delay(1000);
  Serial.println(01000000);
  
  Wire.beginTransmission(0x20);
  Wire.write(0x12);
  Wire.write(00000000);
  Wire.endTransmission();
  delay(1000);
  Serial.println(00000000);

  /*
  Wire.requestFrom(0x20, 1);
  leituras=Wire.read();
  if(leituras>0){
    Serial.println(leituras, BIN);
  }
  delay(200);*/
  client.loop();
}

