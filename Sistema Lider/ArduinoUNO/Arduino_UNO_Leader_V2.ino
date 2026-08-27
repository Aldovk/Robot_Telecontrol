// ================================================================
//  DISPOSITIVO LIDER — Arduino Uno (Adapted for ROS 2 Integration)
//  3 encoders + 2 potenciómetros + 4 motores vibradores (haptic)
// ================================================================
#include <string.h>
#include <stdlib.h>
#include <EEPROM.h>

// ----------------------------------------------------------------
// PINES ENCODERS
// ----------------------------------------------------------------
#define ENC1_A  2
#define ENC1_B  4
#define ENC2_A  7
#define ENC2_B  8
#define ENC3_A  12
#define ENC3_B  13

// ----------------------------------------------------------------
// PINES POTENCIÓMETROS (dedos)
// ----------------------------------------------------------------
#define POT_PULGAR_INDICE          A0    
#define POT_MEDIO_ANULAR_MENIQUE   A1    

// ----------------------------------------------------------------
// PINES MOTORES VIBRADORES (PWM)
// ----------------------------------------------------------------
#define MOTOR_PULGAR        3
#define MOTOR_INDICE        5
#define MOTOR_MEDIO_ANULAR  6
#define MOTOR_MENIQUE       9

#define MOTOR_PWM_MAX  60 // Limite de seguridad PWM
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

#define INTERVALO_PRINT  200   // ms, modo calibración
#define INTERVALO_ENVIO  20    // ms, frecuencia de trama en modo operación (~50Hz)

long conteo1 = 0;
long conteo2 = 0;     
long conteo3 = 0;

int estadoEnc1 = 0;
int estadoEnc2 = 0;
int estadoEnc3 = 0;

// Tabla de transición de cuadratura x4
const int8_t TABLA_CUADRATURA[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

int dir1 = 1;
int dir2 = 1;
int dir3 = 1;

char modo = 'n';
int encoderActivo = 0;
int potCalActivo = 0;   

int fuerzaDedo[4] = {0, 0, 0, 0};

float enc1AGrados(long c) { return c * GRADOS_ENC1; }
float enc2AGrados(long c) { return c * GRADOS_ENC2; }
float enc3AGrados(long c) { return c * GRADOS_ENC3; }

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

float rawAGrados(int raw, int adcMin, int adcMax) {
  if (adcMax == adcMin) return ANGULO_MIN;   
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

// Envía la trama en formato CSV compatible con la lógica del sistema
void enviarTrama() {
  float a1 = enc1AGrados(conteo1) * dir1;
  float a2 = enc2AGrados(conteo2) * dir2;
  float a3 = enc3AGrados(conteo3) * dir3;
  float p1 = rawAGrados(analogRead(POT_PULGAR_INDICE), calPot.pot1Min, calPot.pot1Max);
  float p2 = rawAGrados(analogRead(POT_MEDIO_ANULAR_MENIQUE), calPot.pot2Min, calPot.pot2Max);

  Serial.print(p1, 2); Serial.print(F(","));
  Serial.print(p2, 2); Serial.print(F(","));
  Serial.print(a1, 2); Serial.print(F(","));
  Serial.print(a2, 2); Serial.print(F(","));
  Serial.println(a3, 2);
}

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

  if (trama[0] != 'F' || trama[1] != ',') return;   

  int valores[4];
  char *token = strtok(trama + 2, ",");
  for (int i = 0; i < 4; i++) {
    if (token == NULL) return;   
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
        bufferRxIdx = 0;   
      }
    }
  }
}

void printMenu() {
  Serial.println(F("\n========================================"));
  Serial.println(F("  DISPOSITIVO LIDER — Arduino Uno (ROS)"));
  Serial.println(F("========================================"));
  Serial.println(F("  CALIBRACION:"));
  Serial.println(F("    1 = Hombro abduccion  (600PPR)"));
  Serial.println(F("    2 = Hombro flex/ext   (600PPR)"));
  Serial.println(F("    3 = Codo flexion      (200PPR)"));
  Serial.println(F("    z = Resetear a 0 grados"));
  Serial.println(F("    i = Invertir direccion"));
  Serial.println(F("    p = Ver todos los dedos"));
  Serial.println(F("    c = Calibrar rango real pots"));
  Serial.println(F("----------------------------------------"));
  Serial.println(F("  PRUEBA:"));
  Serial.println(F("    t = Probar motores hapticos"));
  Serial.println(F("----------------------------------------"));
  Serial.println(F("  OPERACION:"));
  Serial.println(F("    r = Iniciar modo operacion ROS"));
  Serial.println(F("----------------------------------------"));
  Serial.println(F("    m = Volver al menu"));
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

  // --- Modo Operación ROS ---
  if (modo == 'r') {
    leerTramaEntrante();
    aplicarFuerzaMotores();

    if (millis() - ultimoEnvio >= INTERVALO_ENVIO) {
      ultimoEnvio = millis();
      enviarTrama();
    }
    return;
  }

  // --- Comandos de Configuración y Calibración ---
  if (Serial.available()) {
    char cmd = Serial.read();

    if (modo == 'c' && cmd == '1') {
      potCalActivo = 1;
      Serial.println(F("\n>> Calibrando POT 1 (A0). Mueva a extremos y pulse e/x"));
    }
    else if (modo == 'c' && cmd == '2') {
      potCalActivo = 2;
      Serial.println(F("\n>> Calibrando POT 2 (A1). Mueva a extremos y pulse e/x"));
    }
    else if (modo == 'c' && cmd == 'e' && potCalActivo > 0) {
      int raw = analogRead(potCalActivo == 1 ? POT_PULGAR_INDICE : POT_MEDIO_ANULAR_MENIQUE);
      if (potCalActivo == 1) calPot.pot1Min = raw; else calPot.pot2Min = raw;
      guardarCalibracion();
      Serial.print(F(">> Extendido capturado, raw=")); Serial.println(raw);
    }
    else if (modo == 'c' && cmd == 'x' && potCalActivo > 0) {
      int raw = analogRead(potCalActivo == 1 ? POT_PULGAR_INDICE : POT_MEDIO_ANULAR_MENIQUE);
      if (potCalActivo == 1) calPot.pot1Max = raw; else calPot.pot2Max = raw;
      guardarCalibracion();
      Serial.print(F(">> Cerrado capturado, raw=")); Serial.println(raw);
    }
    else if (cmd == 'c') {
      modo = 'c'; encoderActivo = 0; potCalActivo = 0;
      Serial.println(F("\n>> MODO CALIBRACION POTENCIOMETROS (1 ó 2)"));
    }
    else if (cmd == '1') { modo = 'e'; encoderActivo = 1; resetEncoder(1); }
    else if (cmd == '2') { modo = 'e'; encoderActivo = 2; resetEncoder(2); }
    else if (cmd == '3') { modo = 'e'; encoderActivo = 3; resetEncoder(3); }
    else if (cmd == 'z') { if (modo == 'e' && encoderActivo > 0) resetEncoder(encoderActivo); }
    else if (cmd == 'i') {
      if (modo == 'e') {
        if (encoderActivo == 1) dir1 *= -1;
        if (encoderActivo == 2) dir2 *= -1;
        if (encoderActivo == 3) dir3 *= -1;
      }
    }
    else if (cmd == 'p') { modo = 'p'; encoderActivo = 0; }
    else if (cmd == 't') { modo = 't'; encoderActivo = 0; detenerMotores(); }
    else if (modo == 't' && cmd == 'a') probarMotor(0, F("Pulgar"));
    else if (modo == 't' && cmd == 's') probarMotor(1, F("Indice"));
    else if (modo == 't' && cmd == 'd') probarMotor(2, F("Medio+Anular"));
    else if (modo == 't' && cmd == 'f') probarMotor(3, F("Menique"));
    else if (cmd == 'r') {
      modo = 'r';
      encoderActivo = 0;
      bufferRxIdx = 0;
      Serial.println(F("\n>> MODO OPERACION ROS INICIADO."));
    }
    else if (cmd == 'm') {
      modo = 'n'; encoderActivo = 0; potCalActivo = 0;
      detenerMotores();
      printMenu();
    }
  }

  // --- Monitoreo visual de depuración ---
  if (modo != 'n' && millis() - ultimoPrint >= INTERVALO_PRINT) {
    ultimoPrint = millis();

    if (modo == 'e' && encoderActivo > 0) {
      float angulo = 0;
      if (encoderActivo == 1) angulo = enc1AGrados(conteo1) * dir1;
      else if (encoderActivo == 2) angulo = enc2AGrados(conteo2) * dir2;
      else if (encoderActivo == 3) angulo = enc3AGrados(conteo3) * dir3;
      Serial.print(F("Encoder ")); Serial.print(encoderActivo); Serial.print(F(": "));
      Serial.print(angulo, 1); Serial.println(F(" deg"));
    }
    if (modo == 'p') {
      int r1 = analogRead(POT_PULGAR_INDICE);
      int r2 = analogRead(POT_MEDIO_ANULAR_MENIQUE);
      Serial.print(F("Pot 1: ")); Serial.print(rawAGrados(r1, calPot.pot1Min, calPot.pot1Max));
      Serial.print(F(" deg | Pot 2: ")); Serial.println(rawAGrados(r2, calPot.pot2Min, calPot.pot2Max));
    }
  }
}