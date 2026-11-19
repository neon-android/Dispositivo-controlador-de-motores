// #include <Wire.h>
// #include <SPI.h>
// #include <Adafruit_DRV2605.h>
// #include <Adafruit_MAX1704X.h>
// #include <bluefruit.h>
// #include <Adafruit_LittleFS.h>
// #include <InternalFileSystem.h>

// using namespace Adafruit_LittleFS_Namespace;

// #define FW_VERSION "2.3.0"

// #define ENABLE_DCDC 1

// #define ENABLE_DEBUG 1

// #if ENABLE_DEBUG
// #define DEBUG_BEGIN(baud) \
//   Serial.begin(baud); \
//   for (int i = 0; i < 30; i++) { delay(100); }
// #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
// #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
// #else
// #define DEBUG_BEGIN(baud)
// #define DEBUG_PRINT(...)
// #define DEBUG_PRINTLN(...)
// #endif

// #define DEBUG_SENSORS 1

// #define ENABLE_OTA_CMD 0

// #define PIN_I2C_SDA D4
// #define PIN_I2C_SCL D5
// #define CS_PIN D7

// #define PIN_PGOOD D18
// #define PIN_CHG D17
// #define PIN_ALRT D1

// #define MUX_ADDR 0x70
// #define MCP_EXPANDER_ADDR 0x20

// #define MCP_IODIR 0x00
// #define MCP_OLAT 0x0A

// unsigned long lastBatteryCheck = 0;
// const unsigned long batteryInterval = 15000;

// const uint8_t CUSTOM_SERVICE_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00 };
// const uint8_t CHAR_SYS_CTRL_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x01, 0x00, 0x00, 0x00 };
// const uint8_t CHAR_TELEMETRY_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x02, 0x00, 0x00, 0x00 };
// const uint8_t CHAR_HAPTIC_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x03, 0x00, 0x00, 0x00 };
// const uint8_t CHAR_MOTORS_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x04, 0x00, 0x00, 0x00 };
// const uint8_t CHAR_GAIN_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x05, 0x00, 0x00, 0x00 };
// const uint8_t CHAR_SENS_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x06, 0x00, 0x00, 0x00 };

// enum SystemState { BOOTING,
//                    DEEP_SLEEP,
//                    LOW_BATTERY_LOCK,
//                    IDLE_CONNECTED,
//                    ACTIVE_LOOP };
// volatile SystemState currentState = BOOTING;
// bool pendingStart = false;

// BLEDis bledis;
// BLEBas bleBattery;
// BLEService customService(CUSTOM_SERVICE_UUID);
// BLECharacteristic charSysCtrl(CHAR_SYS_CTRL_UUID);
// BLECharacteristic charTelemetry(CHAR_TELEMETRY_UUID);
// BLECharacteristic charHaptic(CHAR_HAPTIC_UUID);
// BLECharacteristic charMotors(CHAR_MOTORS_UUID);
// BLECharacteristic charGain(CHAR_GAIN_UUID);
// BLECharacteristic charSens(CHAR_SENS_UUID);

// struct Vibrador {
//   uint8_t id;
//   uint8_t muxChannel;
//   uint8_t mcpPin;
//   float currentIntensity;
//   uint8_t lastSentValue;
// };

// Vibrador grupo[] = {
//   { 1, 6, 7, 0, 0 },
//   { 2, 5, 1, 0, 0 },
//   { 3, 4, 2, 0, 0 },
//   { 4, 3, 3, 0, 0 },
//   { 5, 2, 4, 0, 0 },
//   { 6, 1, 5, 0, 0 },
//   { 7, 0, 6, 0, 0 }
// };

// Adafruit_DRV2605 drv;

// class MAX17048_NoReset : public Adafruit_MAX17048 {
// public:
//   bool beginNoReset(TwoWire* wire = &Wire) {
//     if (i2c_dev) {
//       delete i2c_dev;
//       delete status_reg;
//       i2c_dev = NULL;
//       status_reg = NULL;
//     }
//     i2c_dev = new Adafruit_I2CDevice(MAX17048_I2CADDR_DEFAULT, wire);
//     if (!i2c_dev->begin() || !isDeviceReady()) return false;
//     status_reg = new Adafruit_BusIO_Register(i2c_dev, MAX1704X_STATUS_REG);
//     writeMode16(0x0000);
//     sleep(false);
//     return true;
//   }

//   bool quickStart16() {
//     return writeMode16(0x4000);
//   }

// private:
//   bool writeMode16(uint16_t value) {
//     Adafruit_BusIO_Register mode(i2c_dev, MAX1704X_MODE_REG, 2, MSBFIRST);
//     return mode.write(value);
//   }
// };

// MAX17048_NoReset maxlipo;
// bool fuelGaugeOK = false;

// #define DEV_ADDR 0b01
// #define CMD_START 0x68
// #define REG_CONFIG0 0x01
// #define REG_ADCDATA 0x00
// #define REG_MUX 0x06
// #define REG_IRQ 0x05
// #define ADC_SCALE 8388608.0

// const int NUM_CHANNELS = 6;
// uint8_t channels[] = { 0x08, 0x18, 0x28, 0x38, 0x48, 0x68 };
// float pressure[NUM_CHANNELS] = { 0 };
// long offset[NUM_CHANNELS] = { 0 };

// #define VREF 3.3

// SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);

// int currentVariation = 3;
// const float THRESHOLD = 0.03;

// bool gGateActive = false;
// const float G_THRESH_ON = 0.035;
// const float G_THRESH_OFF = 0.025;

// #define CONFIG_FILE "/rubcfg.bin"
// #define CONFIG_VERSION 1

// struct DeviceConfig {
//   uint8_t version;
//   uint8_t motorMask;
//   uint8_t gainGeneral;
//   uint8_t gainClit;
//   uint8_t gainG;
//   uint8_t sensMin;
//   uint8_t sensMax;
// };

// const DeviceConfig DEFAULT_CONFIG = { CONFIG_VERSION, 0x7E, 50, 50, 50, 12, 193 };
// const float SENS_MIN_MARGIN = 0.20;
// const unsigned long CFG_SAVE_DELAY = 2000;

// DeviceConfig cfg = DEFAULT_CONFIG;
// DeviceConfig savedCfg = DEFAULT_CONFIG;
// bool cfgDirty = false;
// unsigned long cfgChangedAt = 0;

// float voltFromByte(uint8_t b) {
//   return b * VREF / 255.0;
// }

// #define REG_CONFIG0 0x01
// #define REG_CONFIG1 0x02
// #define REG_CONFIG2 0x03
// #define REG_CONFIG3 0x04
// #define REG_IRQ 0x05
// #define REG_MUX 0x06
// #define REG_ADCDATA 0x00

// #define CMD_START 0x68
// #define CMD_STOP 0x6C
// #define CMD_RESET 0x78
// #define CMD_FULL_SHUTDOWN 0x74

// void sendFastCommand(uint8_t cmd) {
//   SPI.beginTransaction(spiSettings);
//   digitalWrite(CS_PIN, LOW);
//   SPI.transfer(cmd);
//   digitalWrite(CS_PIN, HIGH);
//   SPI.endTransaction();
// }
// void feedWatchdog() {
//   NRF_WDT->RR[0] = WDT_RR_RR_Reload;
// }

// void selectMux(uint8_t ch) {
//   Wire.beginTransmission(MUX_ADDR);
//   Wire.write(1 << ch);
//   Wire.endTransmission();
// }

// void writeRegister(uint8_t reg, uint8_t value) {
//   SPI.beginTransaction(spiSettings);
//   digitalWrite(CS_PIN, LOW);
//   SPI.transfer(((DEV_ADDR << 6) | (reg << 2)) | 0x02);
//   SPI.transfer(value);
//   digitalWrite(CS_PIN, HIGH);
//   SPI.endTransaction();
// }

// uint8_t readRegister(uint8_t reg) {
//   SPI.beginTransaction(spiSettings);
//   digitalWrite(CS_PIN, LOW);
//   SPI.transfer(((DEV_ADDR << 6) | (reg << 2)) | 0x03);
//   uint8_t val = SPI.transfer(0x00);
//   digitalWrite(CS_PIN, HIGH);
//   SPI.endTransaction();
//   return val;
// }

// int32_t readADC_raw() {
//   SPI.beginTransaction(spiSettings);
//   digitalWrite(CS_PIN, LOW);
//   SPI.transfer(((DEV_ADDR << 6) | (REG_ADCDATA << 2)) | 0x03);
//   uint8_t b1 = SPI.transfer(0x00);
//   uint8_t b2 = SPI.transfer(0x00);
//   uint8_t b3 = SPI.transfer(0x00);
//   digitalWrite(CS_PIN, HIGH);
//   SPI.endTransaction();
//   int32_t value = ((int32_t)b1 << 16) | ((int32_t)b2 << 8) | b3;
//   if (value & 0x800000) value |= 0xFF000000;
//   return value;
// }

// void waitDataReady() {
//   unsigned long startWait = millis();
//   while ((readRegister(REG_IRQ) & 0x40) != 0 && (millis() - startWait < 20))
//     ;
// }

// int32_t readChannelRaw(uint8_t muxValue) {
//   writeRegister(REG_MUX, muxValue);
//   waitDataReady();
//   readADC_raw();
//   waitDataReady();
//   return readADC_raw();
// }

// void setEnablePin(uint8_t pin, bool state) {
//   static uint8_t current_olat = 0x00;
//   if (state) current_olat |= (1 << pin);
//   else current_olat &= ~(1 << pin);

//   Wire.beginTransmission(MCP_EXPANDER_ADDR);
//   Wire.write(MCP_OLAT);
//   Wire.write(current_olat);
//   Wire.endTransmission();
// }

// void setAllHapticsEnable(bool state) {
//   if (!state) {
//     Wire.beginTransmission(MCP_EXPANDER_ADDR);
//     Wire.write(MCP_OLAT);
//     Wire.write(0x00);
//     Wire.endTransmission();
//   } else {
//     for (int i = 0; i < 7; i++) {
//       setEnablePin(grupo[i].mcpPin, true);
//     }
//   }
// }

// void calibrateSensors() {
//   DEBUG_PRINTLN("[ADC] Calibrando...");
//   long totals[6] = { 0 };
//   for (int i = 0; i < 20; i++) {
//     feedWatchdog();
//     for (int ch = 0; ch < 6; ch++) {
//       totals[ch] += readChannelRaw(channels[ch]);
//     }
//   }
//   for (int ch = 0; ch < 6; ch++) {
//     offset[ch] = totals[ch] / 20;
//     DEBUG_PRINT(" Ch");
//     DEBUG_PRINT(ch);
//     DEBUG_PRINT(":");
//     DEBUG_PRINT(offset[ch]);
//   }
//   DEBUG_PRINTLN("\n[ADC] OK.");
// }

// void initHapticDrivers() {
//   for (int i = 0; i < 7; i++) {
//     selectMux(grupo[i].muxChannel);
//     if (drv.begin()) {
//       drv.setMode(DRV2605_MODE_REALTIME);
//       drv.setRealtimeValue(0);
//     }
//     grupo[i].lastSentValue = 0;
//     feedWatchdog();
//   }
// }

// bool configValid(const DeviceConfig& c) {
//   return c.version == CONFIG_VERSION && c.motorMask <= 0x7F && c.gainGeneral <= 100 && c.gainClit <= 100 && c.gainG <= 100 && voltFromByte(c.sensMax) > voltFromByte(c.sensMin) + SENS_MIN_MARGIN;
// }

// bool writeConfigFile(const DeviceConfig& c) {
//   InternalFS.remove(CONFIG_FILE);
//   File f(InternalFS);
//   if (!f.open(CONFIG_FILE, FILE_O_WRITE)) return false;
//   bool ok = f.write((const uint8_t*)&c, sizeof(c)) == sizeof(c);
//   f.close();
//   return ok;
// }

// void printConfig(const char* tag) {
//   DEBUG_PRINT(tag);
//   DEBUG_PRINT(" motores=0x");
//   DEBUG_PRINT(cfg.motorMask, HEX);
//   DEBUG_PRINT(" ganancia(gen/clit/G)=");
//   DEBUG_PRINT(cfg.gainGeneral);
//   DEBUG_PRINT("/");
//   DEBUG_PRINT(cfg.gainClit);
//   DEBUG_PRINT("/");
//   DEBUG_PRINT(cfg.gainG);
//   DEBUG_PRINT(" sens(min/max)=");
//   DEBUG_PRINT(cfg.sensMin);
//   DEBUG_PRINT("/");
//   DEBUG_PRINTLN(cfg.sensMax);
// }

// void loadConfig() {
//   cfg.motorMask |= 0x01;
//   markConfigChanged();
//   cfg = DEFAULT_CONFIG;
//   bool loaded = false;

//   InternalFS.begin();
//   File f(InternalFS);
//   if (f.open(CONFIG_FILE, FILE_O_READ)) {
//     DeviceConfig tmp;
//     if (f.read(&tmp, sizeof(tmp)) == sizeof(tmp) && configValid(tmp)) {
//       cfg = tmp;
//       loaded = true;
//     }
//     f.close();
//   }
//   feedWatchdog();

//   if (!loaded) writeConfigFile(cfg);
//   savedCfg = cfg;
//   feedWatchdog();

//   printConfig(loaded ? "[CFG] cargada de flash:" : "[CFG] defaults guardados:");
// }

// void markConfigChanged() {
//   cfgDirty = true;
//   cfgChangedAt = millis();
// }

// void saveConfigIfNeeded() {
//   if (!cfgDirty || millis() - cfgChangedAt < CFG_SAVE_DELAY) return;
//   cfgDirty = false;
//   DeviceConfig snapshot = cfg;
//   if (memcmp(&snapshot, &savedCfg, sizeof(snapshot)) == 0) return;
//   if (writeConfigFile(snapshot)) {
//     savedCfg = snapshot;
//     printConfig("[CFG] guardada en flash:");
//   } else {
//     DEBUG_PRINTLN("[CFG] ERROR al guardar en flash");
//   }
// }

// #define POWER_BIT_PGOOD 0
// #define POWER_BIT_CHG 1
// #define POWER_BIT_CRITICAL 2

// uint8_t readPowerStatus() {
//   bool pgood = (digitalRead(PIN_PGOOD) == LOW);
//   bool chg = (digitalRead(PIN_CHG) == LOW);

//   if (fuelGaugeOK && maxlipo.isActiveAlert()) {
//     uint8_t flags = maxlipo.getAlertStatus();
//     maxlipo.clearAlertFlag(flags);
//   }

//   uint8_t status = 0;
//   if (pgood) status |= (1 << POWER_BIT_PGOOD);
//   if (chg) status |= (1 << POWER_BIT_CHG);
//   return status;
// }

// const uint8_t SOC_LOCK = 10;
// const uint8_t SOC_UNLOCK = 20;
// const float V_LOCK = 3.65;
// const float V_CRITICAL = 3.45;
// const float V_UNLOCK = 3.80;
// const float V_UNLOCK_CHG = 3.90;
// const uint8_t SOC_MISMATCH_IDLE = 15;
// const uint8_t SOC_MISMATCH_CHG = 30;
// const float V_CHG_OFFSET = 0.15;
// const unsigned long PGOOD_SETTLE_MS = 120000;
// const unsigned long RESYNC_MIN_INTERVAL = 600000;
// const uint8_t POLICY_CONFIRM = 2;

// uint8_t lastSoc = 0;
// uint16_t lastVoltage_mV = 0;
// bool socValid = false;
// unsigned long lastResync = 0;
// bool resyncDone = false;
// unsigned long lastPgoodMs = 0;
// bool wasCharging = false;
// uint8_t lockVotes = 0;
// uint8_t unlockVotes = 0;

// uint8_t socFromVoltage(float v) {
//   static const float V_TBL[] = { 3.30, 3.50, 3.62, 3.70, 3.75, 3.79, 3.83, 3.88, 3.95, 4.02, 4.10, 4.20 };
//   static const uint8_t S_TBL[] = { 0, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
//   const int n = sizeof(S_TBL);
//   if (v <= V_TBL[0]) return 0;
//   for (int i = 1; i < n; i++) {
//     if (v < V_TBL[i]) {
//       return S_TBL[i - 1] + (uint8_t)((v - V_TBL[i - 1]) / (V_TBL[i] - V_TBL[i - 1]) * (S_TBL[i] - S_TBL[i - 1]));
//     }
//   }
//   return 100;
// }

// void enforcePowerPolicy() {
//   if (millis() - lastBatteryCheck < batteryInterval) return;
//   lastBatteryCheck = millis();

//   if (!fuelGaugeOK) fuelGaugeOK = maxlipo.beginNoReset();

//   float v = fuelGaugeOK ? maxlipo.cellVoltage() : NAN;
//   float pct = fuelGaugeOK ? maxlipo.cellPercent() : NAN;
//   bool gaugeValid = !isnan(v) && !isnan(pct) && v > 2.5 && v < 4.5;

//   uint8_t powerStatus = readPowerStatus();
//   bool charging = powerStatus & (1 << POWER_BIT_CHG);
//   uint8_t socV = 0;

//   if (gaugeValid) {
//     lastSoc = (uint8_t)constrain(pct, 0.0f, 100.0f);
//     lastVoltage_mV = (uint16_t)(v * 1000.0f + 0.5f);
//     socValid = true;
//     bool pgood = powerStatus & (1 << POWER_BIT_PGOOD);
//     if (pgood) lastPgoodMs = millis();
//     socV = socFromVoltage(charging ? v - V_CHG_OFFSET : v);

//     bool settled = !pgood && (millis() - lastPgoodMs >= PGOOD_SETTLE_MS);
//     uint8_t mismatch = charging ? SOC_MISMATCH_CHG : SOC_MISMATCH_IDLE;
//     bool canResync = (charging || settled) && (!resyncDone || millis() - lastResync >= RESYNC_MIN_INTERVAL);
//     bool chargeDone = wasCharging && !charging && pgood && lastSoc < 90;
//     wasCharging = charging;

//     if (currentState != ACTIVE_LOOP && (chargeDone || (canResync && abs((int)lastSoc - (int)socV) > mismatch))) {
//       maxlipo.quickStart16();
//       lastResync = millis();
//       resyncDone = true;
//       DEBUG_PRINT(chargeDone ? "[BAT] quickStart fin de carga: soc=" : "[BAT] quickStart: soc=");
//       DEBUG_PRINT(lastSoc);
//       DEBUG_PRINT(" socV=");
//       DEBUG_PRINTLN(socV);
//     }

//     bool wantLock = (lastSoc <= SOC_LOCK && v < V_LOCK) || v < V_CRITICAL;
//     bool wantUnlock = (lastSoc > SOC_UNLOCK && v >= V_LOCK) || v >= (charging ? V_UNLOCK_CHG : V_UNLOCK);

//     if (currentState != LOW_BATTERY_LOCK) {
//       unlockVotes = 0;
//       lockVotes = wantLock ? lockVotes + 1 : 0;
//       if (lockVotes >= POLICY_CONFIRM) {
//         lockVotes = 0;
//         currentState = LOW_BATTERY_LOCK;
//         pendingStart = false;
//         setAllHapticsEnable(false);
//       }
//     } else {
//       lockVotes = 0;
//       unlockVotes = wantUnlock ? unlockVotes + 1 : 0;
//       if (unlockVotes >= POLICY_CONFIRM) {
//         unlockVotes = 0;
//         currentState = Bluefruit.connected() ? IDLE_CONNECTED : DEEP_SLEEP;
//       }
//     }
//   }

//   if (currentState == LOW_BATTERY_LOCK || (socValid && lastSoc <= SOC_LOCK)) {
//     powerStatus |= (1 << POWER_BIT_CRITICAL);
//   }

//   if (gaugeValid) {
//     DEBUG_PRINT("[BAT] V=");
//     DEBUG_PRINT(v, 3);
//     DEBUG_PRINT(" soc=");
//     DEBUG_PRINT(lastSoc);
//     DEBUG_PRINT(" socV=");
//     DEBUG_PRINT(socV);
//     DEBUG_PRINT(" chg=");
//     DEBUG_PRINT(charging);
//     DEBUG_PRINT(" lockV=");
//     DEBUG_PRINT(lockVotes);
//     DEBUG_PRINT(" unlockV=");
//     DEBUG_PRINT(unlockVotes);
//     DEBUG_PRINT(" st=");
//     DEBUG_PRINTLN((int)currentState);
//   } else {
//     DEBUG_PRINTLN("[BAT] lectura del gauge invalida (I2C/NAN o no detectado)");
//   }

//   bleBattery.write(lastSoc);

//   uint8_t telemetryData[6];
//   telemetryData[0] = lastSoc;
//   telemetryData[1] = currentVariation;
//   telemetryData[2] = (uint8_t)currentState;
//   telemetryData[3] = powerStatus;
//   telemetryData[4] = lastVoltage_mV & 0xFF;
//   telemetryData[5] = (lastVoltage_mV >> 8) & 0xFF;

//   charTelemetry.notify(telemetryData, sizeof(telemetryData));
// }

// void connect_callback(uint16_t conn_handle) {
//   if (currentState != LOW_BATTERY_LOCK) currentState = IDLE_CONNECTED;
// }
// void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
//   setAllHapticsEnable(false);
//   if (currentState != LOW_BATTERY_LOCK) currentState = DEEP_SLEEP;
// }

// void sysCtrl_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
//   uint8_t cmd = data[0];
//   bool locked = (currentState == LOW_BATTERY_LOCK);
//   if (cmd == 0x01) {
//     if (!locked) pendingStart = true;
//   } else if (cmd == 0x02 || cmd == 0x00) {
//     setAllHapticsEnable(false);
//     if (!locked) currentState = IDLE_CONNECTED;
//   } else if (cmd == 0x03) {
//     setAllHapticsEnable(false);
//     if (!locked) currentState = DEEP_SLEEP;
//   } else if (cmd == 0x04) {
//     setAllHapticsEnable(false);
//     Bluefruit.disconnect(conn_hdl);
//   }
// #if ENABLE_OTA_CMD
//   else if (cmd == 0x05) {
//     Bluefruit.Advertising.stop();
//     NRF_POWER->GPREGRET = 0xA8;
//     NVIC_SystemReset();
//   }
// #endif
// }

// void haptic_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
//   if (len < 1) return;
//   if (data[0] >= 1 && data[0] <= 3) currentVariation = data[0];
// }

// void motors_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
//   if (len >= 1) {
//     cfg.motorMask = data[0] & 0x7F;
//     markConfigChanged();
//   }
//   chr->write8(cfg.motorMask);
// }

// void gain_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
//   if (len >= 3) {
//     cfg.gainGeneral = min(data[0], (uint8_t)100);
//     cfg.gainClit = min(data[1], (uint8_t)100);
//     cfg.gainG = min(data[2], (uint8_t)100);
//     markConfigChanged();
//   }
//   uint8_t applied[3] = { cfg.gainGeneral, cfg.gainClit, cfg.gainG };
//   chr->write(applied, sizeof(applied));
// }

// void sens_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
//   if (len >= 2 && voltFromByte(data[1]) > voltFromByte(data[0]) + SENS_MIN_MARGIN) {
//     cfg.sensMin = data[0];
//     cfg.sensMax = data[1];
//     markConfigChanged();
//   } else {
//     DEBUG_PRINTLN("[CFG] sensibilidad rechazada (VOLT_MAX debe superar a VOLT_MIN + 0.20V)");
//   }
//   uint8_t applied[2] = { cfg.sensMin, cfg.sensMax };
//   chr->write(applied, sizeof(applied));
// }

// void setupConfigCharacteristic(BLECharacteristic& chr, uint8_t len, BLECharacteristic::write_cb_t cb) {
//   chr.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
//   chr.setPermission(SECMODE_OPEN, SECMODE_OPEN);
//   chr.setFixedLen(len);
//   chr.setWriteCallback(cb);
//   chr.begin();
// }

// void setup() {
//   DEBUG_BEGIN(115200);
//   DEBUG_PRINT("[FW] version ");
//   DEBUG_PRINTLN(FW_VERSION);
//   NRF_WDT->CONFIG = 0x01;
//   NRF_WDT->CRV = 32768 * 5;
//   NRF_WDT->TASKS_START = 1;

//   Wire.begin();
//   SPI.begin();
//   pinMode(CS_PIN, OUTPUT);
//   digitalWrite(CS_PIN, HIGH);
//   sendFastCommand(CMD_FULL_SHUTDOWN);

//   pinMode(PIN_PGOOD, INPUT);
//   pinMode(PIN_CHG, INPUT);
//   pinMode(PIN_ALRT, INPUT);
//   fuelGaugeOK = maxlipo.beginNoReset();
//   bool gaugeFromPOR = false;
//   if (!fuelGaugeOK) DEBUG_PRINTLN("[POWER] MAX17048 no detectado");
//   else if (maxlipo.getAlertStatus() & MAX1704X_ALERTFLAG_RESET_INDICATOR) {
//     DEBUG_PRINTLN("[POWER] MAX17048 venia de power-on-reset");
//     maxlipo.clearAlertFlag(MAX1704X_ALERTFLAG_RESET_INDICATOR);
//     gaugeFromPOR = true;
//   }

//   Wire.beginTransmission(MUX_ADDR);
//   Wire.write(0x00);
//   Wire.endTransmission();
//   Wire.beginTransmission(MCP_EXPANDER_ADDR);
//   Wire.write(MCP_IODIR);
//   Wire.write(0x01);
//   Wire.endTransmission();
//   setAllHapticsEnable(false);

//   if (fuelGaugeOK && !gaugeFromPOR && digitalRead(PIN_PGOOD) == HIGH) {
//     float v = maxlipo.cellVoltage();
//     float pct = maxlipo.cellPercent();
//     if (!isnan(v) && !isnan(pct) && v > 2.5 && v < 4.5) {
//       uint8_t soc = (uint8_t)constrain(pct, 0.0f, 100.0f);
//       uint8_t socV = socFromVoltage(v);
//       if (abs((int)soc - (int)socV) > SOC_MISMATCH_IDLE) {
//         maxlipo.quickStart16();
//         lastResync = millis();
//         resyncDone = true;
//         DEBUG_PRINT("[BAT] quickStart boot: soc=");
//         DEBUG_PRINT(soc);
//         DEBUG_PRINT(" socV=");
//         DEBUG_PRINTLN(socV);
//       }
//     }
//   }

//   loadConfig();

//   Bluefruit.begin();
// #if ENABLE_DCDC
//   sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
// #endif
//   Bluefruit.autoConnLed(false);
//   uint8_t mac[6];
//   Bluefruit.getAddr(mac);
//   char bleName[9];
//   snprintf(bleName, sizeof(bleName), "RUB_%02X%02X", mac[1], mac[0]);
//   Bluefruit.setName(bleName);
//   DEBUG_PRINT("[BLE] nombre: ");
//   DEBUG_PRINTLN(bleName);
//   Bluefruit.Periph.setConnectCallback(connect_callback);
//   Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

//   bledis.setManufacturer("RUB");
//   bledis.setModel("RUB Haptic V2");
//   bledis.setFirmwareRev(FW_VERSION);
//   bledis.begin();

//   bleBattery.begin();
//   customService.begin();

//   charSysCtrl.setProperties(CHR_PROPS_WRITE | CHR_PROPS_NOTIFY);
//   charSysCtrl.setPermission(SECMODE_OPEN, SECMODE_OPEN);
//   charSysCtrl.setWriteCallback(sysCtrl_write_callback);
//   charSysCtrl.begin();

//   charTelemetry.setProperties(CHR_PROPS_NOTIFY);
//   charTelemetry.begin();

//   charHaptic.setProperties(CHR_PROPS_WRITE);
//   charHaptic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
//   charHaptic.setWriteCallback(haptic_write_callback);
//   charHaptic.begin();

//   setupConfigCharacteristic(charMotors, 1, motors_write_callback);
//   charMotors.write8(cfg.motorMask);

//   setupConfigCharacteristic(charGain, 3, gain_write_callback);
//   uint8_t gains[3] = { cfg.gainGeneral, cfg.gainClit, cfg.gainG };
//   charGain.write(gains, sizeof(gains));

//   setupConfigCharacteristic(charSens, 2, sens_write_callback);
//   uint8_t sens[2] = { cfg.sensMin, cfg.sensMax };
//   charSens.write(sens, sizeof(sens));

//   Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
//   Bluefruit.Advertising.addService(customService);
//   Bluefruit.Advertising.addName();
//   Bluefruit.Advertising.setInterval(32, 874);
//   Bluefruit.Advertising.setFastTimeout(30);
//   Bluefruit.Advertising.start(0);
//   currentState = DEEP_SLEEP;
// }

// void loop() {
//   feedWatchdog();
//   saveConfigIfNeeded();

//   static SystemState prevState = BOOTING;
//   SystemState st = currentState;
//   if (prevState == ACTIVE_LOOP && st != ACTIVE_LOOP) {
//     sendFastCommand(CMD_FULL_SHUTDOWN);
//     DEBUG_PRINTLN("[ADC] full shutdown");
//   }
//   prevState = st;

//   if (pendingStart && currentState != LOW_BATTERY_LOCK) {
//     pendingStart = false;
//     setAllHapticsEnable(true);
//     delay(300);
//     feedWatchdog();
//     initHapticDrivers();
//     feedWatchdog();

//     sendFastCommand(0x78);
//     delay(10);

//     writeRegister(REG_CONFIG0, 0x23);
//     writeRegister(REG_CONFIG1, 0x0C);
//     writeRegister(REG_CONFIG2, 0x8B);
//     writeRegister(REG_CONFIG3, 0xC0);
//     writeRegister(REG_IRQ, 0x07);

//     sendFastCommand(0x68);
//     calibrateSensors();
//     gGateActive = false;
//     currentState = ACTIVE_LOOP;
//   }

//   switch (currentState) {
//     case DEEP_SLEEP:
//       delay(500);
//       enforcePowerPolicy();
//       break;
//     case LOW_BATTERY_LOCK:
//       delay(500);
//       enforcePowerPolicy();
//       break;
//     case IDLE_CONNECTED:
//       enforcePowerPolicy();
//       delay(100);
//       break;
//     case ACTIVE_LOOP:
//       {
//         enforcePowerPolicy();
//         if (currentState != ACTIVE_LOOP) break;

//         float voltMin = voltFromByte(cfg.sensMin);
//         float voltMax = voltFromByte(cfg.sensMax);

//         for (int ch = 0; ch < 6; ch++) {
//           int32_t raw = readChannelRaw(channels[ch]);
//           long corrected = raw - offset[ch];
//           if (corrected < 0) corrected = 0;

//           float v = (corrected * VREF) / ADC_SCALE;
//           pressure[ch] = constrain((v - voltMin) / (voltMax - voltMin), 0.0, 1.0);
//         }

//         unsigned long currentMillis = millis();
//         float phaseRamp = (currentMillis % 1500) / 1500.0;
//         float wRampUp = phaseRamp;
//         float wRampDown = 1.0 - phaseRamp;
//         float wPulse = (sin((currentMillis % 666) / 666.0 * PI * 2 - PI / 2) + 1.0) / 2.0;

//         float t_G = 0, t_U = 0, t_L = 0, t_C = 0, t_R = 0, t_D = 0, t_B = 0;

//         if (currentVariation == 1) {
//           t_U = pressure[4] * wRampUp;    // Up
//           t_D = pressure[1] * wRampUp;    // Down
//           t_L = pressure[2] * wRampDown;  // Left
//           t_R = pressure[0] * wRampDown;  // Right
//           t_C = pressure[5] * wPulse;
//           t_B = pressure[3];
//         } else if (currentVariation == 2) {
//           t_U = pressure[4] * wPulse;
//           t_D = pressure[1] * wPulse;
//           t_L = pressure[2] * wPulse;
//           t_R = pressure[0] * wPulse;
//           t_C = pressure[5] * wPulse;
//           t_B = pressure[3] * wPulse;
//         } else {
//           t_U = pressure[4];
//           t_D = pressure[1];
//           t_L = pressure[2];
//           t_R = pressure[0];
//           t_C = pressure[5];
//           t_B = pressure[3];
//         }

//         float pArriba = pressure[4], pAbajo = pressure[1], pIzq = pressure[2], pDer = pressure[0];
//         bool allAbove = (pArriba > G_THRESH_ON && pAbajo > G_THRESH_ON && pIzq > G_THRESH_ON && pDer > G_THRESH_ON);
//         bool anyBelow = (pArriba < G_THRESH_OFF || pAbajo < G_THRESH_OFF || pIzq < G_THRESH_OFF || pDer < G_THRESH_OFF);
//         if (!gGateActive && allAbove) gGateActive = true;
//         if (gGateActive && anyBelow) gGateActive = false;
//         if (gGateActive) t_G = (pArriba + pAbajo + pIzq + pDer) / 4.0;

//         float gGeneral = cfg.gainGeneral / 100.0;
//         float gClit = cfg.gainClit / 100.0;
//         float gG = cfg.gainG / 100.0;

// #if ENABLE_DEBUG && DEBUG_SENSORS
//         static unsigned long lastSensorPrint = 0;
//         if (currentMillis - lastSensorPrint >= 250) {
//           lastSensorPrint = currentMillis;
//           DEBUG_PRINT("[SNS] Arr=");
//           DEBUG_PRINT(pArriba, 3);
//           DEBUG_PRINT(" Aba=");
//           DEBUG_PRINT(pAbajo, 3);
//           DEBUG_PRINT(" Izq=");
//           DEBUG_PRINT(pIzq, 3);
//           DEBUG_PRINT(" Der=");
//           DEBUG_PRINT(pDer, 3);
//           DEBUG_PRINT(" Clit=");
//           DEBUG_PRINT(pressure[3], 3);
//           DEBUG_PRINT(" Punta=");
//           DEBUG_PRINT(pressure[5], 3);
//           DEBUG_PRINT(" | gate=");
//           DEBUG_PRINT(gGateActive);
//           DEBUG_PRINT(" G_pwr=");
//           DEBUG_PRINT((int)(constrain(t_G * gG, 0.0f, 1.0f) * 127.0));
//           DEBUG_PRINT(" mask=0x");
//           DEBUG_PRINTLN(cfg.motorMask, HEX);
//         }
// #endif
//         // ESTO ES DEL PROGRAMA ORIGINAL
//         // auto updateM = [](int idx, float target) {
//         //   if (!(cfg.motorMask & (1 << idx))) target = 0;
//         //   int targetPower = (int)(constrain(target, 0.0f, 1.0f) * 127.0);
//         //   if (targetPower < 5) targetPower = 0;
//         //   if (targetPower == grupo[idx].lastSentValue) return;
//         //   grupo[idx].currentIntensity = (float)targetPower;
//         //   selectMux(grupo[idx].muxChannel);
//         //   drv.setRealtimeValue((uint8_t)targetPower);
//         //   grupo[idx].lastSentValue = (uint8_t)targetPower;
//         // };

//         //ESTOS SON MIS CAMBIOS
//         auto updateM = [](int idx, float target) {
//           if (!(cfg.motorMask & (1 << idx))) target = 0;

//           const float BOOST = 1.5;   // multiplicador (sube/baja a gusto)
//           const int MIN_POWER = 35;  // potencia mínima a la que el motor realmente vibra
//           float t = constrain(target * BOOST, 0.0f, 1.0f);

//           int targetPower = (t < 0.04f) ? 0 : (int)(MIN_POWER + t * (127 - MIN_POWER));
//           if (targetPower == grupo[idx].lastSentValue) return;

//           grupo[idx].currentIntensity = (float)targetPower;
//           selectMux(grupo[idx].muxChannel);
//           drv.setRealtimeValue((uint8_t)targetPower);
//           grupo[idx].lastSentValue = (uint8_t)targetPower;
//         };
//         updateM(3, 1.0);
//         updateM(0, t_G * gG);
//         updateM(1, t_U * gGeneral);
//         updateM(2, t_C * gGeneral);
//         updateM(3, t_L * gGeneral);
//         updateM(4, t_R * gGeneral);
//         updateM(5, t_D * gGeneral);
//         updateM(6, t_B * gClit);

//         delayMicroseconds(500);
//         break;
//       }
//   }
// }










///////////////////////////code claude



#include <Wire.h>
#include <SPI.h>
#include <Adafruit_DRV2605.h>
#include <Adafruit_MAX1704X.h>
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

using namespace Adafruit_LittleFS_Namespace;

#define FW_VERSION "2.3.0"

#define ENABLE_DCDC 1

#define ENABLE_DEBUG 1

#if ENABLE_DEBUG
#define DEBUG_BEGIN(baud) \
  Serial.begin(baud); \
  for (int i = 0; i < 30; i++) { delay(100); }
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_BEGIN(baud)
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

#define DEBUG_SENSORS 1

#define ENABLE_OTA_CMD 0

#define PIN_I2C_SDA D4
#define PIN_I2C_SCL D5
#define CS_PIN D7

#define PIN_PGOOD D18
#define PIN_CHG D17
#define PIN_ALRT D1

#define MUX_ADDR 0x70
#define MCP_EXPANDER_ADDR 0x20

#define MCP_IODIR 0x00
#define MCP_OLAT 0x0A

unsigned long lastBatteryCheck = 0;
const unsigned long batteryInterval = 15000;

const uint8_t CUSTOM_SERVICE_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00 };
const uint8_t CHAR_SYS_CTRL_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x01, 0x00, 0x00, 0x00 };
const uint8_t CHAR_TELEMETRY_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x02, 0x00, 0x00, 0x00 };
const uint8_t CHAR_HAPTIC_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x03, 0x00, 0x00, 0x00 };
const uint8_t CHAR_MOTORS_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x04, 0x00, 0x00, 0x00 };
const uint8_t CHAR_GAIN_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x05, 0x00, 0x00, 0x00 };
const uint8_t CHAR_SENS_UUID[] = { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x06, 0x00, 0x00, 0x00 };

enum SystemState { BOOTING,
                   DEEP_SLEEP,
                   LOW_BATTERY_LOCK,
                   IDLE_CONNECTED,
                   ACTIVE_LOOP };
volatile SystemState currentState = BOOTING;
bool pendingStart = false;

BLEDis bledis;
BLEBas bleBattery;
BLEService customService(CUSTOM_SERVICE_UUID);
BLECharacteristic charSysCtrl(CHAR_SYS_CTRL_UUID);
BLECharacteristic charTelemetry(CHAR_TELEMETRY_UUID);
BLECharacteristic charHaptic(CHAR_HAPTIC_UUID);
BLECharacteristic charMotors(CHAR_MOTORS_UUID);
BLECharacteristic charGain(CHAR_GAIN_UUID);
BLECharacteristic charSens(CHAR_SENS_UUID);

struct Vibrador {
  uint8_t id;
  uint8_t muxChannel;
  uint8_t mcpPin;
  float currentIntensity;
  uint8_t lastSentValue;
};

Vibrador grupo[] = {
  { 1, 6, 7, 0, 0 },
  { 2, 5, 1, 0, 0 },
  { 3, 4, 2, 0, 0 },
  { 4, 3, 3, 0, 0 },
  { 5, 2, 4, 0, 0 },
  { 6, 1, 5, 0, 0 },
  { 7, 0, 6, 0, 0 }
};

Adafruit_DRV2605 drv;

class MAX17048_NoReset : public Adafruit_MAX17048 {
public:
  bool beginNoReset(TwoWire* wire = &Wire) {
    if (i2c_dev) {
      delete i2c_dev;
      delete status_reg;
      i2c_dev = NULL;
      status_reg = NULL;
    }
    i2c_dev = new Adafruit_I2CDevice(MAX17048_I2CADDR_DEFAULT, wire);
    if (!i2c_dev->begin() || !isDeviceReady()) return false;
    status_reg = new Adafruit_BusIO_Register(i2c_dev, MAX1704X_STATUS_REG);
    writeMode16(0x0000);
    sleep(false);
    return true;
  }

  bool quickStart16() {
    return writeMode16(0x4000);
  }

private:
  bool writeMode16(uint16_t value) {
    Adafruit_BusIO_Register mode(i2c_dev, MAX1704X_MODE_REG, 2, MSBFIRST);
    return mode.write(value);
  }
};

MAX17048_NoReset maxlipo;
bool fuelGaugeOK = false;

#define DEV_ADDR 0b01
#define CMD_START 0x68
#define REG_CONFIG0 0x01
#define REG_ADCDATA 0x00
#define REG_MUX 0x06
#define REG_IRQ 0x05
#define ADC_SCALE 8388608.0

const int NUM_CHANNELS = 6;
uint8_t channels[] = { 0x08, 0x18, 0x28, 0x38, 0x48, 0x68 };
float pressure[NUM_CHANNELS] = { 0 };
long offset[NUM_CHANNELS] = { 0 };

#define VREF 3.3

SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);

int currentVariation = 3;
const float THRESHOLD = 0.03;

bool gGateActive = false;
const float G_THRESH_ON = 0.035;
const float G_THRESH_OFF = 0.025;

#define CONFIG_FILE "/rubcfg.bin"
#define CONFIG_VERSION 1

struct DeviceConfig {
  uint8_t version;
  uint8_t motorMask;
  uint8_t gainGeneral;
  uint8_t gainClit;
  uint8_t gainG;
  uint8_t sensMin;
  uint8_t sensMax;
};

const DeviceConfig DEFAULT_CONFIG = { CONFIG_VERSION, 0x7F, 50, 50, 50, 12, 193 };
const float SENS_MIN_MARGIN = 0.20;
const unsigned long CFG_SAVE_DELAY = 2000;

DeviceConfig cfg = DEFAULT_CONFIG;
DeviceConfig savedCfg = DEFAULT_CONFIG;
bool cfgDirty = false;
unsigned long cfgChangedAt = 0;

float voltFromByte(uint8_t b) {
  return b * VREF / 255.0;
}

#define REG_CONFIG0 0x01
#define REG_CONFIG1 0x02
#define REG_CONFIG2 0x03
#define REG_CONFIG3 0x04
#define REG_IRQ 0x05
#define REG_MUX 0x06
#define REG_ADCDATA 0x00

#define CMD_START 0x68
#define CMD_STOP 0x6C
#define CMD_RESET 0x78
#define CMD_FULL_SHUTDOWN 0x74

void sendFastCommand(uint8_t cmd) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(cmd);
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
}
void feedWatchdog() {
  NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}

void selectMux(uint8_t ch) {
  Wire.beginTransmission(MUX_ADDR);
  Wire.write(1 << ch);
  Wire.endTransmission();
}

void writeRegister(uint8_t reg, uint8_t value) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(((DEV_ADDR << 6) | (reg << 2)) | 0x02);
  SPI.transfer(value);
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
}

uint8_t readRegister(uint8_t reg) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(((DEV_ADDR << 6) | (reg << 2)) | 0x03);
  uint8_t val = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
  return val;
}

int32_t readADC_raw() {
  SPI.beginTransaction(spiSettings);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(((DEV_ADDR << 6) | (REG_ADCDATA << 2)) | 0x03);
  uint8_t b1 = SPI.transfer(0x00);
  uint8_t b2 = SPI.transfer(0x00);
  uint8_t b3 = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
  int32_t value = ((int32_t)b1 << 16) | ((int32_t)b2 << 8) | b3;
  if (value & 0x800000) value |= 0xFF000000;
  return value;
}

void waitDataReady() {
  unsigned long startWait = millis();
  while ((readRegister(REG_IRQ) & 0x40) != 0 && (millis() - startWait < 20))
    ;
}

int32_t readChannelRaw(uint8_t muxValue) {
  writeRegister(REG_MUX, muxValue);
  waitDataReady();
  readADC_raw();
  waitDataReady();
  return readADC_raw();
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
    for (int i = 0; i < 7; i++) {
      setEnablePin(grupo[i].mcpPin, true);
    }
  }
}

void calibrateSensors() {
  DEBUG_PRINTLN("[ADC] Calibrando...");
  long totals[6] = { 0 };
  for (int i = 0; i < 20; i++) {
    feedWatchdog();
    for (int ch = 0; ch < 6; ch++) {
      totals[ch] += readChannelRaw(channels[ch]);
    }
  }
  for (int ch = 0; ch < 6; ch++) {
    offset[ch] = totals[ch] / 20;
    DEBUG_PRINT(" Ch");
    DEBUG_PRINT(ch);
    DEBUG_PRINT(":");
    DEBUG_PRINT(offset[ch]);
  }
  DEBUG_PRINTLN("\n[ADC] OK.");
}

void initHapticDrivers() {
  for (int i = 0; i < 7; i++) {
    selectMux(grupo[i].muxChannel);
    if (drv.begin()) {
      drv.setMode(DRV2605_MODE_REALTIME);
      drv.setRealtimeValue(0);
    } else {
      DEBUG_PRINT("[HAP] DRV2605 FALLA en mux ch "); DEBUG_PRINTLN(grupo[i].muxChannel);
    }
    grupo[i].lastSentValue = 0;
    feedWatchdog();
  }
}

bool configValid(const DeviceConfig& c) {
  return c.version == CONFIG_VERSION && c.motorMask <= 0x7F && c.gainGeneral <= 100 && c.gainClit <= 100 && c.gainG <= 100 && voltFromByte(c.sensMax) > voltFromByte(c.sensMin) + SENS_MIN_MARGIN;
}

bool writeConfigFile(const DeviceConfig& c) {
  InternalFS.remove(CONFIG_FILE);
  File f(InternalFS);
  if (!f.open(CONFIG_FILE, FILE_O_WRITE)) return false;
  bool ok = f.write((const uint8_t*)&c, sizeof(c)) == sizeof(c);
  f.close();
  return ok;
}

void printConfig(const char* tag) {
  DEBUG_PRINT(tag);
  DEBUG_PRINT(" motores=0x");
  DEBUG_PRINT(cfg.motorMask, HEX);
  DEBUG_PRINT(" ganancia(gen/clit/G)=");
  DEBUG_PRINT(cfg.gainGeneral);
  DEBUG_PRINT("/");
  DEBUG_PRINT(cfg.gainClit);
  DEBUG_PRINT("/");
  DEBUG_PRINT(cfg.gainG);
  DEBUG_PRINT(" sens(min/max)=");
  DEBUG_PRINT(cfg.sensMin);
  DEBUG_PRINT("/");
  DEBUG_PRINTLN(cfg.sensMax);
}

void loadConfig() {
  cfg = DEFAULT_CONFIG;
  bool loaded = false;

  InternalFS.begin();
  File f(InternalFS);
  if (f.open(CONFIG_FILE, FILE_O_READ)) {
    DeviceConfig tmp;
    if (f.read(&tmp, sizeof(tmp)) == sizeof(tmp) && configValid(tmp)) {
      cfg = tmp;
      loaded = true;
    }
    f.close();
  }
  feedWatchdog();

  if (!loaded) writeConfigFile(cfg);
  savedCfg = cfg;
  feedWatchdog();

  printConfig(loaded ? "[CFG] cargada de flash:" : "[CFG] defaults guardados:");
}

void markConfigChanged() {
  cfgDirty = true;
  cfgChangedAt = millis();
}

void saveConfigIfNeeded() {
  if (!cfgDirty || millis() - cfgChangedAt < CFG_SAVE_DELAY) return;
  cfgDirty = false;
  DeviceConfig snapshot = cfg;
  if (memcmp(&snapshot, &savedCfg, sizeof(snapshot)) == 0) return;
  if (writeConfigFile(snapshot)) {
    savedCfg = snapshot;
    printConfig("[CFG] guardada en flash:");
  } else {
    DEBUG_PRINTLN("[CFG] ERROR al guardar en flash");
  }
}

#define POWER_BIT_PGOOD 0
#define POWER_BIT_CHG 1
#define POWER_BIT_CRITICAL 2

uint8_t readPowerStatus() {
  bool pgood = (digitalRead(PIN_PGOOD) == LOW);
  bool chg = (digitalRead(PIN_CHG) == LOW);

  if (fuelGaugeOK && maxlipo.isActiveAlert()) {
    uint8_t flags = maxlipo.getAlertStatus();
    maxlipo.clearAlertFlag(flags);
  }

  uint8_t status = 0;
  if (pgood) status |= (1 << POWER_BIT_PGOOD);
  if (chg) status |= (1 << POWER_BIT_CHG);
  return status;
}

const uint8_t SOC_LOCK = 10;
const uint8_t SOC_UNLOCK = 20;
const float V_LOCK = 3.65;
const float V_CRITICAL = 3.45;
const float V_UNLOCK = 3.80;
const float V_UNLOCK_CHG = 3.90;
const uint8_t SOC_MISMATCH_IDLE = 15;
const uint8_t SOC_MISMATCH_CHG = 30;
const float V_CHG_OFFSET = 0.15;
const unsigned long PGOOD_SETTLE_MS = 120000;
const unsigned long RESYNC_MIN_INTERVAL = 600000;
const uint8_t POLICY_CONFIRM = 2;

uint8_t lastSoc = 0;
uint16_t lastVoltage_mV = 0;
bool socValid = false;
unsigned long lastResync = 0;
bool resyncDone = false;
unsigned long lastPgoodMs = 0;
bool wasCharging = false;
uint8_t lockVotes = 0;
uint8_t unlockVotes = 0;

uint8_t socFromVoltage(float v) {
  static const float V_TBL[] = { 3.30, 3.50, 3.62, 3.70, 3.75, 3.79, 3.83, 3.88, 3.95, 4.02, 4.10, 4.20 };
  static const uint8_t S_TBL[] = { 0, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
  const int n = sizeof(S_TBL);
  if (v <= V_TBL[0]) return 0;
  for (int i = 1; i < n; i++) {
    if (v < V_TBL[i]) {
      return S_TBL[i - 1] + (uint8_t)((v - V_TBL[i - 1]) / (V_TBL[i] - V_TBL[i - 1]) * (S_TBL[i] - S_TBL[i - 1]));
    }
  }
  return 100;
}

void enforcePowerPolicy() {
  if (millis() - lastBatteryCheck < batteryInterval) return;
  lastBatteryCheck = millis();

  if (!fuelGaugeOK) fuelGaugeOK = maxlipo.beginNoReset();

  float v = fuelGaugeOK ? maxlipo.cellVoltage() : NAN;
  float pct = fuelGaugeOK ? maxlipo.cellPercent() : NAN;
  bool gaugeValid = !isnan(v) && !isnan(pct) && v > 2.5 && v < 4.5;

  uint8_t powerStatus = readPowerStatus();
  bool charging = powerStatus & (1 << POWER_BIT_CHG);
  uint8_t socV = 0;

  if (gaugeValid) {
    lastSoc = (uint8_t)constrain(pct, 0.0f, 100.0f);
    lastVoltage_mV = (uint16_t)(v * 1000.0f + 0.5f);
    socValid = true;
    bool pgood = powerStatus & (1 << POWER_BIT_PGOOD);
    if (pgood) lastPgoodMs = millis();
    socV = socFromVoltage(charging ? v - V_CHG_OFFSET : v);

    bool settled = !pgood && (millis() - lastPgoodMs >= PGOOD_SETTLE_MS);
    uint8_t mismatch = charging ? SOC_MISMATCH_CHG : SOC_MISMATCH_IDLE;
    bool canResync = (charging || settled) && (!resyncDone || millis() - lastResync >= RESYNC_MIN_INTERVAL);
    bool chargeDone = wasCharging && !charging && pgood && lastSoc < 90;
    wasCharging = charging;

    if (currentState != ACTIVE_LOOP && (chargeDone || (canResync && abs((int)lastSoc - (int)socV) > mismatch))) {
      maxlipo.quickStart16();
      lastResync = millis();
      resyncDone = true;
      DEBUG_PRINT(chargeDone ? "[BAT] quickStart fin de carga: soc=" : "[BAT] quickStart: soc=");
      DEBUG_PRINT(lastSoc);
      DEBUG_PRINT(" socV=");
      DEBUG_PRINTLN(socV);
    }

    bool wantLock = (lastSoc <= SOC_LOCK && v < V_LOCK) || v < V_CRITICAL;
    bool wantUnlock = (lastSoc > SOC_UNLOCK && v >= V_LOCK) || v >= (charging ? V_UNLOCK_CHG : V_UNLOCK);

    if (currentState != LOW_BATTERY_LOCK) {
      unlockVotes = 0;
      lockVotes = wantLock ? lockVotes + 1 : 0;
      if (lockVotes >= POLICY_CONFIRM) {
        lockVotes = 0;
        currentState = LOW_BATTERY_LOCK;
        pendingStart = false;
        setAllHapticsEnable(false);
      }
    } else {
      lockVotes = 0;
      unlockVotes = wantUnlock ? unlockVotes + 1 : 0;
      if (unlockVotes >= POLICY_CONFIRM) {
        unlockVotes = 0;
        currentState = Bluefruit.connected() ? IDLE_CONNECTED : DEEP_SLEEP;
      }
    }
  }

  if (currentState == LOW_BATTERY_LOCK || (socValid && lastSoc <= SOC_LOCK)) {
    powerStatus |= (1 << POWER_BIT_CRITICAL);
  }

  if (gaugeValid) {
    DEBUG_PRINT("[BAT] V=");
    DEBUG_PRINT(v, 3);
    DEBUG_PRINT(" soc=");
    DEBUG_PRINT(lastSoc);
    DEBUG_PRINT(" socV=");
    DEBUG_PRINT(socV);
    DEBUG_PRINT(" chg=");
    DEBUG_PRINT(charging);
    DEBUG_PRINT(" lockV=");
    DEBUG_PRINT(lockVotes);
    DEBUG_PRINT(" unlockV=");
    DEBUG_PRINT(unlockVotes);
    DEBUG_PRINT(" st=");
    DEBUG_PRINTLN((int)currentState);
  } else {
    DEBUG_PRINTLN("[BAT] lectura del gauge invalida (I2C/NAN o no detectado)");
  }

  bleBattery.write(lastSoc);

  uint8_t telemetryData[6];
  telemetryData[0] = lastSoc;
  telemetryData[1] = currentVariation;
  telemetryData[2] = (uint8_t)currentState;
  telemetryData[3] = powerStatus;
  telemetryData[4] = lastVoltage_mV & 0xFF;
  telemetryData[5] = (lastVoltage_mV >> 8) & 0xFF;

  charTelemetry.notify(telemetryData, sizeof(telemetryData));
}

void connect_callback(uint16_t conn_handle) {
  if (currentState != LOW_BATTERY_LOCK) currentState = IDLE_CONNECTED;
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  setAllHapticsEnable(false);
  if (currentState != LOW_BATTERY_LOCK) currentState = DEEP_SLEEP;
}

void sysCtrl_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  uint8_t cmd = data[0];
  bool locked = (currentState == LOW_BATTERY_LOCK);
  if (cmd == 0x01) {
    if (!locked) pendingStart = true;
  } else if (cmd == 0x02 || cmd == 0x00) {
    setAllHapticsEnable(false);
    if (!locked) currentState = IDLE_CONNECTED;
  } else if (cmd == 0x03) {
    setAllHapticsEnable(false);
    if (!locked) currentState = DEEP_SLEEP;
  } else if (cmd == 0x04) {
    setAllHapticsEnable(false);
    Bluefruit.disconnect(conn_hdl);
  }
#if ENABLE_OTA_CMD
  else if (cmd == 0x05) {
    Bluefruit.Advertising.stop();
    NRF_POWER->GPREGRET = 0xA8;
    NVIC_SystemReset();
  }
#endif
}

void haptic_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  if (len < 1) return;
  if (data[0] >= 1 && data[0] <= 3) currentVariation = data[0];
}

void motors_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  if (len >= 1) {
    DEBUG_PRINT("[CFG] app escribe mask=0x"); DEBUG_PRINTLN(data[0], HEX);
    cfg.motorMask = data[0] & 0x7F;
    markConfigChanged();
  }
  chr->write8(cfg.motorMask);
}

void gain_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  if (len >= 3) {
    cfg.gainGeneral = min(data[0], (uint8_t)100);
    cfg.gainClit = min(data[1], (uint8_t)100);
    cfg.gainG = min(data[2], (uint8_t)100);
    markConfigChanged();
  }
  uint8_t applied[3] = { cfg.gainGeneral, cfg.gainClit, cfg.gainG };
  chr->write(applied, sizeof(applied));
}

void sens_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  if (len >= 2 && voltFromByte(data[1]) > voltFromByte(data[0]) + SENS_MIN_MARGIN) {
    cfg.sensMin = data[0];
    cfg.sensMax = data[1];
    markConfigChanged();
  } else {
    DEBUG_PRINTLN("[CFG] sensibilidad rechazada (VOLT_MAX debe superar a VOLT_MIN + 0.20V)");
  }
  uint8_t applied[2] = { cfg.sensMin, cfg.sensMax };
  chr->write(applied, sizeof(applied));
}

void setupConfigCharacteristic(BLECharacteristic& chr, uint8_t len, BLECharacteristic::write_cb_t cb) {
  chr.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
  chr.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  chr.setFixedLen(len);
  chr.setWriteCallback(cb);
  chr.begin();
}

void setup() {
  DEBUG_BEGIN(115200);
  DEBUG_PRINT("[FW] version ");
  DEBUG_PRINTLN(FW_VERSION);
  NRF_WDT->CONFIG = 0x01;
  NRF_WDT->CRV = 32768 * 5;
  NRF_WDT->TASKS_START = 1;

  Wire.begin();
  SPI.begin();
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  sendFastCommand(CMD_FULL_SHUTDOWN);

  pinMode(PIN_PGOOD, INPUT);
  pinMode(PIN_CHG, INPUT);
  pinMode(PIN_ALRT, INPUT);
  fuelGaugeOK = maxlipo.beginNoReset();
  bool gaugeFromPOR = false;
  if (!fuelGaugeOK) DEBUG_PRINTLN("[POWER] MAX17048 no detectado");
  else if (maxlipo.getAlertStatus() & MAX1704X_ALERTFLAG_RESET_INDICATOR) {
    DEBUG_PRINTLN("[POWER] MAX17048 venia de power-on-reset");
    maxlipo.clearAlertFlag(MAX1704X_ALERTFLAG_RESET_INDICATOR);
    gaugeFromPOR = true;
  }

  Wire.beginTransmission(MUX_ADDR);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.beginTransmission(MCP_EXPANDER_ADDR);
  Wire.write(MCP_IODIR);
  Wire.write(0x01);
  Wire.endTransmission();
  setAllHapticsEnable(false);

  if (fuelGaugeOK && !gaugeFromPOR && digitalRead(PIN_PGOOD) == HIGH) {
    float v = maxlipo.cellVoltage();
    float pct = maxlipo.cellPercent();
    if (!isnan(v) && !isnan(pct) && v > 2.5 && v < 4.5) {
      uint8_t soc = (uint8_t)constrain(pct, 0.0f, 100.0f);
      uint8_t socV = socFromVoltage(v);
      if (abs((int)soc - (int)socV) > SOC_MISMATCH_IDLE) {
        maxlipo.quickStart16();
        lastResync = millis();
        resyncDone = true;
        DEBUG_PRINT("[BAT] quickStart boot: soc=");
        DEBUG_PRINT(soc);
        DEBUG_PRINT(" socV=");
        DEBUG_PRINTLN(socV);
      }
    }
  }

  loadConfig();
  cfg.motorMask |= 0x01;   // motor 1 (punto G) siempre habilitado
  markConfigChanged();
  DEBUG_PRINT("[CFG] mask tras fix=0x"); DEBUG_PRINTLN(cfg.motorMask, HEX);

  Bluefruit.begin();
#if ENABLE_DCDC
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
#endif
  Bluefruit.autoConnLed(false);
  uint8_t mac[6];
  Bluefruit.getAddr(mac);
  char bleName[9];
  snprintf(bleName, sizeof(bleName), "RUB_%02X%02X", mac[1], mac[0]);
  Bluefruit.setName(bleName);
  DEBUG_PRINT("[BLE] nombre: ");
  DEBUG_PRINTLN(bleName);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  bledis.setManufacturer("RUB");
  bledis.setModel("RUB Haptic V2");
  bledis.setFirmwareRev(FW_VERSION);
  bledis.begin();

  bleBattery.begin();
  customService.begin();

  charSysCtrl.setProperties(CHR_PROPS_WRITE | CHR_PROPS_NOTIFY);
  charSysCtrl.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  charSysCtrl.setWriteCallback(sysCtrl_write_callback);
  charSysCtrl.begin();

  charTelemetry.setProperties(CHR_PROPS_NOTIFY);
  charTelemetry.begin();

  charHaptic.setProperties(CHR_PROPS_WRITE);
  charHaptic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  charHaptic.setWriteCallback(haptic_write_callback);
  charHaptic.begin();

  setupConfigCharacteristic(charMotors, 1, motors_write_callback);
  charMotors.write8(cfg.motorMask);

  setupConfigCharacteristic(charGain, 3, gain_write_callback);
  uint8_t gains[3] = { cfg.gainGeneral, cfg.gainClit, cfg.gainG };
  charGain.write(gains, sizeof(gains));

  setupConfigCharacteristic(charSens, 2, sens_write_callback);
  uint8_t sens[2] = { cfg.sensMin, cfg.sensMax };
  charSens.write(sens, sizeof(sens));

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addService(customService);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.setInterval(32, 874);
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);
  currentState = DEEP_SLEEP;
}

void loop() {
  feedWatchdog();
  saveConfigIfNeeded();

  static SystemState prevState = BOOTING;
  SystemState st = currentState;
  if (prevState == ACTIVE_LOOP && st != ACTIVE_LOOP) {
    sendFastCommand(CMD_FULL_SHUTDOWN);
    DEBUG_PRINTLN("[ADC] full shutdown");
  }
  prevState = st;

  if (pendingStart && currentState != LOW_BATTERY_LOCK) {
    pendingStart = false;
    setAllHapticsEnable(true);
    delay(300);
    feedWatchdog();
    initHapticDrivers();
    feedWatchdog();

    sendFastCommand(0x78);
    delay(10);

    writeRegister(REG_CONFIG0, 0x23);
    writeRegister(REG_CONFIG1, 0x0C);
    writeRegister(REG_CONFIG2, 0x8B);
    writeRegister(REG_CONFIG3, 0xC0);
    writeRegister(REG_IRQ, 0x07);

    sendFastCommand(0x68);
    calibrateSensors();
    gGateActive = false;
    currentState = ACTIVE_LOOP;
  }

  switch (currentState) {
    case DEEP_SLEEP:
      delay(500);
      enforcePowerPolicy();
      break;
    case LOW_BATTERY_LOCK:
      delay(500);
      enforcePowerPolicy();
      break;
    case IDLE_CONNECTED:
      enforcePowerPolicy();
      delay(100);
      break;
    case ACTIVE_LOOP:
      {
        enforcePowerPolicy();
        if (currentState != ACTIVE_LOOP) break;

        float voltMin = voltFromByte(cfg.sensMin);
        float voltMax = voltFromByte(cfg.sensMax);

        for (int ch = 0; ch < 6; ch++) {
          int32_t raw = readChannelRaw(channels[ch]);
          long corrected = raw - offset[ch];
          if (corrected < 0) corrected = 0;

          float v = (corrected * VREF) / ADC_SCALE;
          pressure[ch] = constrain((v - voltMin) / (voltMax - voltMin), 0.0, 1.0);
        }

        unsigned long currentMillis = millis();
        float phaseRamp = (currentMillis % 1500) / 1500.0;
        float wRampUp = phaseRamp;
        float wRampDown = 1.0 - phaseRamp;
        float wPulse = (sin((currentMillis % 666) / 666.0 * PI * 2 - PI / 2) + 1.0) / 2.0;

        float t_G = 0, t_U = 0, t_L = 0, t_C = 0, t_R = 0, t_D = 0, t_B = 0;

        if (currentVariation == 1) {
          t_U = pressure[4] * wRampUp;    // Up
          t_D = pressure[1] * wRampUp;    // Down
          t_L = pressure[2] * wRampDown;  // Left
          t_R = pressure[0] * wRampDown;  // Right
          t_C = pressure[5] * wPulse;
          t_B = pressure[3];
        } else if (currentVariation == 2) {
          t_U = pressure[4] * wPulse;
          t_D = pressure[1] * wPulse;
          t_L = pressure[2] * wPulse;
          t_R = pressure[0] * wPulse;
          t_C = pressure[5] * wPulse;
          t_B = pressure[3] * wPulse;
        } else {
          t_U = pressure[4];
          t_D = pressure[1];
          t_L = pressure[2];
          t_R = pressure[0];
          t_C = pressure[5];
          t_B = pressure[3];
        }

        float pArriba = pressure[4], pAbajo = pressure[1], pIzq = pressure[2], pDer = pressure[0];
        bool allAbove = (pArriba > G_THRESH_ON && pAbajo > G_THRESH_ON && pIzq > G_THRESH_ON && pDer > G_THRESH_ON);
        bool anyBelow = (pArriba < G_THRESH_OFF || pAbajo < G_THRESH_OFF || pIzq < G_THRESH_OFF || pDer < G_THRESH_OFF);
        if (!gGateActive && allAbove) gGateActive = true;
        if (gGateActive && anyBelow) gGateActive = false;
        if (gGateActive) t_G = (pArriba + pAbajo + pIzq + pDer) / 4.0;

        float gGeneral = cfg.gainGeneral / 100.0;
        float gClit = cfg.gainClit / 100.0;
        float gG = cfg.gainG / 100.0;

#if ENABLE_DEBUG && DEBUG_SENSORS
        static unsigned long lastSensorPrint = 0;
        if (currentMillis - lastSensorPrint >= 250) {
          lastSensorPrint = currentMillis;
          DEBUG_PRINT("[SNS] Arr=");
          DEBUG_PRINT(pArriba, 3);
          DEBUG_PRINT(" Aba=");
          DEBUG_PRINT(pAbajo, 3);
          DEBUG_PRINT(" Izq=");
          DEBUG_PRINT(pIzq, 3);
          DEBUG_PRINT(" Der=");
          DEBUG_PRINT(pDer, 3);
          DEBUG_PRINT(" Clit=");
          DEBUG_PRINT(pressure[3], 3);
          DEBUG_PRINT(" Punta=");
          DEBUG_PRINT(pressure[5], 3);
          DEBUG_PRINT(" | gate=");
          DEBUG_PRINT(gGateActive);
          DEBUG_PRINT(" G_pwr=");
          DEBUG_PRINT((int)(constrain(t_G * gG, 0.0f, 1.0f) * 127.0));
          DEBUG_PRINT(" mask=0x");
          DEBUG_PRINTLN(cfg.motorMask, HEX);
        }
#endif
        // ESTO ES DEL PROGRAMA ORIGINAL
        // auto updateM = [](int idx, float target) {
        //   if (!(cfg.motorMask & (1 << idx))) target = 0;
        //   int targetPower = (int)(constrain(target, 0.0f, 1.0f) * 127.0);
        //   if (targetPower < 5) targetPower = 0;
        //   if (targetPower == grupo[idx].lastSentValue) return;
        //   grupo[idx].currentIntensity = (float)targetPower;
        //   selectMux(grupo[idx].muxChannel);
        //   drv.setRealtimeValue((uint8_t)targetPower);
        //   grupo[idx].lastSentValue = (uint8_t)targetPower;
        // };

        //ESTOS SON MIS CAMBIOS
        auto updateM = [](int idx, float target) {
          if (!((cfg.motorMask | 0x01) & (1 << idx))) target = 0;

          const float BOOST = 1.5;   // multiplicador (sube/baja a gusto)
          const int MIN_POWER = 35;  // potencia mínima a la que el motor realmente vibra
          float t = constrain(target * BOOST, 0.0f, 1.0f);

          int targetPower = (t < 0.04f) ? 0 : (int)(MIN_POWER + t * (127 - MIN_POWER));
          if (targetPower == grupo[idx].lastSentValue) return;

          grupo[idx].currentIntensity = (float)targetPower;
          selectMux(grupo[idx].muxChannel);
          drv.setRealtimeValue((uint8_t)targetPower);
          grupo[idx].lastSentValue = (uint8_t)targetPower;
        };
        updateM(0, t_G * gG);
        updateM(1, t_U * gGeneral);
        updateM(2, t_C * gGeneral);
        updateM(3, t_L * gGeneral);
        updateM(4, t_R * gGeneral);
        updateM(5, t_D * gGeneral);
        updateM(6, t_B * gClit);

        delayMicroseconds(500);
        break;
      }
  }
}
