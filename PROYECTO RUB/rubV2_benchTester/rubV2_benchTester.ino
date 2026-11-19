// ==========================================================
// TEST BENCH - RubHaptic V2
// XIAO nRF52840 - Solo Serial, SIN BLE.
// ==========================================================
// Objetivo: probar cada subsistema de la tarjeta de forma aislada
// via comandos de texto por el Monitor Serial (115200 baudios, "Newline").
//
//
// Comandos disponibles (escribir y Enter):
//   help                -> muestra este menu
//   m <1-7> <0-127>     -> enciende motor N a la intensidad dada (RTP)
//   m <1-7> 0           -> apaga motor N
//   moff                -> apaga TODOS los motores
//   list                -> lista motores y sensores con su nombre/indice
//   s <1-6>             -> lee UNA vez el sensor N y muestra voltaje+presion
//   sall                -> lee UNA vez los 6 sensores y muestra la tabla
//   stream on           -> streaming continuo de los 6 sensores (~5 Hz)
//   stream off          -> detiene el streaming
//   batt                -> % bateria y voltaje (MAX17048)
//   power               -> estado PGOOD / CHG / ALRT
//   cal                 -> recalibra el offset de los 6 sensores (reposo)
// ==========================================================

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_DRV2605.h>
#include <Adafruit_MAX1704X.h>

#define PIN_I2C_SDA D4
#define PIN_I2C_SCL D5
#define CS_PIN      D7   

#define PIN_PGOOD   D18
#define PIN_CHG     D17
#define PIN_ALRT    D1

#define MUX_ADDR          0x70
#define MCP_EXPANDER_ADDR 0x20
#define MCP_IODIR 0x00
#define MCP_OLAT  0x0A

struct Vibrador {
  uint8_t id;
  uint8_t muxChannel;
  uint8_t mcpPin;
  const char* nombre;
};

Vibrador grupo[] = {
  {1, 6, 7, "IC1 - G (Punto G)"},
  {2, 5, 1, "IC2 - Flor UP"},
  {3, 4, 2, "IC3 - Flor CENTER"},
  {4, 3, 3, "IC4 - Flor LEFT"},
  {5, 2, 4, "IC5 - Flor RIGHT"},
  {6, 1, 5, "IC6 - Flor DOWN"},
  {7, 0, 6, "IC7 - CLIT"}
};
const int NUM_MOTORS = 7;

Adafruit_DRV2605 drv;

#define DEV_ADDR 0b01
#define REG_ADCDATA  0x00
#define REG_CONFIG0  0x01
#define REG_CONFIG1  0x02
#define REG_CONFIG2  0x03
#define REG_CONFIG3  0x04
#define REG_IRQ      0x05
#define REG_MUX      0x06
#define CMD_START 0x68
#define CMD_RESET 0x78
#define ADC_SCALE 8388608.0
#define VREF 3.3

const float VOLT_MIN = 0.15;
const float VOLT_MAX = 2.50;

const int NUM_SENSORS = 6;
uint8_t channels[NUM_SENSORS]   = {0x08, 0x18, 0x28, 0x38, 0x48, 0x68};
const char* sensorNames[NUM_SENSORS] = {"Right", "Down", "Left", "Clit", "Up", "Punta"};
long offset[NUM_SENSORS] = {0};

SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);

Adafruit_MAX17048 maxlipo;
bool fuelGaugeOK = false;

bool streaming = false;
unsigned long lastStream = 0;
const unsigned long streamInterval = 200; 

void sendFastCommand(uint8_t cmd) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(cmd);
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
}

void selectMux(uint8_t ch) {
  Wire.beginTransmission(MUX_ADDR); Wire.write(1 << ch); Wire.endTransmission();
}

void writeRegister(uint8_t reg, uint8_t value) {
  SPI.beginTransaction(spiSettings); digitalWrite(CS_PIN, LOW);
  SPI.transfer(((DEV_ADDR << 6) | (reg << 2)) | 0x02); SPI.transfer(value);
  digitalWrite(CS_PIN, HIGH); SPI.endTransaction();
}

uint8_t readRegister(uint8_t reg) {
  SPI.beginTransaction(spiSettings); digitalWrite(CS_PIN, LOW);
  SPI.transfer(((DEV_ADDR << 6) | (reg << 2)) | 0x03);
  uint8_t val = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH); SPI.endTransaction();
  return val;
}

int32_t readADC_raw() {
  SPI.beginTransaction(spiSettings); digitalWrite(CS_PIN, LOW);
  SPI.transfer(((DEV_ADDR << 6) | (REG_ADCDATA << 2)) | 0x03);
  uint8_t b1 = SPI.transfer(0x00); uint8_t b2 = SPI.transfer(0x00); uint8_t b3 = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH); SPI.endTransaction();
  int32_t value = ((int32_t)b1 << 16) | ((int32_t)b2 << 8) | b3;
  if (value & 0x800000) value |= 0xFF000000;
  return value;
}

void setEnablePin(uint8_t pin, bool state) {
  static uint8_t current_olat = 0x00;
  if (state) current_olat |= (1 << pin);
  else current_olat &= ~(1 << pin);

  Wire.beginTransmission(MCP_EXPANDER_ADDR);
  Wire.write(MCP_OLAT);
  Wire.write(current_olat);
  Wire.endTransmission();
}

void setAllHapticsEnable(bool state) {
  if (!state) {
    Wire.beginTransmission(MCP_EXPANDER_ADDR);
    Wire.write(MCP_OLAT);
    Wire.write(0x00);
    Wire.endTransmission();
  } else {
    for (int i = 0; i < NUM_MOTORS; i++) setEnablePin(grupo[i].mcpPin, true);
  }
}

void waitDataReady() {
  unsigned long startWait = millis();
  while ((readRegister(REG_IRQ) & 0x40) != 0 && (millis() - startWait < 20));
}

float readSensorVoltage(int idx, long* rawOut = nullptr) {
  writeRegister(REG_MUX, channels[idx]);
  waitDataReady();
  readADC_raw(); 

  waitDataReady();
  int32_t raw = readADC_raw();
  long corrected = raw - offset[idx];
  if (corrected < 0) corrected = 0;
  if (rawOut) *rawOut = raw;
  return (corrected * VREF) / ADC_SCALE;
}

float voltageToPressure(float v) {
  float p = (v - VOLT_MIN) / (VOLT_MAX - VOLT_MIN);
  if (p < 0) p = 0;
  if (p > 1) p = 1;
  return p;
}

void calibrateSensors() {
  Serial.println("[ADC] Calibrando offsets (reposo, no tocar los sensores)...");
  long totals[NUM_SENSORS] = {0};
  const int N = 20;
  for (int i = 0; i < N; i++) {
    for (int ch = 0; ch < NUM_SENSORS; ch++) {
      writeRegister(REG_MUX, channels[ch]);
      waitDataReady();
      readADC_raw(); 
      waitDataReady();
      totals[ch] += readADC_raw();
    }
  }
  for (int ch = 0; ch < NUM_SENSORS; ch++) {
    offset[ch] = totals[ch] / N;
    Serial.print("  "); Serial.print(sensorNames[ch]); Serial.print(" offset="); Serial.println(offset[ch]);
  }
  Serial.println("[ADC] Calibracion OK.");
}

void initADC() {
  sendFastCommand(CMD_RESET);
  delay(10);
  writeRegister(REG_CONFIG0, 0x23); 
  writeRegister(REG_CONFIG1, 0x0C); 
  writeRegister(REG_CONFIG2, 0x8B); 
  writeRegister(REG_CONFIG3, 0xC0); 
  writeRegister(REG_IRQ, 0x07);
  sendFastCommand(CMD_START);
}

void initHapticDrivers() {
  Serial.println("[HAPTIC] Inicializando 7 drivers DRV2605L...");
  for (int i = 0; i < NUM_MOTORS; i++) {
    selectMux(grupo[i].muxChannel);
    if (drv.begin()) {
      drv.setMode(DRV2605_MODE_REALTIME);
      drv.setRealtimeValue(0);
      Serial.print("  OK  "); Serial.println(grupo[i].nombre);
    } else {
      Serial.print("  FAIL "); Serial.println(grupo[i].nombre);
    }
  }
}

void setMotor(int idx1based, int intensity) {
  if (idx1based < 1 || idx1based > NUM_MOTORS) {
    Serial.println("Motor invalido. Usa 1-7 (ver 'list').");
    return;
  }
  if (intensity < 0) intensity = 0;
  if (intensity > 127) intensity = 127;
  int idx = idx1based - 1;
  selectMux(grupo[idx].muxChannel);
  drv.setRealtimeValue((uint8_t)intensity);
  Serial.print(grupo[idx].nombre);
  Serial.print(" -> intensidad ");
  Serial.println(intensity);
}

void allMotorsOff() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    selectMux(grupo[i].muxChannel);
    drv.setRealtimeValue(0);
  }
  Serial.println("Todos los motores apagados.");
}

void printPowerStatus() {
  bool pgood = (digitalRead(PIN_PGOOD) == LOW);
  bool chg   = (digitalRead(PIN_CHG)   == LOW);
  bool alrt  = (digitalRead(PIN_ALRT)  == LOW);

  Serial.println("----- Estado de energia -----");
  Serial.print("PGOOD (fuente conectada): "); Serial.println(pgood ? "SI" : "NO");
  Serial.print("CHG   (cargando):         "); Serial.println(chg ? "SI" : "NO");
  Serial.print("ALRT  (pin digital):      "); Serial.println(alrt ? "ACTIVO" : "inactivo");

  if (fuelGaugeOK && maxlipo.isActiveAlert()) {
    uint8_t flags = maxlipo.getAlertStatus();
    Serial.print("Alerta confirmada por I2C (0x"); Serial.print(flags, HEX); Serial.println(") -> limpiando...");
    maxlipo.clearAlertFlag(flags);
  } else if (alrt) {
    Serial.println("AVISO: ALRT en bajo pero sin confirmacion I2C -> revisar cableado.");
  }
}

void printBattery() {
  if (!fuelGaugeOK) {
    Serial.println("MAX17048 no detectado.");
    return;
  }
  Serial.print("Bateria: "); Serial.print(maxlipo.cellPercent(), 1);
  Serial.print(" %   |   Voltaje: "); Serial.print(maxlipo.cellVoltage(), 3);
  Serial.println(" V");
}

void printAllSensors() {
  Serial.println("----- Sensores FSR -----");
  for (int ch = 0; ch < NUM_SENSORS; ch++) {
    float v = readSensorVoltage(ch);
    float p = voltageToPressure(v);
    Serial.print(ch + 1); Serial.print(") ");
    Serial.print(sensorNames[ch]);
    Serial.print("\tV="); Serial.print(v, 3);
    Serial.print("\tpresion="); Serial.println(p, 3);
  }
}

void printOneSensor(int idx1based) {
  if (idx1based < 1 || idx1based > NUM_SENSORS) {
    Serial.println("Sensor invalido. Usa 1-6 (ver 'list').");
    return;
  }
  int idx = idx1based - 1;
  long raw;
  float v = readSensorVoltage(idx, &raw);
  float p = voltageToPressure(v);
  Serial.print(sensorNames[idx]);
  Serial.print("\traw="); Serial.print(raw);
  Serial.print("\tV="); Serial.print(v, 3);
  Serial.print("\tpresion="); Serial.println(p, 3);
}

void printList() {
  Serial.println("----- Motores (comando: m <n> <0-127>) -----");
  for (int i = 0; i < NUM_MOTORS; i++) {
    Serial.print(i + 1); Serial.print(") "); Serial.println(grupo[i].nombre);
  }
  Serial.println("----- Sensores (comando: s <n>) -----");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(i + 1); Serial.print(") "); Serial.println(sensorNames[i]);
  }
}

void printHelp() {
  Serial.println();
  Serial.println("========== TEST BENCH RubHaptic V2 ==========");
  Serial.println("m <1-7> <0-127>  - enciende motor N a intensidad dada");
  Serial.println("m <1-7> 0        - apaga motor N");
  Serial.println("moff             - apaga TODOS los motores");
  Serial.println("list             - lista motores y sensores");
  Serial.println("s <1-6>          - lee UNA vez el sensor N");
  Serial.println("sall             - lee UNA vez los 6 sensores");
  Serial.println("stream on/off    - streaming continuo de sensores (~5Hz)");
  Serial.println("batt             - % bateria y voltaje (MAX17048)");
  Serial.println("power            - estado PGOOD / CHG / ALRT");
  Serial.println("cal              - recalibra offsets de sensores (reposo)");
  Serial.println("help             - muestra este menu");
  Serial.println("==============================================");
  Serial.println();
}

void handleCommand(String line) {
  line.trim();
  if (line.length() == 0) return;

  int sp1 = line.indexOf(' ');
  String cmd = (sp1 == -1) ? line : line.substring(0, sp1);
  String rest = (sp1 == -1) ? "" : line.substring(sp1 + 1);
  cmd.toLowerCase();

  if (cmd == "help" || cmd == "?") {
    printHelp();
  } else if (cmd == "list") {
    printList();
  } else if (cmd == "m") {
    int sp2 = rest.indexOf(' ');
    if (sp2 == -1) { Serial.println("Uso: m <1-7> <0-127>"); return; }
    int idx = rest.substring(0, sp2).toInt();
    int val = rest.substring(sp2 + 1).toInt();
    setMotor(idx, val);
  } else if (cmd == "moff") {
    allMotorsOff();
  } else if (cmd == "s") {
    printOneSensor(rest.toInt());
  } else if (cmd == "sall") {
    printAllSensors();
  } else if (cmd == "stream") {
    rest.trim(); rest.toLowerCase();
    if (rest == "on") { streaming = true; Serial.println("Streaming ON."); }
    else if (rest == "off") { streaming = false; Serial.println("Streaming OFF."); }
    else Serial.println("Uso: stream on|off");
  } else if (cmd == "batt") {
    printBattery();
  } else if (cmd == "power") {
    printPowerStatus();
  } else if (cmd == "cal") {
    calibrateSensors();
  } else {
    Serial.print("Comando desconocido: "); Serial.println(cmd);
    Serial.println("Escribe 'help' para ver el menu.");
  }
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 30 && !Serial; i++) delay(100);

  Wire.begin();
  SPI.begin();
  pinMode(CS_PIN, OUTPUT); digitalWrite(CS_PIN, HIGH);

  pinMode(PIN_PGOOD, INPUT);
  pinMode(PIN_CHG, INPUT);
  pinMode(PIN_ALRT, INPUT);

  fuelGaugeOK = maxlipo.begin();
  if (!fuelGaugeOK) Serial.println("[POWER] MAX17048 no detectado.");

  Wire.beginTransmission(MUX_ADDR); Wire.write(0x00); Wire.endTransmission();
  Wire.beginTransmission(MCP_EXPANDER_ADDR); Wire.write(MCP_IODIR); Wire.write(0x01); Wire.endTransmission();

  setAllHapticsEnable(true);
  delay(300);
  initHapticDrivers();

  initADC();
  calibrateSensors();

  printHelp();
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    handleCommand(line);
  }

  if (streaming && millis() - lastStream >= streamInterval) {
    lastStream = millis();
    printAllSensors();
  }
}