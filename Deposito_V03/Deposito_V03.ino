#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "DHT.h"

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


//SoftwareSerial linkSerial(10, 11); // RX, TX

const int OUT_PIN = A4;
const int IN_PIN = A0;
long int acumulado=0;
long int actualizado=0;
long int nivel=0;
int agua = 0;
boolean aguabool = 0;

const int DHTPin = 5;     // what digital pin we're connected to

char* tankKey = "tanks.fuel.0.currentLevel";
char* TemperatureKey = "environment.inside.mainCabin.temperature";
char* HumidityKey = "environment.inside.mainCabin.relativeHumidity";
char* sourcetankKey = "Tanque Gasolina";
char* sourceTemperatureKey = "Tempreatura";
char* sourceHumidityKey = "Humedad";


//char* sensorKey = "propulsion.0.revolutions";

void sendSigK(String sigKey, float data);
DHT dht(DHTPin, DHTTYPE);

void setup(){
   Serial.begin(1200);
   Serial1.begin(1200);
   //linkSerial.begin(600);
   pinMode(OUT_PIN, OUTPUT);
   pinMode(IN_PIN, OUTPUT);
   while (!Serial) continue; 
   dht.begin();
}


void sendSigK(String sigKey, float data, String source)
{
 //if (sendSig_Flag == 1)
 //{
   DynamicJsonDocument delta(1024);
   String deltaText;

   
   //updated array
   JsonArray updatesArr = delta.createNestedArray("updates");
   JsonObject thisUpdate = updatesArr.createNestedObject();   //Json Object nested inside delta [...
   JsonArray values = thisUpdate.createNestedArray("values"); // Values array nested in delta[ values....
   JsonObject thisValue = values.createNestedObject();
   
   thisValue["path"] = sigKey;
   thisValue["value"] = data;
   thisValue["Source"] = source;

   // Send to serial
   //serializeJson(delta, linkSerial);
   serializeJson(delta, Serial);
   serializeJson(delta, Serial1);
   Serial1.println();
   Serial.println();
   
 //}
}
void loop(){
   
   for(int i=0; i<100; i++){
      pinMode(IN_PIN, INPUT);
      digitalWrite(OUT_PIN, HIGH);
      int val = analogRead(IN_PIN);
      digitalWrite(OUT_PIN, LOW);
      pinMode(IN_PIN, OUTPUT);
      float capacitancia = (float)val * 24.5 / (float)(1023 - val);
      acumulado = capacitancia + acumulado;
   }  
   actualizado = acumulado/100;
   acumulado=0;
   nivel = ((actualizado - 7) * 100) /38;
   //Serial.print("Valor en pF: ");
   //Serial.print(actualizado);
   //Serial.print(" Nivel:");
   //Serial1.print(" Nivel:");
   //Serial.print(nivel);
   //Serial1.print(nivel);
    //Serial.println("%");
   //Serial1.println("%");

   float h = dht.readHumidity();
   float t = dht.readTemperature();
   agua = analogRead(A1);
   if (agua != 0)
   aguabool = 1;
   else
   aguabool = 0;
 
   if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      //return;
   }
   Serial.print("Humidity: ");
   Serial.print(h);
   Serial.println(" %\t");
   Serial.print("Temperature: ");
   Serial.print(t);
   Serial.print(" *C ");
   Serial.println();
   Serial.print("Agua: ");
   Serial.print(agua);
   Serial.println();
   Serial.print("Agua boolean: ");
   Serial.print(aguabool);
   Serial.println();
   
   sendSigK(tankKey, nivel,sourcetankKey ); //send percent level via UDP
   sendSigK(TemperatureKey, t, sourceTemperatureKey); //send percent level via UDP
   sendSigK(HumidityKey, h, sourceHumidityKey); //send percent level via UDP
   
   
   delay(500);
   
}
