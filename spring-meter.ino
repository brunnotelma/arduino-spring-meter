
#include <Wire.h>  
#include <Adafruit_VL53L0X.h>
#include <LiquidCrystal_I2C.h>
#include <HX711.h>

#define CALIBRATION_FACTOR -7050.0  // Valor de calibracao
#define DOUT  3                     // Porta DT no HX711
#define CLK  2                      // Porta SCK no HX711

// Display LCD 16x2
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
// CJVL53L0XV2 - Sensor de distancia laser
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
// HX711 - Amplificador celula de carga
HX711 scale(DOUT, CLK);

void setup(){
  // Inicia o display 16x2
  lcd.begin(16,2);
  // Liga o Backlight do display
  lcd.backlight();

  // Espera porta Serial abrir no USB
  while (!Serial) {
    delay(1);
  }

  // Inicia o sensor de distancia
  if (!lox.begin()) {
    Serial.println(F("Falha ao iniciar sensor VL53L0X"));
    while(1);
  }

  // Seta o valor de calibracao
  scale.set_scale(CALIBRATION_FACTOR);
  // Reseta a escala da celula de carga (sem peso)
  scale.tare();
}

void loop(){
  VL53L0X_RangingMeasurementData_t measure;

  // Mede a distancia (mm)
  lox.rangingTest(&measure, false);

  // Limpa o display
  lcd.clear();
  lcd.setCursor(0,0);
  
  // Se a distancia for valida
  if (measure.RangeStatus != 4) {
    // Exibe a distancia no display
    lcd.print("Distancia:");
    lcd.setCursor(10,0);
    // Distancia em milimetros
    lcd.print(measure.RangeMilliMeter);
  } else {
    lcd.print("Fora do alcance.");
  }
  
  lcd.setCursor(0,1);
  lcd.print("Peso:");
  lcd.setCursor(5,1);
  // Peso em lb
  lcd.print(scale.get_units());
    
  delay(100);
}
