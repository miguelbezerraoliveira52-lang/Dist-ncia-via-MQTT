Prática 5 - Distância via MQTT
Projeto da disciplina Sistemas Ciberfísicos e Colaborativos da Unifor.
Uma ESP32 mede a distância com o HC-SR04 e envia leituras para um broker MQTT. O MQTTX no computador permite acompanhar as mensagens.
Materiais
- ESP32 DevKit/WROOM-32 clássica.
- Sensor HC-SR04 de quatro pinos.
- Resistores de 1 kΩ e 2 kΩ, preferencialmente de 1%.
- Protoboard, jumpers e cabo USB de dados.
- Rede Wi-Fi de 2,4 GHz com acesso à internet.
- Arduino IDE e MQTTX Desktop.
Montagem
Monte com o USB desconectado.
| Componente | Ligação |
|---|---|
| VCC do HC-SR04 | 5 V da ESP32 |
| GND do HC-SR04 | GND da ESP32 |
| TRIG do HC-SR04 | GPIO18 |
| ECHO do HC-SR04 | Resistor de 1 kΩ até o nó do GPIO19 |
| Nó do GPIO19 | Resistor de 2 kΩ até o GND |


O resistor de 1 kΩ fica entre ECHO e GPIO19; o de 2 kΩ fica entre GPIO19 e GND. Esse divisor reduz o sinal de aproximadamente 5 V para 3,33 V. Não conecte ECHO diretamente à ESP32. Todos os terras devem estar conectados.
Preparação no Arduino IDE
1. Instale o suporte às placas ESP32 da Espressif pelo Gerenciador de Placas.
2. Instale a biblioteca PubSubClient, de Nick O’Leary, pelo Gerenciador de Bibliotecas. A biblioteca WiFi acompanha o suporte à ESP32.
3. Salve o arquivo Pratica_5_MQTT.ino dentro de uma pasta chamada Pratica_5_MQTT e abra-o no Arduino IDE.
4. Altere WIFI_SSID e WIFI_SENHA para os dados da sua rede.
5. Em MQTT_TOPICO, substitua equipe01-a7f2 por um identificador exclusivo da equipe.
6. Selecione a placa correspondente à sua ESP32 e a porta USB.
7. Verifique o código, envie à placa e abra o Monitor Serial em 115200 baud.
| Campo | Valor |
|---|---|
| Transporte | MQTT sobre TCP |
| Host | broker.hivemq.com |
| Porta | 1883 |
| Versão | MQTT 3.1.1 |
| TLS/SSL | Desativado |
| Usuário e senha | Em branco |
| Client ID | Um identificador exclusivo para o computador |
| QoS da assinatura | 0 |


Após conectar, crie uma assinatura com o mesmo tópico definido em MQTT_TOPICO, respeitando maiúsculas e minúsculas.
Exemplo de tópico:
unifor/t169-10/2026-2/p5/equipe01-a7f2/distancia_cm
A ESP32 gera seu próprio Client ID com base no identificador do chip. O computador deve usar um Client ID diferente.
Funcionamento
- Emite um pulso de 10 microssegundos no TRIG.
- Aguarda o ECHO por até 30.000 microssegundos.
- Calcula a distância: duração × 0,0343 ÷ 2.
- Descarta ausência de resposta e valores fora de 2 a 400 cm, sem publicar zero.
- Publica leituras válidas aproximadamente uma vez por segundo.
- Envia texto com uma casa decimal e ponto, por exemplo 25.4.
- Usa QoS 0 e sem retenção.
- Mantém o processamento MQTT e tenta reconectar após falhas.
As tentativas de reconexão são espaçadas em cinco segundos. Uma tentativa de conexão pode causar uma breve espera; o intervalo de publicação é aproximado.
Teste na bancada
1. Conecte o MQTTX e assine o tópico da equipe.
2. Ligue a ESP32 e confira as conexões pelo Monitor Serial.
3. Posicione um objeto plano entre 2 e 400 cm do sensor.
4. Mova o objeto e observe as mudanças no MQTTX.
5. Retire o objeto do alcance: leituras inválidas devem ser descartadas.
6. Interrompa e restabeleça a rede para observar a reconexão.
| Problema | Verificação |
|---|---|
| PubSubClient.h não encontrado | Instale a biblioteca PubSubClient |
| WiFi.h não encontrado | Confira o suporte à ESP32 e a placa selecionada |
| Wi-Fi não conecta | Confira SSID, senha e rede de 2,4 GHz |
| MQTTX não recebe | Confira broker, porta e tópico exato; conecte e depois assine |
| Leituras inválidas | Confira alimentação, divisor, GND, pinos e posição do objeto |
| Monitor Serial ilegível | Selecione 115200 baud |



Observações
O broker é público e a porta 1883 não usa criptografia. Publique apenas as leituras da atividade. Não envie credenciais ou dados pessoais.
O código foi preparado conforme o enunciado da Prática 5. A compilação no Arduino IDE e o funcionamento no hardware precisam ser verificados na bancada.
