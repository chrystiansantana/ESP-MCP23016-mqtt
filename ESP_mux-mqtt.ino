#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

#define host_name "ESP8266"
#define PUBLICAR "ESP8266/sinais"

long unsigned int tempo;
char menssagem[50];

const char* ssid = "SSID";
const char* password = "69665245";
const char* mqtt_server = "192.168.1.14";
IPAddress ip(192, 168, 1, 80);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);

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
void setup() {
  Serial.begin(115200);
  Serial.println("\n\r**(Setup)**\n\r");

  conectar_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  OTA(); // atualizar via rede
  Serial.println("\n\r**(Setup)**\nPronto");
}
///#############   WiFi   ##############
void conectar_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.hostname(host_name);
  WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, password);

  Serial.println("Conectando WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.write(".");
    delay(1000);
  }
  Serial.println("\n\rWiFi Conectado");
  Serial.println("Hostname: " + WiFi.hostname());
  Serial.println("IP: " + WiFi.localIP());
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
  //ArduinoOTA.handle();
  if (!client.connected()) {
    reconnect();
  }
  //snprintf (menssagem, 75, "hello world #%1d", tempo);
  //client.publish(PUBLICAR, menssagem);
  delay(1500);



  client.loop();
}

