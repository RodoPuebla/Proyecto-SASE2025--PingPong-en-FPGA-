#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// Entradas desde la FPGA
#define ENTRADA_GANADOR_2     4  // Solo para contar flancos
#define ENTRADA_GANADOR      15  // Dispara el apagado de LEDs
#define ENTRADA_STRART_C1    21  // START físico (nuevo)

// LEDs de puntos (empiezan todos encendidos)
#define LED_PUNTO_1         32
#define LED_PUNTO_2         33
#define LED_PUNTO_3         25

// LED de estado Bluetooth
#define LED_BLUETOOTH       26

// Salidas hacia la FPGA
#define SALIDA_A            5
#define SALIDA_B            0
#define SALIDA_START        23

// Variables de movimiento/start
int codigo = 0b11;
bool startActiva = false;
unsigned long tiempoStart = 0;
const unsigned long duracionPulso = 100;

// Estado de la conexión BT
bool conectado = false;

// Para detectar flancos
bool ganador1Anterior = false;
bool ganador2Anterior = false;
bool startC1Anterior  = false;  // <- nuevo

// Contadores de “vidas” para cada entrada
int puntosGanador1 = 3;   // no controla LEDs, solo reset
int puntosGanador2 = 3;   // sí controla LEDs

void setup() {
  Serial.begin(115200);
  SerialBT.begin("control2");
  Serial.println("Bluetooth iniciado. Esperando conexión...");

  // Pines de entrada
  pinMode(ENTRADA_GANADOR,    INPUT);
  pinMode(ENTRADA_GANADOR_2,  INPUT);
  pinMode(ENTRADA_STRART_C1,  INPUT); // <- nuevo

  // Pines de salida hacia FPGA
  pinMode(SALIDA_A,     OUTPUT);
  pinMode(SALIDA_B,     OUTPUT);
  pinMode(SALIDA_START, OUTPUT);

  // Pines de LEDs
  pinMode(LED_PUNTO_1,   OUTPUT);
  pinMode(LED_PUNTO_2,   OUTPUT);
  pinMode(LED_PUNTO_3,   OUTPUT);
  pinMode(LED_BLUETOOTH, OUTPUT);

  // Estado inicial
  digitalWrite(SALIDA_START, LOW);
  reiniciarTodos();           // Enciende los 3 LEDs y pone ambos contadores a 3
  digitalWrite(LED_BLUETOOTH, LOW);
}

void reiniciarTodos() {
    puntosGanador1 = 3;
    puntosGanador2 = 3;

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


void apagarUnPunto() {
  // Apaga LEDs en orden 1 → 2 → 3
  if (puntosGanador2 == 3)      digitalWrite(LED_PUNTO_1, LOW);
  else if (puntosGanador2 == 2) digitalWrite(LED_PUNTO_2, LOW);
  else if (puntosGanador2 == 1) digitalWrite(LED_PUNTO_3, LOW);

  if (puntosGanador2 > 0) puntosGanador2--;
}

void loop() {
  // 1) Estado de conexión Bluetooth
  if (SerialBT.hasClient()) {
    if (!conectado) {
      Serial.println("Bluetooth conectado");
      conectado = true;
      digitalWrite(LED_BLUETOOTH, HIGH);
    }
  } else {
    if (conectado) {
      Serial.println("Bluetooth desconectado");
      conectado = false;
      digitalWrite(LED_BLUETOOTH, LOW);
    }
  }

  // 2) Lectura de comandos por Bluetooth
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.print("Recibido por BT: ");
    Serial.println(c);
    switch (c) {
      case 'F':  codigo = 0b01; break;  // Adelante
      case 'B':  codigo = 0b10; break;  // Atrás
      case 'S':  codigo = 0b00; break;  // Stop
      case 'L':  // START
        startActiva = true;
        tiempoStart = millis();
//        if (puntosGanador1 == 0 || puntosGanador2 == 0) {
//          reiniciarTodos();
//        }
        break;
    }
  }

  // 3) Envío de dirección a la FPGA
  digitalWrite(SALIDA_A, (codigo >> 1) & 0x01);
  digitalWrite(SALIDA_B,  codigo       & 0x01);

  // 4) Generación del pulso de START
  if (startActiva) {
    digitalWrite(SALIDA_START, HIGH);
    if (millis() - tiempoStart >= duracionPulso) {
      digitalWrite(SALIDA_START, LOW);
      startActiva = false;
    }
  }

  // 5) Detección de flancos en ENTRADA_GANADOR (solo cuenta)
  bool g1 = digitalRead(ENTRADA_GANADOR);
  if (!ganador1Anterior && g1 && puntosGanador1 > 0) {
    puntosGanador1--;
  }
  ganador1Anterior = g1;

  // 6) Detección de flancos en ENTRADA_GANADOR_2 (apaga LED y cuenta)
  bool g2 = digitalRead(ENTRADA_GANADOR_2);
  if (!ganador2Anterior && g2 && puntosGanador2 > 0) {
    apagarUnPunto();
  }
  ganador2Anterior = g2;

  // 7) START desde pin físico C1 (flanco de subida)
    if (puntosGanador1 == 0 || puntosGanador2 == 0) {
      reiniciarTodos();
    }
  

  delay(5);
}
