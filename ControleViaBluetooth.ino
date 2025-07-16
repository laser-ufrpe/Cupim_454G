#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// Pinos dos motores
int motoresPins[3][2] = {
  {13, 14},  // motor esquerdo
  {4, 27},   // motor direito
  {18, 5}    // motor da arma
};

#define LED_MOTOR 5  // Pino do LED (ajustado para evitar conflitos)

bool motorAAtivo = false;
bool controlesInvertidos = false;

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

  // Se os dois motores estão parados (todos os pinos estão LOW), desliga LED
  bool motorEsquerdoParado = digitalRead(motoresPins[0][0]) == LOW && digitalRead(motoresPins[0][1]) == LOW;
  bool motorDireitoParado  = digitalRead(motoresPins[1][0]) == LOW && digitalRead(motoresPins[1][1]) == LOW;
  if (motorEsquerdoParado && motorDireitoParado)
    atualizarLED(false);
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("RoboCupim");  // Nome do dispositivo Bluetooth
  Serial.println("Bluetooth pronto! Use 1 letra para controlar:");

  for (int i = 0; i < 3; i++) {
    pinMode(motoresPins[i][0], OUTPUT);
    pinMode(motoresPins[i][1], OUTPUT);
  }

  pinMode(LED_MOTOR, OUTPUT);
  atualizarLED(false);
}

void loop() {
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.print("Recebido: ");
    Serial.println(c);

    switch (c) {
      case 'F': frente(); break;
      case 'T': tras(); break;
      case 'E': esquerda(); break;
      case 'D': direita(); break;
      case 'N': parado(); break;
      case 'a': armaOn(); break;
      case 'z': armaOff(); break;
      case 'Y':
        controlesInvertidos = !controlesInvertidos;
        Serial.println(controlesInvertidos ? "Invertido: SIM" : "Invertido: NÃO");
        break;
      case 'B': parado(); armaOff(); break;
      default: Serial.println("Comando desconhecido");
    }
  }
}
