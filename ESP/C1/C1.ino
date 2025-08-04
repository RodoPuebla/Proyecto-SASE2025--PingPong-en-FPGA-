#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// — Entradas desde la FPGA —
#define ENTRADA_GANADOR     32
#define ENTRADA_GANADOR2    12
#define ENTRADA_COLISION    36
#define ENTRADA_START_C2   34  // Nueva entrada de START desde C2

// — LEDs de “vidas” —
#define LED_PUNTO_1        21
#define LED_PUNTO_2         5
#define LED_PUNTO_3         4

// — LED indicador Bluetooth —
#define LED_BLUETOOTH      15

// — Buzzer para sonidos —
#define LED_BUZZ           23

// — Pines de movimiento y START (ESP → FPGA) —
#define SALIDA_A           25
#define SALIDA_B           14
#define SALIDA_START       13

int codigo = 0b11;
bool startActiva = false;
unsigned long tiempoStart = 0;
const unsigned long duracionPulso = 100;

bool conectadoBT = false;
bool ganador1Anterior = false;
bool ganador2Anterior = false;
bool startC2Anterior = false;

int puntosJugador1 = 3;
int puntosJugador2 = 3;
bool colisionDetectada = false;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Control1");
  Serial.println("Bluetooth iniciado. Esperando conexión...");

  pinMode(ENTRADA_GANADOR,    INPUT);
  pinMode(ENTRADA_GANADOR2,   INPUT);
  pinMode(ENTRADA_COLISION,   INPUT);
  pinMode(ENTRADA_START_C2,  INPUT);

  pinMode(SALIDA_A,     OUTPUT);
  pinMode(SALIDA_B,     OUTPUT);
  pinMode(SALIDA_START, OUTPUT);

  pinMode(LED_PUNTO_1,   OUTPUT);
  pinMode(LED_PUNTO_2,   OUTPUT);
  pinMode(LED_PUNTO_3,   OUTPUT);
  pinMode(LED_BLUETOOTH, OUTPUT);
  pinMode(LED_BUZZ,      OUTPUT);

  digitalWrite(SALIDA_START, LOW);
  digitalWrite(LED_BLUETOOTH, LOW);
  reiniciarPuntos();
}

void reiniciarPuntos() {
  puntosJugador1 = 3;
  puntosJugador2 = 3;

  // Efecto visual de reinicio
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_PUNTO_1, i % 2);
    digitalWrite(LED_PUNTO_2, i % 2);
    digitalWrite(LED_PUNTO_3, i % 2);
    delay(200);
  }

  digitalWrite(LED_PUNTO_1, HIGH);
  digitalWrite(LED_PUNTO_2, HIGH);
  digitalWrite(LED_PUNTO_3, HIGH);
}

void actualizarLEDsVida() {
  digitalWrite(LED_PUNTO_1, puntosJugador1 >= 1 ? HIGH : LOW);
  digitalWrite(LED_PUNTO_2, puntosJugador1 >= 2 ? HIGH : LOW);
  digitalWrite(LED_PUNTO_3, puntosJugador1 >= 3 ? HIGH : LOW);
}

void sonidoVictoria() {
  int melody[]        = {262, 330, 392, 523, 784}; // Do, Mi, Sol, Do’, Sol’
  int noteDurations[] = {200, 200, 200, 400, 400};

  for (int i = 0; i < 5; i++) {
    tone(LED_BUZZ, melody[i], noteDurations[i]);
    delay(noteDurations[i] * 1.2);
    noTone(LED_BUZZ);
  }
}

void efectoGanador() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_PUNTO_1, i % 2);
    digitalWrite(LED_PUNTO_2, i % 2);
    digitalWrite(LED_PUNTO_3, i % 2);
    delay(200);
  }
  actualizarLEDsVida();  // Mostrar las vidas actuales
}

void loop() {
  // 1) Estado de Bluetooth
  if (SerialBT.hasClient()) {
    if (!conectadoBT) {
      Serial.println("Bluetooth conectado");
      conectadoBT = true;
      digitalWrite(LED_BLUETOOTH, HIGH);
    }
  } else {
    if (conectadoBT) {
      Serial.println("Bluetooth desconectado");
      conectadoBT = false;
      digitalWrite(LED_BLUETOOTH, LOW);
    }
  }

  // 2) Comandos desde BT
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.print("Recibido por BT: ");
    Serial.println(c);
    switch (c) {
      case 'F': codigo = 0b01; break;
      case 'B': codigo = 0b10; break;
      case 'S': codigo = 0b00; break;
      case 'L':
        startActiva = true;
        tiempoStart = millis();
//        if (puntosJugador1 == 0 || puntosJugador2 == 0) {
 //         reiniciarPuntos();
  //      }
        break;
    }
  }

  // 3) Movimiento
  digitalWrite(SALIDA_A, (codigo >> 1) & 0x01);
  digitalWrite(SALIDA_B,  codigo       & 0x01);

  // 4) Pulso START
  if (startActiva) {
    digitalWrite(SALIDA_START, HIGH);
    if (millis() - tiempoStart >= duracionPulso) {
      digitalWrite(SALIDA_START, LOW);
      startActiva = false;
    }
  }

  // 5) Flanco de ENTRADA_GANADOR2 → pierde jugador 1
  bool g2 = digitalRead(ENTRADA_GANADOR2);
  if (!ganador2Anterior && g2 && puntosJugador1 > 0) {
    puntosJugador1--;
    actualizarLEDsVida();
    sonidoVictoria();
    if (puntosJugador1 == 0) efectoGanador();
  }
  ganador2Anterior = g2;

  // 6) Flanco de ENTRADA_GANADOR → pierde jugador 2
  bool g1 = digitalRead(ENTRADA_GANADOR);
  if (!ganador1Anterior && g1 && puntosJugador2 > 0) {
    puntosJugador2--;
    sonidoVictoria();
    if (puntosJugador2 == 0) efectoGanador();
  }
  ganador1Anterior = g1;

  // 7) Colisión
  if (digitalRead(ENTRADA_COLISION) == HIGH && !colisionDetectada) {
    colisionDetectada = true;
    Serial.println("Colisión detectada");
    tone(LED_BUZZ, 1200, 200);
    delay(200);
    colisionDetectada = false;
  }

  // 8) START desde control 2

    if (puntosJugador1 == 0 || puntosJugador2 == 0) {
      reiniciarPuntos();
    }
  

//  startC2Anterior = startC2;

  delay(5);
}
