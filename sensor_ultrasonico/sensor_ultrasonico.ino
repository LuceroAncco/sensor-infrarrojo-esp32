/*
  Proyecto: Lectura del sensor ultrasonico HC-SR04 con ESP32
  Curso:    Robotica
  Objetivo: medir la distancia a un objeto y mostrarla en el Monitor Serie.

  Conexiones (ESP32 DevKit USB-C, chip CP2102):
    HC-SR04 VCC  -> ESP32 3V3
    HC-SR04 GND  -> ESP32 GND
    HC-SR04 Trig -> ESP32 D5  (GPIO 5)
    HC-SR04 Echo -> ESP32 D18 (GPIO 18)

  Se alimenta con 3.3 V para que el pin Echo no entregue 5 V al ESP32
  (sus pines solo toleran 3.3 V) y asi no hacen falta resistencias.

  Principio: el sensor emite un pulso de ultrasonido (40 kHz) y mide cuanto
  tarda el eco en volver. distancia = (tiempo * velocidad_sonido) / 2
*/

const int PIN_TRIG = 5;
const int PIN_ECHO = 18;

const float VELOCIDAD_SONIDO = 0.0343;   // cm por microsegundo (343 m/s a ~20 C)
const unsigned long TIMEOUT_US = 30000;  // ~5 m ida y vuelta; si no hay eco, se descarta
const int MUESTRAS = 5;                  // lecturas por medicion para filtrar ruido
const unsigned long PERIODO_MS = 500;    // una medicion cada medio segundo

unsigned long numeroLectura = 0;

// Dispara el sensor una vez y devuelve la distancia en cm (o -1 si no hubo eco).
float medirUnaVez() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);   // pulso de 10 us para iniciar la medicion
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIMEOUT_US);
  if (duracion == 0) {
    return -1;
  }
  return duracion * VELOCIDAD_SONIDO / 2.0;
}

// Toma varias lecturas y devuelve la mediana, que ignora valores aislados erroneos.
float medirDistancia() {
  float lecturas[MUESTRAS];
  int validas = 0;

  for (int i = 0; i < MUESTRAS; i++) {
    float d = medirUnaVez();
    if (d > 0) {
      lecturas[validas++] = d;
    }
    delay(10);  // deja que se apague el eco anterior
  }
  if (validas == 0) {
    return -1;
  }

  // ordenamiento simple (pocas muestras)
  for (int i = 0; i < validas - 1; i++) {
    for (int j = i + 1; j < validas; j++) {
      if (lecturas[j] < lecturas[i]) {
        float t = lecturas[i];
        lecturas[i] = lecturas[j];
        lecturas[j] = t;
      }
    }
  }
  return lecturas[validas / 2];
}

const char* clasificar(float cm) {
  if (cm < 10)  return "MUY CERCA";
  if (cm < 30)  return "CERCA";
  if (cm < 100) return "MEDIO";
  return "LEJOS";
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  delay(500);

  Serial.println();
  Serial.println("=========================================");
  Serial.println("  Sensor ultrasonico HC-SR04 + ESP32");
  Serial.println("  Rango util: 2 cm a 400 cm");
  Serial.println("=========================================");
  Serial.println("  N\tTiempo(s)\tDistancia\tEstado");
}

void loop() {
  float cm = medirDistancia();
  numeroLectura++;

  Serial.print(numeroLectura);
  Serial.print("\t");
  Serial.print(millis() / 1000.0, 1);
  Serial.print("\t\t");

  if (cm < 0 || cm > 400) {
    Serial.println("---\t\tFUERA DE RANGO");
  } else {
    Serial.print(cm, 1);
    Serial.print(" cm\t");
    Serial.println(clasificar(cm));
  }

  delay(PERIODO_MS);
}
