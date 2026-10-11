/*
  Proyecto: Sensor infrarrojo de obstaculos con ESP32
  Curso:    Robotica
  Objetivo: detectar objetos frente al sensor y mostrarlo en el Monitor Serie.

  Conexiones (ESP32 DevKit USB-C, chip CP2102):
    Sensor VCC -> ESP32 3V3
    Sensor GND -> ESP32 GND
    Sensor OUT -> ESP32 D4 (GPIO 4)

  El modulo trae un LED emisor IR (transparente) y un receptor (negro).
  Cuando un objeto refleja la luz IR, el comparador LM393 pone OUT en LOW.
  La distancia de deteccion se ajusta con el potenciometro azul.
  Nota: la luz solar directa contiene IR y puede provocar falsas detecciones.

  Consola: 115200 baudios.
*/

const int PIN_SENSOR = 4;
const unsigned long PERIODO_MS = 50;        // lectura del sensor cada 50 ms
const unsigned long REPORTE_MS = 3000;      // linea de estado cada 3 s

bool hayObstaculoAntes = false;
unsigned long detecciones = 0;
unsigned long inicioObstaculo = 0;          // momento en que aparecio el objeto
unsigned long ultimoReporte = 0;

bool leerSensor() {
  return digitalRead(PIN_SENSOR) == LOW;    // salida activa en bajo
}

void imprimirTiempo() {
  Serial.print("[");
  Serial.print(millis() / 1000.0, 1);
  Serial.print(" s] ");
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_SENSOR, INPUT);
  delay(500);

  Serial.println();
  Serial.println("=========================================");
  Serial.println("  Sensor IR de obstaculos + ESP32");
  Serial.println("  Salida OUT -> GPIO 4 (activa en LOW)");
  Serial.println("=========================================");

  hayObstaculoAntes = leerSensor();
  imprimirTiempo();
  Serial.print("Estado inicial: ");
  Serial.println(hayObstaculoAntes ? "OBSTACULO" : "LIBRE");
}

void loop() {
  bool hayObstaculo = leerSensor();

  // evento: el estado cambio
  if (hayObstaculo != hayObstaculoAntes) {
    imprimirTiempo();
    if (hayObstaculo) {
      detecciones++;
      inicioObstaculo = millis();
      Serial.print(">>> OBSTACULO DETECTADO  (deteccion #");
      Serial.print(detecciones);
      Serial.println(")");
    } else {
      Serial.print("<<< Camino libre  (el objeto estuvo ");
      Serial.print((millis() - inicioObstaculo) / 1000.0, 1);
      Serial.println(" s)");
    }
    hayObstaculoAntes = hayObstaculo;
  }

  // reporte periodico del estado
  if (millis() - ultimoReporte >= REPORTE_MS) {
    ultimoReporte = millis();
    imprimirTiempo();
    Serial.print("Estado: ");
    Serial.print(hayObstaculo ? "OBSTACULO" : "LIBRE    ");
    Serial.print(" | detecciones totales: ");
    Serial.println(detecciones);
  }

  delay(PERIODO_MS);
}
