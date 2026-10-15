#include <WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "NOME_DO_WIFI";
const char* WIFI_SENHA = "SENHA_DO_WIFI";

const char* MQTT_SERVIDOR = "broker.hivemq.com";
const uint16_t MQTT_PORTA = 1883;

const char* MQTT_TOPICO = "unifor/t169-10/2026-2/p5/equipe01-a7f2/distancia_cm";

const uint8_t PINO_TRIG = 18;
const uint8_t PINO_ECHO = 19;

const unsigned long TIMEOUT_ECHO_US = 30000;
const unsigned long INTERVALO_LEITURA_MS = 1000;
const unsigned long INTERVALO_RECONEXAO_MS = 5000;

WiFiClient clienteWiFi;
PubSubClient clienteMQTT(clienteWiFi);

char clientId[40];

unsigned long ultimaLeitura = 0;
unsigned long ultimaTentativaWiFi = 0;
unsigned long ultimaTentativaMQTT = 0;

bool wifiEstavaConectado = false;

bool medirDistancia(float& distanciaCm) {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  unsigned long duracao = pulseIn(
    PINO_ECHO,
    HIGH,
    TIMEOUT_ECHO_US
  );

  if (duracao == 0) {
    return false;
  }

  distanciaCm = duracao * 0.0343f / 2.0f;

  return distanciaCm >= 2.0f && distanciaCm <= 400.0f;
}

void manterConexoes() {
  unsigned long agora = millis();

  if (WiFi.status() != WL_CONNECTED) {
    wifiEstavaConectado = false;

    if (clienteMQTT.connected()) {
      clienteMQTT.disconnect();
    }

    if (agora - ultimaTentativaWiFi >= INTERVALO_RECONEXAO_MS) {
      ultimaTentativaWiFi = agora;

      Serial.println("Tentando reconectar ao Wi-Fi...");
      WiFi.reconnect();
    }

    return;
  }

  if (!wifiEstavaConectado) {
    wifiEstavaConectado = true;

    Serial.print("Wi-Fi conectado. IP: ");
    Serial.println(WiFi.localIP());

    ultimaTentativaMQTT = agora - INTERVALO_RECONEXAO_MS;
  }

  if (!clienteMQTT.connected() &&
      agora - ultimaTentativaMQTT >= INTERVALO_RECONEXAO_MS) {

    ultimaTentativaMQTT = agora;

    Serial.print("Conectando ao broker MQTT... ");

    if (clienteMQTT.connect(clientId)) {
      Serial.println("conectado!");
    } else {
      Serial.print("falhou. Codigo: ");
      Serial.println(clienteMQTT.state());
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
  digitalWrite(PINO_TRIG, LOW);

  uint64_t chipId = ESP.getEfuseMac();

  snprintf(
    clientId,
    sizeof(clientId),
    "p5-esp32-%04X%08X",
    (unsigned int)(chipId >> 32),
    (unsigned int)(chipId & 0xFFFFFFFFULL)
  );

  clienteMQTT.setServer(MQTT_SERVIDOR, MQTT_PORTA);
  clienteMQTT.setKeepAlive(30);
  clienteMQTT.setSocketTimeout(2);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_SENHA);

  ultimaTentativaWiFi = millis();

  Serial.println("Iniciando pratica 5...");
  Serial.print("Client ID: ");
  Serial.println(clientId);
  Serial.print("Topico: ");
  Serial.println(MQTT_TOPICO);
  Serial.println("Conectando ao Wi-Fi...");
}

void loop() {
  manterConexoes();

  if (!clienteMQTT.connected()) {
    return;
  }

  clienteMQTT.loop();

  unsigned long agora = millis();

  if (agora - ultimaLeitura >= INTERVALO_LEITURA_MS) {
    ultimaLeitura = agora;

    float distanciaCm;

    if (!medirDistancia(distanciaCm)) {
      Serial.println("Leitura invalida: nao publicada.");
      return;
    }

    char mensagem[16];

    snprintf(mensagem, sizeof(mensagem), "%.1f", distanciaCm);


    bool publicado = clienteMQTT.publish(
      MQTT_TOPICO,
      mensagem,
      false
    );

    if (publicado) {
      Serial.print("Distancia publicada: ");
      Serial.print(mensagem);
      Serial.println(" cm");
    } else {
      Serial.println("Falha ao publicar a distancia.");
    }
  }
}