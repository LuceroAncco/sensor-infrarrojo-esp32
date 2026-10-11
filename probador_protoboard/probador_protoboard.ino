/*
  Probador de continuidad para la protoboard usando el ESP32.

  Conexiones (ESP32 FUERA de la protoboard, cables hembra-macho):
    Cable 1: pin GND del ESP32 -> un agujero de la fila a probar
    Cable 2: pin D4  del ESP32 -> otro agujero de la MISMA fila

  D4 se configura con resistencia pull-up interna: si la fila conduce,
  D4 queda unido a GND y lee LOW -> "CONECTADO". Es seguro: no hay corto.
*/

const int PIN_PRUEBA = 4;

int estadoAnterior = -1;
unsigned long conexiones = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_PRUEBA, INPUT_PULLUP);
  delay(500);
  Serial.println();
  Serial.println("=== Probador de protoboard ===");
  Serial.println("Pon los dos cables en la misma fila y mira el resultado.");
}

void loop() {
  int estado = digitalRead(PIN_PRUEBA);
  if (estado != estadoAnterior) {
    if (estado == LOW) {
      conexiones++;
      Serial.print(">>> CONECTADO  (prueba #");
      Serial.print(conexiones);
      Serial.println(")  la fila funciona");
    } else {
      Serial.println("    abierto   (sin contacto)");
    }
    estadoAnterior = estado;
  }
  delay(50);
}
