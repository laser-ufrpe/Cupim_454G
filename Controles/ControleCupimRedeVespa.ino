#include <WiFi.h>
#include <WebServer.h>

// Pinos dos motores
int motoresPins[3][2] = {
  {13, 14},  // motor esquerdo
  {4, 27},   // motor direito
  {18, 5}    // motor da arma
};

#define LED_MOTOR 2

bool motorAAtivo = false;
bool controlesInvertidos = false;

// Configurações do Wi-Fi AP
const char* ssid = "RoboCupim";
const char* password = "12345678";

WebServer server(80);

void atualizarLED(bool ligado) {
  digitalWrite(LED_MOTOR, ligado ? HIGH : LOW);
}

void frente() {
  digitalWrite(motoresPins[0][0], HIGH); digitalWrite(motoresPins[0][1], LOW);
  digitalWrite(motoresPins[1][0], HIGH); digitalWrite(motoresPins[1][1], LOW);
  atualizarLED(true);
}

void tras() {
  digitalWrite(motoresPins[0][0], LOW); digitalWrite(motoresPins[0][1], HIGH);
  digitalWrite(motoresPins[1][0], LOW); digitalWrite(motoresPins[1][1], HIGH);
  atualizarLED(true);
}

void direita() {
  digitalWrite(motoresPins[0][0], LOW); digitalWrite(motoresPins[0][1], HIGH);
  digitalWrite(motoresPins[1][0], HIGH); digitalWrite(motoresPins[1][1], LOW);
  atualizarLED(true);
}

void esquerda() {
  digitalWrite(motoresPins[0][0], HIGH); digitalWrite(motoresPins[0][1], LOW);
  digitalWrite(motoresPins[1][0], LOW); digitalWrite(motoresPins[1][1], HIGH);
  atualizarLED(true);
}

void parado() {
  digitalWrite(motoresPins[0][0], LOW); digitalWrite(motoresPins[0][1], LOW);
  digitalWrite(motoresPins[1][0], LOW); digitalWrite(motoresPins[1][1], LOW);
  if (!motorAAtivo) atualizarLED(false);
}

void armaOn() {
  motorAAtivo = true;
  atualizarLED(true);
  if (controlesInvertidos) {
    digitalWrite(motoresPins[2][0], HIGH);
    digitalWrite(motoresPins[2][1], LOW);
  } else {
    digitalWrite(motoresPins[2][0], LOW);
    digitalWrite(motoresPins[2][1], HIGH);
  }
}

void armaOff() {
  motorAAtivo = false;
  digitalWrite(motoresPins[2][0], LOW);
  digitalWrite(motoresPins[2][1], LOW);

  bool motorEsquerdoParado = digitalRead(motoresPins[0][0]) == LOW && digitalRead(motoresPins[0][1]) == LOW;
  bool motorDireitoParado  = digitalRead(motoresPins[1][0]) == LOW && digitalRead(motoresPins[1][1]) == LOW;
  if (motorEsquerdoParado && motorDireitoParado)
    atualizarLED(false);
}

// Página Web HTML
String paginaControle = R"rawlit(
<!DOCTYPE html><html><head>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<style>
  body { text-align:center; font-family:Arial; }
  button { width:80px; height:80px; margin:10px; font-size:20px; border-radius:10px; }
</style>
</head><body>
<h2>Controle RoboCupim</h2>
<div>
  <button onclick="enviar('/frente')">↑</button><br>
  <button onclick="enviar('/esquerda')">←</button>
  <button onclick="enviar('/parar')">■</button>
  <button onclick="enviar('/direita')">→</button><br>
  <button onclick="enviar('/tras')">↓</button><br><br>
  <button onclick="enviar('/armaOn')">Arma ON</button>
  <button onclick="enviar('/armaOff')">Arma OFF</button><br>
  <button onclick="enviar('/inverter')">Inverter</button>
</div>
<script>
  function enviar(comando) {
    fetch(comando);
  }
</script>
</body></html>
)rawlit";

void setupRotas() {
  server.on("/", []() { server.send(200, "text/html", paginaControle); });
  server.on("/frente", []() { frente(); server.send(200, "text/plain", "Frente"); });
  server.on("/tras", []() { tras(); server.send(200, "text/plain", "Trás"); });
  server.on("/esquerda", []() { esquerda(); server.send(200, "text/plain", "Esquerda"); });
  server.on("/direita", []() { direita(); server.send(200, "text/plain", "Direita"); });
  server.on("/parar", []() { parado(); server.send(200, "text/plain", "Parar"); });
  server.on("/armaOn", []() { armaOn(); server.send(200, "text/plain", "Arma Ligada"); });
  server.on("/armaOff", []() { armaOff(); server.send(200, "text/plain", "Arma Desligada"); });
  server.on("/inverter", []() {
    controlesInvertidos = !controlesInvertidos;
    server.send(200, "text/plain", String("Invertido: ") + (controlesInvertidos ? "SIM" : "NÃO"));
  });
}

void setup() {
  Serial.begin(115200);

  // Inicializa os motores e o LED
  for (int i = 0; i < 3; i++) {
    pinMode(motoresPins[i][0], OUTPUT);
    pinMode(motoresPins[i][1], OUTPUT);
  }
  pinMode(LED_MOTOR, OUTPUT);
  atualizarLED(false);

  // Inicia o Access Point
  WiFi.softAP(ssid, password);
  Serial.print("IP do controle: ");
  Serial.println(WiFi.softAPIP());

  setupRotas();
  server.begin();
}

void loop() {
  server.handleClient();
}
