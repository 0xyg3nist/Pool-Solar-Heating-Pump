// Three (3) Pt100 Sensor Connector Proof of Concept
// RRef = 430ohm (4300)
// Controller: Arduino MEGA 2560
// Lower Skill Level Commentary to aid faster uptake of technology

#include <Arduino.h>
#include <SPI.h>
// ICSP Header SPI: MISO:1, VCC:2, SCK:3, MOSI:4, /RESET:5, GND:6.
// MEGA SPI PINS: MISO:50, MOSI:51, SS:53, SCK:52 parallels the ICSP Header
// UNO SPI PINS:  MISO:12, MOSI:11, SS:10, SCK:13 parallels the ICSP Header
// MOSI == {DIN, SDI}. MISO == {DO, SDO}
// https://arduino.stackexchange.com/questions/16348/how-do-you-use-spi-on-an-arduino

#include <Adafruit_MAX31865.h>
// Common CLK, SDO, SDI to three MAX31865 Sensor Boards.
#define MAXCLK 52
#define MAXDI 51
#define MAXDO 50
// Deployment Specific Pins for Clock Select / Slave Select...
#define ONE_MAXCS  37
#define TWO_MAXCS  35
#define THR_MAXCS  33

// SENSOR ALLOCATION:
//    ONE: Roof Surface Temperature, measured from underneath.
//    TWO: Pool Solar Heater Pump Inlet Temperature, measured outside of pipe.
//    THR: Pool Solar Heater Pump Outlet Temperature, measured outside of pipe.

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

Adafruit_MAX31865 ONE = Adafruit_MAX31865(ONE_MAXCS, MAXDI, MAXDO, MAXCLK);
Adafruit_MAX31865 TWO = Adafruit_MAX31865(TWO_MAXCS, MAXDI, MAXDO, MAXCLK);
Adafruit_MAX31865 THR = Adafruit_MAX31865(THR_MAXCS, MAXDI, MAXDO, MAXCLK);

void setup() {
  Serial.begin(115200);
  Serial.println("Three Adafruit MAX31865 4-WIRE PT100 Sensors!");
  Serial.println(" *** TJL DEC 2019");

// Initialise CS lines as not-selected
  digitalWrite(ONE_MAXCS, HIGH);
  digitalWrite(TWO_MAXCS, HIGH);
  digitalWrite(THR_MAXCS, HIGH);


  ONE.begin(MAX31865_2WIRE);  // set to 2WIRE, 3WIRE or 4WIRE as necessary
  TWO.begin(MAX31865_2WIRE);  // set to 2WIRE, 3WIRE or 4WIRE as necessary
  THR.begin(MAX31865_2WIRE);  // set to 2WIRE, 3WIRE or 4WIRE as necessary
}

void loop() {
  // put your main code here, to run repeatedly:
  // Reset all slave select lines to disabled
  digitalWrite(ONE_MAXCS, HIGH);
  digitalWrite(TWO_MAXCS, HIGH);
  digitalWrite(THR_MAXCS, HIGH);

  //Serial.println("This is the first PlatformIO code!");
  digitalWrite(ONE_MAXCS, LOW);
  delay(50);
  uint16_t rtd_one = ONE.readRTD();
  digitalWrite(ONE_MAXCS, HIGH);
  digitalWrite(TWO_MAXCS, LOW);
  delay(50);
  uint16_t rtd_two = TWO.readRTD();
  digitalWrite(TWO_MAXCS, HIGH);
  digitalWrite(THR_MAXCS, LOW);
  delay(50);
  uint16_t rtd_thr = THR.readRTD();
  digitalWrite(THR_MAXCS, HIGH);

  Serial.print("RTD values: Sensor One:"); Serial.print(rtd_one);
  Serial.print(" n. Sensor Two:"); Serial.print(rtd_one);
  Serial.print(" n. Sensor Three:"); Serial.print(rtd_one); Serial.println(" n.");

  float ratio_one = rtd_one;
  float ratio_two = rtd_two;
  float ratio_thr = rtd_thr;
  ratio_one /= 32768;
  ratio_two /= 32768;
  ratio_thr /= 32768;

  digitalWrite(ONE_MAXCS, LOW);
  delay(50);
  Serial.print("SENSOR ONE:::: Ratio = "); Serial.print(ratio_one,8);
  Serial.print(" n. Resistance = "); Serial.print(RREF*ratio_one,8);
  Serial.print(" ohms. Temperature = "); Serial.print(ONE.temperature(RNOMINAL, RREF)); Serial.println(" deg C.");
  digitalWrite(ONE_MAXCS, HIGH);

  digitalWrite(TWO_MAXCS, LOW);
  delay(50);
  Serial.print("SENSOR TWO:::: Ratio = "); Serial.print(ratio_two,8);
  Serial.print(". Resistance = "); Serial.print(RREF*ratio_two,8);
  Serial.print(". Temperature = "); Serial.print(TWO.temperature(RNOMINAL, RREF)); Serial.println(" deg C.");
  digitalWrite(TWO_MAXCS, HIGH);

  digitalWrite(THR_MAXCS, LOW);
  delay(50);
  Serial.print("SENSOR THREE:: Ratio = "); Serial.print(ratio_thr,8);
  Serial.print(" n. Resistance = "); Serial.print(RREF*ratio_thr,8);
  Serial.print(" ohms. Temperature = "); Serial.print(THR.temperature(RNOMINAL, RREF)); Serial.println(" deg C.");
  digitalWrite(THR_MAXCS, HIGH);

  Serial.println("... end main temperature readout.");
  Serial.println("");

  // Check and print any faults
  digitalWrite(ONE_MAXCS, LOW);
  delay(50);
  uint8_t fault = ONE.readFault();
  if (fault) {
    Serial.print("SENSOR  ONE  Temperature: Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage");
    }
    ONE.clearFault();
  }
  digitalWrite(ONE_MAXCS, HIGH);

  digitalWrite(TWO_MAXCS, LOW);
  delay(50);
    fault = TWO.readFault();
      if (fault) {
        Serial.print("SENSOR  TWO  Temperature: Fault 0x"); Serial.println(fault, HEX);
        if (fault & MAX31865_FAULT_HIGHTHRESH) {
          Serial.println("RTD High Threshold");
        }
        if (fault & MAX31865_FAULT_LOWTHRESH) {
          Serial.println("RTD Low Threshold");
        }
        if (fault & MAX31865_FAULT_REFINLOW) {
          Serial.println("REFIN- > 0.85 x Bias");
        }
        if (fault & MAX31865_FAULT_REFINHIGH) {
          Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
        }
        if (fault & MAX31865_FAULT_RTDINLOW) {
          Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
        }
        if (fault & MAX31865_FAULT_OVUV) {
          Serial.println("Under/Over voltage");
        }
      TWO.clearFault();
    }
    digitalWrite(TWO_MAXCS, HIGH);

    digitalWrite(THR_MAXCS, LOW);
    delay(50);
        fault = THR.readFault();
        if (fault) {
          Serial.print("SENSOR THREE Temperature: Fault 0x"); Serial.println(fault, HEX);
          if (fault & MAX31865_FAULT_HIGHTHRESH) {
            Serial.println("RTD High Threshold");
          }
          if (fault & MAX31865_FAULT_LOWTHRESH) {
            Serial.println("RTD Low Threshold");
          }
          if (fault & MAX31865_FAULT_REFINLOW) {
            Serial.println("REFIN- > 0.85 x Bias");
          }
          if (fault & MAX31865_FAULT_REFINHIGH) {
            Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
          }
          if (fault & MAX31865_FAULT_RTDINLOW) {
            Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
          }
          if (fault & MAX31865_FAULT_OVUV) {
            Serial.println("Under/Over voltage");
          }
        THR.clearFault();
      }
      digitalWrite(THR_MAXCS, HIGH);
}
