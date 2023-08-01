/*
 Fundacion Kinal
 Centro Educativo Tecnico Laboral KINAL
 Electronica
 Grado:5to A
 Nombre:Roberto Monterroso
 Carne :2019507
 */
//Librerias
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <MFRC522.h>
#include <SPI.h>
#include <RTClib.h>

//Directivas de preprocesador

//LCD
#define LCD_ADRESS 0x26
#define filas 2
#define columnas 16
//PINES A USAR 
#define LED_GREEN 3
#define LED_RED 4
#define RST_PIN 9
#define SS_PIN 10

//variables a usar 
float Sensibilidad=0.185;
int Sensor = A1;

//Constructores
LiquidCrystal_I2C LCD(LCD_ADRESS, columnas, filas);
MFRC522 RFID(SS_PIN, RST_PIN);
RTC_DS3231 rtc;

//funciones a utilizar
void valor_LCD(void);
void Usuario(void);
void Usuario_incorrecto(void);

void setup() {
Serial.begin(9600);
LCD.init();// Activa la LCD
LCD.backlight();
Wire.begin();// Activa la comunicacion I2C
rtc.begin(); 
SPI.begin();// Activa el bus SPI
RFID.PCD_Init();// Activa el MFRC522
Serial.println("Ingrese su Tarjeta...");
Serial.println();
Serial.println("- - - - - - - - - - - - - - - - - -");
pinMode(LED_GREEN,OUTPUT);// pin del led como salida
pinMode(LED_RED,OUTPUT);// pin del led como salida

// 31 de Julio del 2023, 10:59 am 
rtc.adjust(DateTime(2023, 7, 31, 10, 59, 30));   
}

void loop() {
LCD.setCursor(0,0);
LCD.print("  EEGSA_Kinal  ");
LCD.setCursor(0,1);
LCD.print("  Energia:   ");
digitalWrite(LED_RED, LOW); // El led inicia apagado
digitalWrite(LED_GREEN,LOW); // El led inicia apagado

 // Acceso para nuevas tarjeras
  if ( ! RFID.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Selecciona una de las tarjetas
  if ( ! RFID.PICC_ReadCardSerial()) 
  {
    return;
  }
  // Muestra el UID sobre el Monitor Serial
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < RFID.uid.size; i++) 
  {
     Serial.print(RFID.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(RFID.uid.uidByte[i], HEX);
     content.concat(String(RFID.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(RFID.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.println("- - - - - - - - - - - - - - - - - -");
  Serial.print("Mensaje: ");
  content.toUpperCase();
  if (content.substring(1) == "13 85 5B AD") // si el No. de tarjeta es el correcto, realiza lo siguiente:
  {
  Serial.println("Acceso Autorizado");
  Serial.println("- - - - - - - - - - - - - - - - - -");
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED,LOW);
  Usuario(); // funcion que contiene las instrucciones para confirmar que el usuario es correcto
  delay(2000); // se mantiene el mensaje por 2 segundos
  valor_LCD(); // muestra en la LCD los valores 
  delay(6000); // se mantiene el mensaje por 6 segundos
  }
  else if (content.substring(1) != "13 85 5B AD"){ // si el No. de tarjeta es incorrecto, realiza la siguiente accion 
    Serial.println("Acceso Denegado");
    Serial.println("- - - - - - - - - - - - - - - - - -");
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN,LOW);
  Usuario_incorrecto(); //funcion que indica que es el usuario incorrecto
  delay(1500);
  }
}

void valor_LCD(void){ // funcion que muestra los valores de V, C, P

// Valores de voltaje
float voltaje =  (float)25*analogRead(A1)/1023;//operacion para sacar el valor de voltaje

// Valores de corriente
float corriente = get_corriente(200);  
  
// Valores de potencia
 float potencia = corriente * voltaje;

LCD.clear();
LCD.setCursor(0,0);  
LCD.print("  EEGSA - Kinal  ");
LCD.setCursor(0,1);  
LCD.print(voltaje,1);
LCD.setCursor(4,1);
LCD.print("V");
LCD.setCursor(6,1);  
LCD.print(corriente, 1);
LCD.setCursor(9,1);
LCD.print("A");
LCD.setCursor(11,1);  
LCD.print(potencia, 1); 
LCD.setCursor(15,1);
LCD.print("W");
} 

void Usuario(){ // funcion que confirma si el usuario registrado es correcto
DateTime now = rtc.now();
LCD.clear();
LCD.setCursor(0,0);  
LCD.print("Registered  User");
LCD.setCursor(0,1);  
LCD.print("Time:");
LCD.setCursor(5,1);
LCD.print(" ");
LCD.print(now.hour(), DEC);
LCD.print(':');
LCD.print(now.minute(), DEC);
LCD.print(':');
LCD.print(now.second(), DEC);
}

void Usuario_incorrecto(){ // funcion que confirma si el usuario registrado es incorrecto
LCD.clear();
LCD.setCursor(0,0);  
LCD.print("  EEGSA - Kinal  ");
LCD.setCursor(0,1);  
LCD.print("  Invalid User  ");  
}

float get_corriente(int n_muestras) // funcion que acompana la formula para obtener el valor de corriente
{
  float voltajeSensor;
  float CURRENT=0;
  for(int i=0;i<n_muestras;i++)
  {
    voltajeSensor = analogRead(A0) * (5.0 / 1023.0);//lectura del sensor
    CURRENT=CURRENT+(voltajeSensor-2.5)/Sensibilidad; //Ecuación  para obtener la corriente
  }
  CURRENT=CURRENT/n_muestras;
  return(CURRENT);
}
