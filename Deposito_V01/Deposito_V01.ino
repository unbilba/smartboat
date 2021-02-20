#include <ArduinoJson.h>
#include <SoftwareSerial.h>

//SoftwareSerial linkSerial(10, 11); // RX, TX

const int OUT_PIN = A4;
const int IN_PIN = A0;
long int acumulado=0;
long int actualizado=0;
long int nivel=0;

char* sensorKey = "tanks.fuel.0.currentLevel";
//char* sensorKey = "propulsion.0.revolutions";

void sendSigK(String sigKey, float data);

void setup(){
   Serial.begin(1200);
   Serial1.begin(1200);
   //linkSerial.begin(600);
   pinMode(OUT_PIN, OUTPUT);
   pinMode(IN_PIN, OUTPUT);
   while (!Serial) continue;
}

void sendSigK(String sigKey, float data)
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
   //thisUpdate["Source"] = "WaterSensors";

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
   sendSigK(sensorKey, nivel); //send percent level via UDP
   delay(500);
   
}
