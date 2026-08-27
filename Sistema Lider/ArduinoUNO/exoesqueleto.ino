// // ================================================================
// //  CALIBRACIÓN — OpenCM9.04
// //  3 encoders incrementales + 5 potenciómetros
// //  Encoders alimentados a 5V directo sin divisores
// // ================================================================

// // ----------------------------------------------------------------
// // PINES ENCODERS
// // ----------------------------------------------------------------
// #define ENC1_A  11    // Hombro abducción
// #define ENC1_B  12
// #define ENC2_A  13    // Hombro flexión/extensión
// #define ENC2_B  14
// #define ENC3_A  15    // Codo
// #define ENC3_B  16

// // ----------------------------------------------------------------
// // PINES POTENCIÓMETROS
// // ----------------------------------------------------------------
// #define POT_1   A0    // Pulgar
// #define POT_2   A1    // Índice
// #define POT_3   A2    // Medio
// #define POT_4   A3    // Anular
// #define POT_5   A4    // Meñique

// // ----------------------------------------------------------------
// // PPR por encoder
// // ----------------------------------------------------------------
// #define PPR_ENC1  600
// #define PPR_ENC2  600
// #define PPR_ENC3  200

// #define PASOS_ENC1  (PPR_ENC1 * 4)
// #define PASOS_ENC2  (PPR_ENC2 * 4)
// #define PASOS_ENC3  (PPR_ENC3 * 4)

// #define GRADOS_ENC1  (360.0 / PASOS_ENC1)
// #define GRADOS_ENC2  (360.0 / PASOS_ENC2)
// #define GRADOS_ENC3  (360.0 / PASOS_ENC3)

// // ----------------------------------------------------------------
// // CONFIGURACIÓN POTENCIÓMETROS
// // Ajusta MIN y MAX después de calibrar físicamente cada dedo
// // ----------------------------------------------------------------
// #define POT_ADC_MIN   0      // OpenCM = 12 bits → 0..4095
// #define POT_ADC_MAX   4095
// #define ANGULO_MIN    0.0    // dedo extendido
// #define ANGULO_MAX    90.0   // dedo cerrado

// #define INTERVALO_PRINT  200  // ms

// // ----------------------------------------------------------------
// // VARIABLES ENCODERS
// // ----------------------------------------------------------------
// volatile long conteo1 = 0;
// volatile long conteo2 = 0;
// volatile long conteo3 = 0;

// volatile int ultimo_A1 = LOW;
// volatile int ultimo_A2 = LOW;
// volatile int ultimo_A3 = LOW;

// int dir1 = 1;
// int dir2 = 1;
// int dir3 = 1;

// // Modo activo: 'e' encoders, 'p' potenciómetros
// char modo = 'n';
// int encoderActivo = 0;

// // ----------------------------------------------------------------
// // ISR — OpenCM no usa IRAM_ATTR
// // ----------------------------------------------------------------
// void isr_enc1() {
//   int A = digitalRead(ENC1_A);
//   int B = digitalRead(ENC1_B);
//   if (A != ultimo_A1) {
//     conteo1 += (A == B) ? -1 : 1;
//     ultimo_A1 = A;
//   }
// }

// void isr_enc2() {
//   int A = digitalRead(ENC2_A);
//   int B = digitalRead(ENC2_B);
//   if (A != ultimo_A2) {
//     conteo2 += (A == B) ? -1 : 1;
//     ultimo_A2 = A;
//   }
// }

// void isr_enc3() {
//   int A = digitalRead(ENC3_A);
//   int B = digitalRead(ENC3_B);
//   if (A != ultimo_A3) {
//     conteo3 += (A == B) ? -1 : 1;
//     ultimo_A3 = A;
//   }
// }

// // ----------------------------------------------------------------
// // CONVERSIONES
// // ----------------------------------------------------------------
// float enc1AGrados(long c) { return c * GRADOS_ENC1; }
// float enc2AGrados(long c) { return c * GRADOS_ENC2; }
// float enc3AGrados(long c) { return c * GRADOS_ENC3; }

// float potAGrados(int pin) {
//   int raw = analogRead(pin);
//   return (float)(raw - POT_ADC_MIN) * (ANGULO_MAX - ANGULO_MIN)
//          / (POT_ADC_MAX - POT_ADC_MIN) + ANGULO_MIN;
// }

// // ----------------------------------------------------------------
// // RESET
// // ----------------------------------------------------------------
// void resetEncoder(int num) {
//   if (num == 1) conteo1 = 0;
//   if (num == 2) conteo2 = 0;
//   if (num == 3) conteo3 = 0;
// }

// // ----------------------------------------------------------------
// // MENÚ
// // ----------------------------------------------------------------
// void printMenu() {
//   Serial.println("\n========================================");
//   Serial.println("  CALIBRACIÓN — OpenCM9.04");
//   Serial.println("========================================");
//   Serial.println("  ENCODERS:");
//   Serial.println("    1 = Hombro abducción  (600 PPR, D11/D12)");
//   Serial.println("    2 = Hombro flex/ext   (600 PPR, D13/D14)");
//   Serial.println("    3 = Codo flexión      (200 PPR, D15/D16)");
//   Serial.println("    z = Resetear encoder activo a 0°");
//   Serial.println("    i = Invertir dirección encoder activo");
//   Serial.println("----------------------------------------");
//   Serial.println("  POTENCIÓMETROS:");
//   Serial.println("    p = Ver todos los potenciómetros");
//   Serial.println("----------------------------------------");
//   Serial.println("    m = Volver a este menú");
//   Serial.println("========================================\n");
// }

// // ----------------------------------------------------------------
// // SETUP
// // ----------------------------------------------------------------
// void setup() {
//   Serial.begin(115200);
//   delay(300);

//   pinMode(ENC1_A, INPUT_PULLUP); pinMode(ENC1_B, INPUT_PULLUP);
//   pinMode(ENC2_A, INPUT_PULLUP); pinMode(ENC2_B, INPUT_PULLUP);
//   pinMode(ENC3_A, INPUT_PULLUP); pinMode(ENC3_B, INPUT_PULLUP);

//   attachInterrupt(ENC1_A, isr_enc1, CHANGE);
//   attachInterrupt(ENC2_A, isr_enc2, CHANGE);
//   attachInterrupt(ENC3_A, isr_enc3, CHANGE);

//   analogReadResolution(12);  // 12 bits → 0..4095

//   printMenu();
// }

// // ----------------------------------------------------------------
// // LOOP
// // ----------------------------------------------------------------
// void loop() {
//   static unsigned long ultimoPrint = 0;

//   // --- Comandos ---
//   if (Serial.available()) {
//     char cmd = Serial.read();

//     if (cmd == '1') {
//       modo = 'e'; encoderActivo = 1;
//       resetEncoder(1);
//       Serial.println("\n>> Encoder 1 — HOMBRO ABDUCCIÓN (600 PPR)");
//       Serial.println("   Levanta el brazo lateralmente.");
//       Serial.println("   z=reset | i=invertir | m=menú\n");
//     }
//     else if (cmd == '2') {
//       modo = 'e'; encoderActivo = 2;
//       resetEncoder(2);
//       Serial.println("\n>> Encoder 2 — HOMBRO FLEXIÓN/EXTENSIÓN (600 PPR)");
//       Serial.println("   Lleva el brazo hacia adelante o atrás.");
//       Serial.println("   z=reset | i=invertir | m=menú\n");
//     }
//     else if (cmd == '3') {
//       modo = 'e'; encoderActivo = 3;
//       resetEncoder(3);
//       Serial.println("\n>> Encoder 3 — CODO FLEXIÓN (200 PPR)");
//       Serial.println("   Dobla el codo.");
//       Serial.println("   z=reset | i=invertir | m=menú\n");
//     }
//     else if (cmd == 'z') {
//       if (modo == 'e' && encoderActivo > 0) {
//         resetEncoder(encoderActivo);
//         Serial.println(">> Reset a 0° — posición de referencia fijada.");
//       }
//     }
//     else if (cmd == 'i') {
//       if (modo == 'e') {
//         if (encoderActivo == 1) { dir1 *= -1; Serial.println(">> Encoder 1 invertido."); }
//         if (encoderActivo == 2) { dir2 *= -1; Serial.println(">> Encoder 2 invertido."); }
//         if (encoderActivo == 3) { dir3 *= -1; Serial.println(">> Encoder 3 invertido."); }
//       }
//     }
//     else if (cmd == 'p') {
//       modo = 'p';
//       encoderActivo = 0;
//       Serial.println("\n>> Modo POTENCIÓMETROS");
//       Serial.println("   Mueve cada dedo y observa el ángulo.");
//       Serial.println("   m=menú\n");
//     }
//     else if (cmd == 'm') {
//       modo = 'n';
//       encoderActivo = 0;
//       printMenu();
//     }
//   }

//   // --- Imprimir según modo ---
//   if (modo != 'n' && millis() - ultimoPrint >= INTERVALO_PRINT) {
//     ultimoPrint = millis();

//     // ENCODERS
//     if (modo == 'e' && encoderActivo > 0) {
//       float angulo = 0;
//       String nombre = "";
//       String ppr = "";

//       if (encoderActivo == 1) {
//         angulo = enc1AGrados(conteo1) * dir1;
//         nombre = "Hombro abduccion";
//         ppr    = "600PPR";
//       } else if (encoderActivo == 2) {
//         angulo = enc2AGrados(conteo2) * dir2;
//         nombre = "Hombro flex/ext ";
//         ppr    = "600PPR";
//       } else if (encoderActivo == 3) {
//         angulo = enc3AGrados(conteo3) * dir3;
//         nombre = "Codo flexion    ";
//         ppr    = "200PPR";
//       }

//       // Barra visual ±180°
//       int barLen = (int)(abs(angulo) / 180.0 * 20);
//       if (barLen > 20) barLen = 20;
//       String barra = "[";
//       for (int i = 0; i < 20; i++) barra += (i < barLen) ? "=" : " ";
//       barra += "]";

//       Serial.print("["); Serial.print(ppr); Serial.print("] ");
//       Serial.print(nombre); Serial.print(" : ");
//       if (angulo >= 0 && angulo < 100) Serial.print(" ");
//       if (angulo >= 0 && angulo < 10)  Serial.print(" ");
//       if (angulo >= 0)                  Serial.print(" ");
//       Serial.print(angulo, 1);
//       Serial.print(" deg   ");
//       Serial.println(barra);
//     }

//     // POTENCIÓMETROS
//     if (modo == 'p') {
//       float d1 = potAGrados(POT_1);
//       float d2 = potAGrados(POT_2);
//       float d3 = potAGrados(POT_3);
//       float d4 = potAGrados(POT_4);
//       float d5 = potAGrados(POT_5);

//       // Raw para calibración
//       int r1 = analogRead(POT_1);
//       int r2 = analogRead(POT_2);
//       int r3 = analogRead(POT_3);
//       int r4 = analogRead(POT_4);
//       int r5 = analogRead(POT_5);

//       Serial.println("-------- DEDOS --------------------------------");
//       Serial.print("Pulgar  (A0): "); Serial.print(d1, 1);
//       Serial.print(" deg  [raw:"); Serial.print(r1); Serial.println("]");

//       Serial.print("Indice  (A1): "); Serial.print(d2, 1);
//       Serial.print(" deg  [raw:"); Serial.print(r2); Serial.println("]");

//       Serial.print("Medio   (A2): "); Serial.print(d3, 1);
//       Serial.print(" deg  [raw:"); Serial.print(r3); Serial.println("]");

//       Serial.print("Anular  (A3): "); Serial.print(d4, 1);
//       Serial.print(" deg  [raw:"); Serial.print(r4); Serial.println("]");

//       Serial.print("Menique (A4): "); Serial.print(d5, 1);
//       Serial.print(" deg  [raw:"); Serial.print(r5); Serial.println("]");

//       Serial.println("-----------------------------------------------\n");
//     }
//   }
// }
/////////////////////////////////////////////////////////////////////////opencm
// const int pinA = 2;  // Canal A del encoder
// const int pinB = 3;  // Canal B del encoder

// int pulsesPerRevolution = 1200;  // MODIFCALO A TU GUSTO EL PULSO
// long counter = 0;
// float angle = 0.0;
// unsigned long startime;
// unsigned long currentmillis;
// int lastStateA;

// void setup() {
//   pinMode(pinA, INPUT_PULLUP);
//   pinMode(pinB, INPUT_PULLUP);

//   Serial.begin(9600);
//   delay(1000);
//   lastStateA = digitalRead(pinA);
//   startime = millis();
// }

// void loop() {
//   int currentStateA = digitalRead(pinA);

//   if (currentStateA != lastStateA) {
//     // Cambio en el canal A detectado
//     if (digitalRead(pinB) != currentStateA) {
//       counter++;  // dirección hacia adelante
//     } else {
//       counter--;  // dirección hacia atrás
//     }

//     angle = (360.0 * counter) / pulsesPerRevolution;
//     currentmillis = startime;
//     Serial.println(currentmillis);
//     //Serial.println("Pulsos: ");
//     //Serial.println(counter);
//     Serial.println(",");
//     Serial.println(angle, 2);
//   }
//   else
//   {
//      angle = (360.0 * counter) / pulsesPerRevolution;

//     //Serial.print("Pulsos: ");
//     //Serial.print(counter);
//     //Serial.print(" | Ángulo: ");
//     //Serial.print(currentmillis);
//     Serial.println(angle, 2);

//   }

//   lastStateA = currentStateA;
// }

/////////////////////////////arduino
// ================================================================
//  DISPOSITIVO LIDER — Arduino Uno
//  3 encoders (polling) + 2 potenciometros (dedos) + 4 motores
//  vibradores (retroalimentacion haptica)
//
//  Modos:
//    Calibracion (menu 1,2,3,z,i,p,m) -> igual que antes, para
//    verificar cada sensor antes de operar.
//    Operacion   (comando 'r')        -> transmite continuamente
//    los angulos hacia la Raspberry Pi/ROS y recibe los valores de
//    fuerza del sistema Seguidor para mover los motores.
// ================================================================
#include <string.h>
#include <stdlib.h>
#include <EEPROM.h>

// ----------------------------------------------------------------
// PINES ENCODERS (solo lectura digital, no requieren PWM)
// NOTA: se movieron respecto a la version anterior (2,3,4,5,6,7)
// para liberar los pines PWM 3, 5 y 6 y poder usarlos en los
// motores vibradores. Hay que recablear los encoders a estos
// pines antes de programar esta version.
// ----------------------------------------------------------------
#define ENC1_A  2
#define ENC1_B  4
#define ENC2_A  7
#define ENC2_B  8
#define ENC3_A  12
#define ENC3_B  13

// ----------------------------------------------------------------
// PINES POTENCIOMETROS (dedos)
// Solo 2 potenciometros: uno montado en el indice que capta el
// movimiento conjunto de pulgar + indice, y otro montado en el
// anular que capta el movimiento conjunto de medio + anular + menique.
// ----------------------------------------------------------------
#define POT_PULGAR_INDICE          A0    // montado en el indice
#define POT_MEDIO_ANULAR_MENIQUE   A1    // montado en el anular

// ----------------------------------------------------------------
// PINES MOTORES VIBRADORES (deben ser pines PWM del Uno: 3,5,6,9,10,11)
//
// ADVERTENCIA DE HARDWARE - leer antes de conectar los motores:
//   Van cableados directo al pin del Arduino, sin transistor. Se
//   midio el consumo real con fuente de banco: cada motor funciona
//   correctamente desde <1V hasta 5V y consume ~0.12A (120mA) a
//   plena tension. El Arduino Uno soporta max ~40mA por pin y max
//   ~200mA sumando TODOS los pines de E/S encendidos a la vez, y
//   el propio puerto USB del PC/Raspberry Pi suele entregar solo
//   ~500mA total para toda la placa. Con los 4 motores activos al
//   mismo tiempo y a maxima intensidad (~480mA) se sigue quedando
//   cerca de ese limite, asi que hay poco margen.
//   Recomendaciones minimas:
//     1) No actives los 4 motores a fuerza maxima al mismo tiempo
//        de forma sostenida.
//     2) MOTOR_PWM_MAX limita la intensidad maxima como salvaguarda
//        parcial; bajalo si el Arduino se reinicia solo o se calienta.
//     3) En cuanto puedas, migra a un transistor NPN/MOSFET (o un
//        ULN2003A que maneja hasta 7 canales en un solo chip) por
//        motor: es barato y elimina este riesgo por completo.
// ----------------------------------------------------------------
#define MOTOR_PULGAR        3
#define MOTOR_INDICE        5
#define MOTOR_MEDIO_ANULAR  6
#define MOTOR_MENIQUE       9

// Limite de seguridad en escala PWM 0-255. Se deja bajo a proposito: solo se
// busca una vibracion sutil que vaya de 0 a "un poco" conforme aumenta la
// fuerza recibida, sin que el motor llegue nunca a su maximo. Ademas, al
// limitar la intensidad de los 4 motores se reduce el consumo total muy por
// debajo del limite del puerto USB. Sube este valor solo si la vibracion se
// siente demasiado debil.
#define MOTOR_PWM_MAX  60

const int pinesMotor[4] = {MOTOR_PULGAR, MOTOR_INDICE, MOTOR_MEDIO_ANULAR, MOTOR_MENIQUE};

#define PPR_ENC1  600
#define PPR_ENC2  600
#define PPR_ENC3  200

#define PASOS_ENC1  (PPR_ENC1 * 4)
#define PASOS_ENC2  (PPR_ENC2 * 4)  
#define PASOS_ENC3  (PPR_ENC3 * 4)

#define GRADOS_ENC1  (360.0 / PASOS_ENC1)
#define GRADOS_ENC2  (360.0 / PASOS_ENC2)
#define GRADOS_ENC3  (360.0 / PASOS_ENC3)

#define POT_ADC_MIN   0
#define POT_ADC_MAX   1023
#define ANGULO_MIN    0.0
#define ANGULO_MAX    90.0

#define INTERVALO_PRINT  200   // ms, solo se usa en modo calibracion
#define INTERVALO_ENVIO  20    // ms, frecuencia de trama en modo operacion (~50Hz)

long conteo1 = 0;
long conteo2 = 0;     
long conteo3 = 0;

// Estado combinado de los 2 bits (canal A, canal B) de cada encoder,
// usado para decodificacion de cuadratura x4 (ver leerEncoders()).
int estadoEnc1 = 0;
int estadoEnc2 = 0;
int estadoEnc3 = 0;

// Tabla de transicion de cuadratura x4: indexada por
// (estado_anterior<<2 | estado_actual), donde cada estado es (A<<1|B).
// Devuelve +1, -1 o 0 (0 = transicion invalida/ruido, se ignora).
const int8_t TABLA_CUADRATURA[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

int dir1 = 1;
int dir2 = 1;
int dir3 = 1;

// Modo activo: 'n' menu, 'e' encoders, 'p' potenciometros, 'c' calibracion
// de potenciometros, 't' prueba de motores, 'r' operacion
char modo = 'n';
int encoderActivo = 0;
int potCalActivo = 0;   // 1 o 2 = potenciometro seleccionado en modo 'c'

// Valores crudos de los sensores hapticos recibidos por nodos de ROS
// (0-1023, misma escala que un ADC de 10 bits), uno por motor.
// orden: Pulgar, Indice, Medio+Anular, Menique
int fuerzaDedo[4] = {0, 0, 0, 0};

float enc1AGrados(long c) { return c * GRADOS_ENC1; }
float enc2AGrados(long c) { return c * GRADOS_ENC2; }
float enc3AGrados(long c) { return c * GRADOS_ENC3; }

// ----------------------------------------------------------------
// CALIBRACION DE POTENCIOMETROS (modo 'c')
// Cada potenciometro tiene su propio recorrido mecanico real (el
// mecanismo del dedo no gira los 0-1023 completos del ADC), asi que
// en vez de asumir POT_ADC_MIN/POT_ADC_MAX fijos, se guarda el raw
// real capturado en los dos extremos fisicos de cada dedo.
// Se persiste en EEPROM para no perder la calibracion al reiniciar.
// ----------------------------------------------------------------
struct CalibracionPot {
  int pot1Min;
  int pot1Max;
  int pot2Min;
  int pot2Max;
};
CalibracionPot calPot = {POT_ADC_MIN, POT_ADC_MAX, POT_ADC_MIN, POT_ADC_MAX};

#define EEPROM_MAGIC_ADDR  0
#define EEPROM_MAGIC_VAL   0xC7
#define EEPROM_DATA_ADDR   1

void cargarCalibracion() {
  if (EEPROM.read(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC_VAL) {
    EEPROM.get(EEPROM_DATA_ADDR, calPot);
  }
}

void guardarCalibracion() {
  EEPROM.put(EEPROM_DATA_ADDR, calPot);
  EEPROM.update(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VAL);
}

// Convierte una lectura cruda de ADC a grados (0-90) usando el rango
// calibrado real del potenciometro (adcMin = dedo extendido,
// adcMax = dedo cerrado). Acepta adcMin > adcMax si el cableado quedo
// invertido. El resultado siempre se recorta a [0, ANGULO_MAX].
float rawAGrados(int raw, int adcMin, int adcMax) {
  if (adcMax == adcMin) return ANGULO_MIN;   // sin calibrar todavia, evita division por cero
  int lo = min(adcMin, adcMax);
  int hi = max(adcMin, adcMax);
  raw = constrain(raw, lo, hi);
  float angulo = (float)(raw - adcMin) * (ANGULO_MAX - ANGULO_MIN) / (float)(adcMax - adcMin) + ANGULO_MIN;
  return constrain(angulo, ANGULO_MIN, ANGULO_MAX);
}


void resetEncoder(int num) {
  if (num == 1) conteo1 = 0;
  if (num == 2) conteo2 = 0;
  if (num == 3) conteo3 = 0;
}

void leerEncoders() {
  int a1 = digitalRead(ENC1_A);
  int b1 = digitalRead(ENC1_B);
  estadoEnc1 = ((estadoEnc1 << 2) | (a1 << 1) | b1) & 0x0F;
  conteo1 += TABLA_CUADRATURA[estadoEnc1];

  int a2 = digitalRead(ENC2_A);
  int b2 = digitalRead(ENC2_B);
  estadoEnc2 = ((estadoEnc2 << 2) | (a2 << 1) | b2) & 0x0F;
  conteo2 += TABLA_CUADRATURA[estadoEnc2];

  int a3 = digitalRead(ENC3_A);
  int b3 = digitalRead(ENC3_B);
  estadoEnc3 = ((estadoEnc3 << 2) | (a3 << 1) | b3) & 0x0F;
  conteo3 += TABLA_CUADRATURA[estadoEnc3];
}

// ----------------------------------------------------------------
// MOTORES HAPTICOS
// ----------------------------------------------------------------
void detenerMotores() {
  for (int i = 0; i < 4; i++) {
    fuerzaDedo[i] = 0;
    analogWrite(pinesMotor[i], 0);
  }
}

void aplicarFuerzaMotores() {
  for (int i = 0; i < 4; i++) {
    int crudo = constrain(fuerzaDedo[i], 0, 1023);
    int pwm = map(crudo, 0, 1023, 0, MOTOR_PWM_MAX);
    analogWrite(pinesMotor[i], pwm);
  }
}

// ----------------------------------------------------------------
// PRUEBA MANUAL DE MOTORES (modo 't')
// Rampa el motor indicado de 0 a MOTOR_PWM_MAX y de vuelta a 0, para
// confirmar que vibra y que esta en el pin correcto antes de operar.
// ----------------------------------------------------------------
void probarMotor(int idx, const __FlashStringHelper *nombre) {
  Serial.print(F(">> Probando motor: "));
  Serial.println(nombre);

  for (int pwm = 0; pwm <= MOTOR_PWM_MAX; pwm += 4) {
    analogWrite(pinesMotor[idx], pwm);
    delay(15);
  }
  delay(200);
  for (int pwm = MOTOR_PWM_MAX; pwm >= 0; pwm -= 4) {
    analogWrite(pinesMotor[idx], pwm);
    delay(15);
  }
  analogWrite(pinesMotor[idx], 0);
}

// ----------------------------------------------------------------
// TRAMA DE SALIDA (Lider -> Raspberry Pi / ROS)
// Formato: L,enc1,enc2,enc3,potPulgarIndice,potMedioAnularMenique\n
// Angulos de encoders y de dedos en grados, 2 decimales.
// ----------------------------------------------------------------
void enviarTrama() {
  float a1 = enc1AGrados(conteo1) * dir1;
  float a2 = enc2AGrados(conteo2) * dir2;
  float a3 = enc3AGrados(conteo3) * dir3;

  Serial.print(F("L,"));
  Serial.print(a1, 2); Serial.print(F(","));
  Serial.print(a2, 2); Serial.print(F(","));
  Serial.print(a3, 2); Serial.print(F(","));
  float p1 = rawAGrados(analogRead(POT_PULGAR_INDICE), calPot.pot1Min, calPot.pot1Max);
  float p2 = rawAGrados(analogRead(POT_MEDIO_ANULAR_MENIQUE), calPot.pot2Min, calPot.pot2Max);
  Serial.print(p1, 2); Serial.print(F(","));
  Serial.println(p2, 2);
}

// ----------------------------------------------------------------
// TRAMA DE ENTRADA (Raspberry Pi / ROS -> Lider)
// Formato esperado: F,f1,f2,f3,f4\n   (f1..f4 = 0-1023, lectura cruda
// de los sensores hapticos, misma escala que un ADC de 10 bits)
// orden: Pulgar, Indice, Medio+Anular, Menique
// Tambien acepta una linea "X" (o "m") para salir del modo
// operacion manualmente desde el Monitor Serial mientras pruebas.
// Lectura NO bloqueante, caracter por caracter, para no frenar
// la lectura de encoders mientras llega una trama.
// ----------------------------------------------------------------
#define BUFFER_RX_LEN 40
char bufferRx[BUFFER_RX_LEN];
uint8_t bufferRxIdx = 0;

void procesarTramaEntrante(char *trama) {
  if ((trama[0] == 'X' || trama[0] == 'm') && trama[1] == '\0') {
    modo = 'n';
    encoderActivo = 0;
    detenerMotores();
    printMenu();
    return;
  }

  if (trama[0] != 'F' || trama[1] != ',') return;   // trama desconocida, se descarta

  int valores[4];
  char *token = strtok(trama + 2, ",");
  for (int i = 0; i < 4; i++) {
    if (token == NULL) return;   // trama incompleta, se descarta completa
    valores[i] = constrain(atoi(token), 0, 1023);
    token = strtok(NULL, ",");
  }

  for (int i = 0; i < 4; i++) fuerzaDedo[i] = valores[i];
}

void leerTramaEntrante() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      bufferRx[bufferRxIdx] = '\0';
      procesarTramaEntrante(bufferRx);
      bufferRxIdx = 0;
    } else if (c != '\r') {
      if (bufferRxIdx < BUFFER_RX_LEN - 1) {
        bufferRx[bufferRxIdx++] = c;
      } else {
        bufferRxIdx = 0;   // trama demasiado larga, se descarta
      }
    }
  }
}

void printMenu() {
  Serial.println(F("\n========================================"));
  Serial.println(F("  DISPOSITIVO LIDER — Arduino Uno"));
  Serial.println(F("========================================"));
  Serial.println(F("  CALIBRACION:"));
  Serial.println(F("    1 = Hombro abduccion  (600PPR)"));
  Serial.println(F("    2 = Hombro flex/ext   (600PPR)"));
  Serial.println(F("    3 = Codo flexion      (200PPR)"));
  Serial.println(F("    z = Resetear a 0 grados"));
  Serial.println(F("    i = Invertir direccion"));
  Serial.println(F("    p = Ver todos los dedos"));
  Serial.println(F("    c = Calibrar rango real de los potenciometros"));
  Serial.println(F("----------------------------------------"));
  Serial.println(F("  PRUEBA:"));
  Serial.println(F("    t = Probar motores hapticos uno por uno"));
  Serial.println(F("----------------------------------------"));
  Serial.println(F("  OPERACION:"));
  Serial.println(F("    r = Iniciar modo operacion (envia angulos,"));
  Serial.println(F("        recibe fuerza y mueve los motores)"));
  Serial.println(F("----------------------------------------"));
  Serial.println(F("    m = Volver al menu / detener motores"));
  Serial.println(F("========================================\n"));
}

void setup() {
  Serial.begin(115200);
  cargarCalibracion();

  pinMode(ENC1_A, INPUT_PULLUP); pinMode(ENC1_B, INPUT_PULLUP);
  pinMode(ENC2_A, INPUT_PULLUP); pinMode(ENC2_B, INPUT_PULLUP);
  pinMode(ENC3_A, INPUT_PULLUP); pinMode(ENC3_B, INPUT_PULLUP);

  for (int i = 0; i < 4; i++) {
    pinMode(pinesMotor[i], OUTPUT);
    analogWrite(pinesMotor[i], 0);
  }

  estadoEnc1 = (digitalRead(ENC1_A) << 1) | digitalRead(ENC1_B);
  estadoEnc2 = (digitalRead(ENC2_A) << 1) | digitalRead(ENC2_B);
  estadoEnc3 = (digitalRead(ENC3_A) << 1) | digitalRead(ENC3_B);

  printMenu();
}

void loop() {
  static unsigned long ultimoPrint = 0;
  static unsigned long ultimoEnvio = 0;

  leerEncoders();

  // --- Modo operacion: trama de entrada/salida continua, sin
  //     procesar comandos de un solo caracter ---
  if (modo == 'r') {
    leerTramaEntrante();
    aplicarFuerzaMotores();

    if (millis() - ultimoEnvio >= INTERVALO_ENVIO) {
      ultimoEnvio = millis();
      enviarTrama();
    }
    return;
  }

  // --- Comandos de calibracion ---
  if (Serial.available()) {
    char cmd = Serial.read();

    // --- Sub-comandos del modo 'c' (calibracion de potenciometros) van
    //     primero para que '1'/'2' no disparen la seleccion de encoder ---
    if (modo == 'c' && cmd == '1') {
      potCalActivo = 1;
      Serial.println(F("\n>> Calibrando POT 1 — Pulgar+Indice (A0)"));
      Serial.println(F("   Mueve el dedo a sus dos extremos y observa el raw."));
      Serial.println(F("   e = capturar EXTENDIDO (minimo)"));
      Serial.println(F("   x = capturar CERRADO (maximo)\n"));
    }
    else if (modo == 'c' && cmd == '2') {
      potCalActivo = 2;
      Serial.println(F("\n>> Calibrando POT 2 — Medio+Anular+Menique (A1)"));
      Serial.println(F("   Mueve el dedo a sus dos extremos y observa el raw."));
      Serial.println(F("   e = capturar EXTENDIDO (minimo)"));
      Serial.println(F("   x = capturar CERRADO (maximo)\n"));
    }
    else if (modo == 'c' && cmd == 'e' && potCalActivo > 0) {
      int raw = analogRead(potCalActivo == 1 ? POT_PULGAR_INDICE : POT_MEDIO_ANULAR_MENIQUE);
      if (potCalActivo == 1) calPot.pot1Min = raw; else calPot.pot2Min = raw;
      guardarCalibracion();
      Serial.print(F(">> Extendido capturado, raw="));
      Serial.println(raw);
    }
    else if (modo == 'c' && cmd == 'x' && potCalActivo > 0) {
      int raw = analogRead(potCalActivo == 1 ? POT_PULGAR_INDICE : POT_MEDIO_ANULAR_MENIQUE);
      if (potCalActivo == 1) calPot.pot1Max = raw; else calPot.pot2Max = raw;
      guardarCalibracion();
      Serial.print(F(">> Cerrado capturado, raw="));
      Serial.println(raw);
    }
    else if (cmd == 'c') {
      modo = 'c';
      encoderActivo = 0;
      potCalActivo = 0;
      Serial.println(F("\n>> MODO CALIBRACION POTENCIOMETROS"));
      Serial.println(F("   1 = Pulgar+Indice (A0)"));
      Serial.println(F("   2 = Medio+Anular+Menique (A1)"));
      Serial.println(F("   m = Volver al menu\n"));
    }
    else if (cmd == '1') {
      modo = 'e'; encoderActivo = 1;
      resetEncoder(1);
      Serial.println(F("\n>> Encoder 1 — HOMBRO ABDUCCION (600PPR)"));
      Serial.println(F("   Levanta el brazo lateralmente."));
      Serial.println(F("   z=reset | i=invertir | m=menu\n"));
    }
    else if (cmd == '2') {
      modo = 'e'; encoderActivo = 2;
      resetEncoder(2);
      Serial.println(F("\n>> Encoder 2 — HOMBRO FLEX/EXT (600PPR)"));
      Serial.println(F("   Lleva el brazo adelante o atras."));
      Serial.println(F("   z=reset | i=invertir | m=menu\n"));
    }
    else if (cmd == '3') {
      modo = 'e'; encoderActivo = 3;
      resetEncoder(3);
      Serial.println(F("\n>> Encoder 3 — CODO FLEXION (200PPR)"));
      Serial.println(F("   Dobla el codo."));
      Serial.println(F("   z=reset | i=invertir | m=menu\n"));
    }
    else if (cmd == 'z') {
      if (modo == 'e' && encoderActivo > 0) {
        resetEncoder(encoderActivo);
        Serial.println(F(">> Reset a 0 grados."));
      }
    }
    else if (cmd == 'i') {
      if (modo == 'e') {
        if (encoderActivo == 1) { dir1 *= -1; Serial.println(F(">> Encoder 1 invertido.")); }
        if (encoderActivo == 2) { dir2 *= -1; Serial.println(F(">> Encoder 2 invertido.")); }
        if (encoderActivo == 3) { dir3 *= -1; Serial.println(F(">> Encoder 3 invertido.")); }
      }
    }
    else if (cmd == 'p') {
      modo = 'p';
      encoderActivo = 0;
      Serial.println(F("\n>> Modo POTENCIOMETROS"));
      Serial.println(F("   Mueve cada dedo y observa el angulo."));
      Serial.println(F("   m=menu\n"));
    }
    else if (cmd == 't') {
      modo = 't';
      encoderActivo = 0;
      detenerMotores();
      Serial.println(F("\n>> MODO PRUEBA DE MOTORES"));
      Serial.println(F("   a = Pulgar        (D3)"));
      Serial.println(F("   s = Indice        (D5)"));
      Serial.println(F("   d = Medio+Anular  (D6)"));
      Serial.println(F("   f = Menique       (D9)"));
      Serial.println(F("   g = Probar los 4 en secuencia"));
      Serial.println(F("   m = Volver al menu\n"));
    }
    else if (modo == 't' && cmd == 'a') {
      probarMotor(0, F("Pulgar (D3)"));
    }
    else if (modo == 't' && cmd == 's') {
      probarMotor(1, F("Indice (D5)"));
    }
    else if (modo == 't' && cmd == 'd') {
      probarMotor(2, F("Medio+Anular (D6)"));
    }
    else if (modo == 't' && cmd == 'f') {
      probarMotor(3, F("Menique (D9)"));
    }
    else if (modo == 't' && cmd == 'g') {
      probarMotor(0, F("Pulgar (D3)"));
      probarMotor(1, F("Indice (D5)"));
      probarMotor(2, F("Medio+Anular (D6)"));
      probarMotor(3, F("Menique (D9)"));
      Serial.println(F(">> Secuencia completa.\n"));
    }
    else if (cmd == 'r') {
      modo = 'r';
      encoderActivo = 0;
      bufferRxIdx = 0;
      Serial.println(F("\n>> MODO OPERACION iniciado."));
      Serial.print(F("   Enviando trama cada ")); Serial.print(INTERVALO_ENVIO); Serial.println(F(" ms."));
      Serial.println(F("   Trama enviada : L,enc1,enc2,enc3,potPulgarIndice,potMedioAnularMenique"));
      Serial.println(F("   Trama esperada: F,f1,f2,f3,f4   (0-1023, orden Pulgar/Indice/Medio+Anular/Menique)"));
      Serial.println(F("   Escribe X (o m) + Enter para volver al menu y detener motores.\n"));
    }
    else if (cmd == 'm') {
      modo = 'n';
      encoderActivo = 0;
      potCalActivo = 0;
      detenerMotores();
      printMenu();
    }
  }

  if (modo != 'n' && millis() - ultimoPrint >= INTERVALO_PRINT) {
    ultimoPrint = millis();

    if (modo == 'e' && encoderActivo > 0) {
      float angulo = 0;
      const __FlashStringHelper *nombre = F("");
      const __FlashStringHelper *ppr = F("");

      if (encoderActivo == 1) {
        angulo = enc1AGrados(conteo1) * dir1;
        nombre = F("Hombro abduccion");
        ppr    = F("600PPR");
      } else if (encoderActivo == 2) {
        angulo = enc2AGrados(conteo2) * dir2;
        nombre = F("Hombro flex/ext ");
        ppr    = F("600PPR");
      } else if (encoderActivo == 3) {
        angulo = enc3AGrados(conteo3) * dir3;
        nombre = F("Codo flexion    ");
        ppr    = F("200PPR");
      }

      int barLen = (int)(abs(angulo) / 180.0 * 20);
      if (barLen > 20) barLen = 20;

      Serial.print(F("[")); Serial.print(ppr); Serial.print(F("] "));
      Serial.print(nombre); Serial.print(F(" : "));
      if (angulo >= 0 && angulo < 100) Serial.print(F(" "));
      if (angulo >= 0 && angulo < 10)  Serial.print(F(" "));
      if (angulo >= 0)                  Serial.print(F(" "));
      Serial.print(angulo, 1);
      Serial.print(F(" deg   "));
      Serial.print(F("["));
      for (int i = 0; i < 20; i++) Serial.print(i < barLen ? '=' : ' ');
      Serial.println(F("]"));
    }

    if (modo == 'c' && potCalActivo > 0) {
      int pin = (potCalActivo == 1) ? POT_PULGAR_INDICE : POT_MEDIO_ANULAR_MENIQUE;
      int raw = analogRead(pin);
      int mn  = (potCalActivo == 1) ? calPot.pot1Min : calPot.pot2Min;
      int mx  = (potCalActivo == 1) ? calPot.pot1Max : calPot.pot2Max;
      Serial.print(F(">> raw: "));
      Serial.print(raw);
      Serial.print(F("   (guardado: min="));
      Serial.print(mn);
      Serial.print(F(" max="));
      Serial.print(mx);
      Serial.println(F(")"));
    }

    if (modo == 'p') {
      int pines[2] = {POT_PULGAR_INDICE, POT_MEDIO_ANULAR_MENIQUE};
      int calMin[2] = {calPot.pot1Min, calPot.pot2Min};
      int calMax[2] = {calPot.pot1Max, calPot.pot2Max};
      const __FlashStringHelper *nombres[2] = {
        F("Pulgar+Indice        (A0)"),
        F("Medio+Anular+Menique (A1)")
      };

      Serial.println(F("-------- DEDOS --------------------------------"));
      for (int i = 0; i < 2; i++) {
        int raw   = analogRead(pines[i]);
        float ang = rawAGrados(raw, calMin[i], calMax[i]);
        Serial.print(nombres[i]); Serial.print(F(": "));
        Serial.print(ang, 1);
        Serial.print(F(" deg  [raw:"));
        Serial.print(raw);
        Serial.println(F("]"));
      }
      Serial.println(F("----------------------------------------------\n"));
    }
  }
}