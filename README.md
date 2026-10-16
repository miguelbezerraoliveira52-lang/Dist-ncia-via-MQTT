# Prática 5 — Medição de Distância via MQTT

Projeto desenvolvido para a disciplina **Sistemas Ciberfísicos e Colaborativos**, da Universidade de Fortaleza (Unifor).

## Descrição

O sistema utiliza uma **ESP32** e um sensor ultrassônico **HC-SR04** para medir a distância de um objeto e publicar as leituras em um tópico MQTT. As mensagens podem ser acompanhadas no computador pelo **MQTTX**.

## Materiais necessários

- ESP32 DevKit/WROOM-32.
- Sensor ultrassônico HC-SR04.
- Resistor de 1 kΩ.
- Resistor de 2 kΩ.
- Protoboard e jumpers.
- Cabo USB de dados.
- Rede Wi-Fi de 2,4 GHz com internet.

## Tecnologias utilizadas

- C++ para Arduino.
- Arduino IDE.
- Biblioteca WiFi do suporte à ESP32.
- Biblioteca PubSubClient.
- Protocolo MQTT.
- MQTTX Desktop.

## Ligações do circuito

| Componente | Conexão |
|---|---|
| VCC do HC-SR04 | 5 V da ESP32 |
| GND do HC-SR04 | GND da ESP32 |
| TRIG do HC-SR04 | GPIO18 |
| ECHO do HC-SR04 | Resistor de 1 kΩ até o nó do GPIO19 |
| Nó do GPIO19 | Resistor de 2 kΩ até o GND |

**Atenção:** o ECHO do sensor fornece um sinal de 5 V e não deve ser conectado diretamente à ESP32. O divisor de tensão com os dois resistores reduz esse sinal para aproximadamente 3,33 V. Monte o circuito com o USB desconectado.

## Configuração no Arduino IDE

1. Instale o suporte às placas **ESP32 da Espressif** no Gerenciador de Placas.
2. Instale a biblioteca **PubSubClient**, de Nick O’Leary, no Gerenciador de Bibliotecas.
3. Abra o código no Arduino IDE.
4. Altere as configurações do Wi-Fi:

```cpp
const char* WIFI_SSID = "NOME_DO_WIFI";
const char* WIFI_SENHA = "SENHA_DO_WIFI";
```

5. Substitua o identificador da equipe no tópico por um nome exclusivo:

```cpp
const char* MQTT_TOPICO =
  "unifor/t169-10/2026-2/p5/equipe01-a7f2/distancia_cm";
```

6. Selecione a placa correspondente à ESP32 e a porta USB.
7. Compile e envie o código.
8. Abra o Monitor Serial na velocidade de **115200 baud**.

## Configuração do MQTTX

Crie uma conexão com os seguintes parâmetros:

| Campo | Valor |
|---|---|
| Transporte | MQTT sobre TCP |
| Host | `broker.hivemq.com` |
| Porta | `1883` |
| Versão | MQTT 3.1.1 |
| TLS/SSL | Desativado |
| Usuário e senha | Em branco |
| Client ID | Identificador exclusivo para o computador |

Após conectar, crie uma assinatura com **QoS 0**, utilizando exatamente o mesmo tópico definido no código.

O Client ID do MQTTX deve ser diferente do utilizado pela ESP32. O código gera automaticamente o identificador da placa com base no chip.

## Funcionamento

1. A ESP32 conecta-se ao Wi-Fi e ao broker MQTT.
2. Envia um pulso de 10 microssegundos ao pino TRIG.
3. Mede a duração do pulso recebido no ECHO, com espera máxima de 30.000 microssegundos.
4. Calcula a distância usando a fórmula:

```text
Distância (cm) = duração (µs) × 0,0343 ÷ 2
```

5. Descarta leituras sem resposta ou fora da faixa de **2 a 400 cm**.
6. Publica as leituras válidas aproximadamente uma vez por segundo.

As mensagens contêm apenas o valor numérico com uma casa decimal, como `25.4`, representando **25,4 cm**. A publicação utiliza **QoS 0 e sem retenção**.

O programa também mantém o processamento MQTT e realiza tentativas de reconexão quando necessário.

## Como testar

- Conecte o MQTTX ao broker e assine o tópico da equipe.
- Ligue a ESP32 e acompanhe as mensagens no Monitor Serial.
- Posicione um objeto plano na frente do sensor.
- Aproxime e afaste o objeto para observar a variação da distância.
- Confira se as leituras aparecem no MQTTX.
- Retire o objeto do alcance para verificar o descarte das leituras inválidas.

## Observações
O broker utilizado é público e a conexão na porta 1883 não possui criptografia. Envie apenas as leituras da atividade, sem dados pessoais ou credenciais.
O código foi preparado conforme o enunciado da prática. A compilação e o funcionamento com os componentes devem ser verificados na bancada.
