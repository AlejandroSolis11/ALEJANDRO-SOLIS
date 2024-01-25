#include <Arduino.h>
 
//Definicion de variables de uso general
const int pressureInput = A5;//RAW
float sensorVal;
float V = 0;

//Definicion de variable de presion RAW
float pressureValueRAW = 0;

//Definicion de variable filtrada
float pressureValueFILTER = 0;

//Definicion de varaibles de operacion de filtro
const float alpha = 0.95;
float filter = 100;

    
    #define BPS 9600 // COM SERIAL A 9600 BAUDS

    #define VOLTMAX2 5
    #define BITMAX 1023
    #define VOLTMIN2 0.47
    #define BITMIN 100
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
    #define VOLTMIN 0.47
    #define PRESMIN 97580
    #define RANGOPRES (PRESMAX-PRESMIN)
    #define RANGOVOLT (VOLTMAX-VOLTMIN)
    #define SLOPE (RANGOPRES/RANGOVOLT)

        float volt2pres(float volt)
        {
        return SLOPE*(voltios(filter)-VOLTMIN)+PRESMIN;
        }
void setup() {
    Serial.begin(BPS);
}

void loop() {
    Serial.println("sensorVal\tfilter\tVolt\tPres");
    Serial.print(sensorVal);
    Serial.print("\t\t");
    Serial.print(filter);
    Serial.print("\t");
    Serial.print(V);
    Serial.print("\t");
    Serial.println(pressureValueFILTER);

    //sensorVal//
        //En esta variable, guardamos lo que nos devuelve el sensor dependiendo el Delay del programa, en este caso,
        // cada segundo toma una medida la cual se introduce en las funciones de voltios y volt2pres.
    sensorVal = (float)analogRead(pressureInput);//RAW
       
    //pressureValueRAW//
        //
    pressureValueRAW = (volt2pres(voltios(sensorVal)));

    //filtro//
        //Este filtro, se encarga de filtrar valga la redundancia las medidas que toma nuestro sensor con
        //la finalidad de tener una respuesta mas estable, reduciendo el ruido segun un valor alpha que instauramos
        //nosotros como usuario.

    filter = (alpha*sensorVal)+((1-alpha)*filter);

    //pressureValueFILTER (FILTRADO)// 
        //Valor de presion el cual se obtiene, usando la funcion de voltios, para obtener los voltios y finalmente, esos voltios que da el sensor,
        //se pasan a presion gracias a las funciones voltios y volt2pres.
    V = voltios(filter);
    pressureValueFILTER = (volt2pres(voltios(filter)));

    delay(1000);
    }