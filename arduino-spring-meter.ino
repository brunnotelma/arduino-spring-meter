#include <HX711.h>
#include <VL53L0X.h>
#include <LiquidCrystal_I2C.h>

#define BUTTON              8       // Pino do Push button
#define DOUT                3       // Porta DT no HX711
#define CLK                 2       // Porta SCK no HX711
#define CALIBRATION_FACTOR  -4220.0 // Valor de calibracao
#define LB_CALC             2.20462 // Constante para conversao em lb

// Display LCD 16x2
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
// VL53L0X - Sensor de distancia laser
VL53L0X sensor;
// HX711 - Amplificador celula de carga
HX711 scale(DOUT, CLK);
// Ultima distancia medida (usado para resetar a medida)
uint16_t lastDistance = 0;

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
  sensor.init();
  sensor.setTimeout(500);
  sensor.setMeasurementTimingBudget(100000);

  // Seta o valor de calibracao
  scale.set_scale(CALIBRATION_FACTOR);
  // Reseta a escala da celula de carga (sem peso)
  scale.tare();
}

void loop() {
  // Se o botao foi apertado
  if (digitalRead(BUTTON) == HIGH) {
    // Limpa o display
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Resetando...");
    // Delay para fazer o debounce
    delay(500);
    // Reseta os sensores
    reset();
  }
  
  // Distancia em mm
  uint16_t distance = sensor.readRangeSingleMillimeters();

  // Se a distancia de reset for menor ou igual
  if (lastDistance <= distance) {
    // Subtrai a distancia medida pelo reset
    distance -= lastDistance;
  } else { 
    // Se for maior, distancia = 0
    // Pra nao exibir ditancia negativa
    distance = 0;
  }

  float inches = distance / 25.4;
  
  // Peso em kg
  float weight = scale.get_units(10);

  // Se peso for menor que 0.0
  if (weight < 0) {
    // Exibe 0.0 como medida
    weight = 0.0;
  }

  // Multiplica pelo valor de um Kg em Lb
  weight = weight * LB_CALC;

  // Limpa o display
  lcd.clear();
  
  // Exibe a distancia no display
  lcd.setCursor(0,0);
  lcd.print("Dist:");
  lcd.setCursor(5,0);
  lcd.print(inches);
  lcd.setCursor(13,0);
  lcd.print("pol");

  // Exibe o peso no display
  lcd.setCursor(0,1);
  lcd.print("Peso:");
  lcd.setCursor(5,1);
  lcd.print(weight);
  lcd.setCursor(14,1);
  lcd.print("lb");
}

void reset() {
  // Reseta a balanca
  scale.tare();
  // Reseta o sensor de distancia
  lastDistance = sensor.readRangeSingleMillimeters();
}
