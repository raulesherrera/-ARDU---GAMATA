// Defines para los motores.
#define IN1  13   // Pines para M1
#define IN2  12   // Pines para M1
#define IN3  11   // Pines para M1
#define IN4  10   // Pines para M1
#define ON1 9     // Pines para M2
#define ON2 8     // Pines para M2
#define ON3 7     // Pines para M2
#define ON4 6     // Pines para M2

// Defines para los DHT
#include "DHT.h"  // Cargamos la libreria DHT
#define DHTPIN_1 5  // Pin para el sensor DHT22 #1
#define DHTPIN_2 4  // Pin para el sensor DHT22 #2
#define DHTTYPE DHT22 // SE selecciona el tipo de DHT (hay otros DHT)

// Defines para la fotoresistencia
#define ResLight 15  // Resistencia a la luz (10 lux) en Kilo Ohms
#define ResCalib 10  // Resistencia calibración en KiloOhms
#define ResOscu 1000 // Resistencia en oscuridad en KiloOhms

// Defines para el riego y para el fertilizante.
#define AguaPIN 3 // Pin para el riego del agua
#define FertPIN 2 // PIN para el riego del agua

String comando;
boolean stringComplete = false;

// Variables necesarias para el motor.
int steps_left = 4095;
boolean Direction = true;
boolean Direction2 = true;
int Steps = 0;
int Steps2 = 0;
int a = 0;
int Paso [ 8 ][ 4 ] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

// Variables necesarias para el DHT
DHT dht_1(DHTPIN_1, DHTTYPE); // Se inicia una variable que será usada por Arduino para comunicarse con el sensor
DHT dht_2(DHTPIN_2, DHTTYPE); // Se inicia otra variable que será usada por Arduino para comunicarse con el sensor
float temp, hum; // Se crean variables para obtener los datos.
int c = 0, b = 0;

// Variables necesarias para la resistencia foto.
const int LDRPin = A0;
int V, ilum;

void setup() {
  // Setup necesario para los motores
  Serial.begin(9600);
  comando.reserve(200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ON1, OUTPUT);
  pinMode(ON2, OUTPUT);
  pinMode(ON3, OUTPUT);
  pinMode(ON4, OUTPUT);

  // Setup necesario para los sensores DHT22
  dht_1.begin(); // Se inicia el sensor (1).
  dht_2.begin(); // Se inicia el sensor (2).
  pinMode(DHTPIN_1, OUTPUT);
  pinMode(DHTPIN_2, OUTPUT);

  // Setup necesario para el riego y la fertilizacion
  pinMode(AguaPIN, OUTPUT);
  pinMode(FertPIN, OUTPUT);

  //Establecer la temperatura máxima
  setTempHum();
}

void loop() {
  leerDHTs();
  
  if (comando.equals("subir")) {
    Direction = false;
    Direction2 = true;
    motores();
  }

   else if (comando.equals("bajar")) {
    Direction = true;
    Direction2 = false;
    motores();
  }

  else if (comando.equals("setear")) {
    setTempHum();
  }

  else if (comando.equals("leerTempHum")) {
    leerDHTs();
  }

  else if (comando.equals("getLums")){
    leerLums();
  }

  else if (comando.equals("regar")){
    regar();
  }

  else if (comando.equals("fertilizar")){
    fertilizar();
  }
}
/*
    Inicia el codigo para el motor
*/
void motores() {
  for (int a = 0; a < 4; a++) {
    while (steps_left > 0) {
      stepper();    // Avanza un paso
      stepper2();
      steps_left-- ;  // Un paso menos
      delay (1) ;
    }
    steps_left = 4095;
  }
  delay(2000);
}

void stepper() {
  digitalWrite( IN1, Paso[Steps][ 0] );
  digitalWrite( IN2, Paso[Steps][ 1] );
  digitalWrite( IN3, Paso[Steps][ 2] );
  digitalWrite( IN4, Paso[Steps][ 3] );
  SetDirection();
}

void stepper2() {
  digitalWrite( ON1, Paso[Steps2][ 0] );
  digitalWrite( ON2, Paso[Steps2][ 1] );
  digitalWrite( ON3, Paso[Steps2][ 2] );
  digitalWrite( ON4, Paso[Steps2][ 3] );
  SetDirection2();
}

void SetDirection() {
  if (Direction)
    Steps++;
  else
    Steps--;
  Steps = ( Steps + 7 ) % 7 ;
}

void SetDirection2() {
  if (Direction2)
    Steps2++;
  else
    Steps2--;
  Steps2 = ( Steps2 + 7 ) % 7 ;
}
/*
   Terminan los codigos para los motores
*/

/*
   Comienzan los codigos para los DHT22
*/
void setTempHum() {
  Serial.println("Favor de introducir la temperatura estandar.");
  while (c == 0) {
    temp = Serial.parseFloat();
    delay(500);
    if (temp) {
      c = 1;
    }
  }
  Serial.println("Favor de introdicr la humedad estandar.");
  while (b == 0) {
    hum = Serial.parseFloat();
    delay(500);
    if (hum) {
      b = 1;
    }
  }
}

void leerDHTs() {
  float h1 = dht_1.readHumidity(); // Lee la humedad del primer sensor.
  float h2 = dht_2.readHumidity(); // Lee la humedad del segundo sensor
  float t1 = dht_1.readTemperature(); // Lee la temperatura del primer sensor.
  float t2 = dht_2.readTemperature(); // Lee la temperatura del segundo sensor.
  float h = (h1 + h2) / 2;
  float t = (t1 + t2) / 2;

  if (t >= temp) {
    motores();
  }
  if (h < hum) {
    motores();
  }
}
/*
 * Terminan los codigos de DHT22
 */
/*
 * Comienzan los codigos para luminocidad
 */
void leerLums(){
  ilum = ((long)(1024-V)*ResOscu*10)/((long)ResLight*ResCalib*V); // Usar si LDR entre GND y A0
  ilum = ((long)V*ResOscu*10)/((long)ResLight*ResCalib*(1024-V)); // Usar si LDR entre A0 y Vcc (como en el esquema anterior)
  Serial.println(ilum);
  delay(1000);
}
/*
 * Terminan los codigos para luminocidad
 */

 /*
  * Comienzan los codigos para regar y fertilizar.
  */
void regar(){
  digitalWrite(AguaPIN, HIGH);
  delay(2000);
  digitalWrite(AguaPIN, LOW);
  delay(2000);
}

void fertilizar(){
  digitalWrite(FertPIN, HIGH);
  delay(2000);
  digitalWrite(FertPIN, LOW);
  delay(2000);
}
/*
 * Terminan los codigos de rogar y fertilizar.
 */
void serialEvent(){
  while(Serial.available()){
    char inChar = (char)Serial.read();
    if (inChar == '\r') continue;
    if(inChar == '\n'){
      stringComplete = true;
    } else {
      comando += inChar;
    }
  }
}
