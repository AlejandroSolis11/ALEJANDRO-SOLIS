#include <Arduino.h>
#include <U8g2lib.h>

// Variable que indica si la pregunta se ha hecho
boolean respuestaDisponible = false;
#include <LiquidCrystal.h>  

const int pressureInput= A5;
float COMP;
float AnalogValue=0;
float pressureValue=0;
float Analog_measure=0;
float rep=0;
const float alpha = 0.95;
float filter = 100;
float sensorVal;
float voltage;
float psiVal;
const float aVal = 5;

// Calcula la presión a una determinada altura suponiendo modelo ISA
#define P_SL 101325    // Presión ISA a nivel del mar
#define SL 0.000022558 // 
#define POT 5.2559     // 
float ISApressure(float alturaEnMetros) {
  return P_SL*(pow((1-SL*alturaEnMetros),(POT))); 
}

#define VOLTMAX2 5
#define BITMAX 1023
#define VOLTMIN2 0.44
#define BITMIN 101
#define RANGOBIT (BITMAX-BITMIN)
#define RANGOVOLT2 (VOLTMAX2-VOLTMIN2)
#define SLOPEE (RANGOVOLT2/RANGOBIT)

float voltios(uint8_t pinSensor)
{
return SLOPEE*(filter-BITMIN)+VOLTMIN2;
//return analogRead(pinSensor);
}

#define VOLTMAX 5
#define PRESMAX 206842.8
#define VOLTMIN 0.44
#define PRESMIN 97580
#define RANGOPRES (PRESMAX-PRESMIN)
#define RANGOVOLT (VOLTMAX-VOLTMIN)
#define SLOPE (RANGOPRES/RANGOVOLT)

float volt2pres(float volt)
{
  return SLOPE*(voltios(filter)-VOLTMIN)+PRESMIN;
}

 LiquidCrystal lcd(10, 9, 8, 5, 4, 3, 2); //(RS, RW, E, D4,D5, D6, D7)
#define BPS 9600 // COM SERIAL A 9600 BAUDS
#define PEET2MTS 0.3048 // Cte de paso de pies a metros
// Variables que indican si la pregunta se ha hecho

float alturaEnPiesAnterior = 0;

void setup() {
  Serial.begin(BPS);
  lcd.begin(16, 2);

  // Inicializa las variables a false
  respuestaDisponible = false;
  alturaEnPiesAnterior = 0;
}

void loop() {
  float alturaEnMetros;
  float presion_ISA;  // Variable donde se almacena la presión calculada
  String inputString; // Entrada de usuario
  float alturaEnPies; // Altura introducida en pies

  // Si la pregunta no se ha hecho, la realiza
  
    Serial.println("Por favor, ingresa una altura en pies [Ft]:");
    while (!respuestaDisponible) {
      if (Serial.available()) {
        String inputString = Serial.readStringUntil('\n');
        alturaEnPies = inputString.toFloat();
        alturaEnMetros = alturaEnPies * PEET2MTS;
        respuestaDisponible = true;

        // Si la respuesta es la misma que la anterior, no la repite
        if (alturaEnPies != alturaEnPiesAnterior) {
          alturaEnPiesAnterior = alturaEnPies;
        } else {
          break;
        }
      }
    }
    // Cálculo de la presión ISA
    presion_ISA = ISApressure(alturaEnMetros)/100;
    //
    sensorVal=(float)analogRead(pressureInput);
    
    //filtro//
    filter=(alpha*sensorVal)+((1-alpha)*filter);
    //voltage = (filter/1023)* aVal;
    //psiVal = 24066.69*voltage-86509.32;

    pressureValue = (volt2pres(voltios(filter)))/100;
    delay (1000);
  // Presión obtenida
  Serial.print("La presion equivalente a ");
  Serial.print(alturaEnPies);
  Serial.print("[ft] es ");
  Serial.print(presion_ISA);
  Serial.println("[hPa]");

  // Comparar 'presion_ISA' con 'presionLeida'
  
  COMP=presion_ISA-pressureValue;
  Serial.print(COMP);
  lcd.setCursor(0,0);
  //lcd.print("SP");
  //lcd.setCursor(3, 0);
  lcd.print(pressureValue);
  //lcd.print(" hPa  ");
  lcd.setCursor(7,0);
  lcd.print(presion_ISA);
  lcd.print("hPa  ");
  lcd.setCursor(0,1);
  lcd.print("DIFF");
  lcd.print(COMP);
  lcd.print(" hPa  ");

  
  // Llevar 'presion_ISA', 'presionLeida' y comparación a display

  delay(1000);
}
