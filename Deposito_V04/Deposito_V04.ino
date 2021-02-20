#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#include <DFRobot_sim808.h>

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


//SoftwareSerial linkSerial(10, 11); // RX, TX

DFRobot_SIM808 sim808(&Serial1);

char http_cmd[] = "POST /update?api_key=K6YGK0RZEAEWBE2T&field1=5";
char buffer[512];
int i=100;
float lati, longi;

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
   Serial.begin(9600);
   Serial1.begin(9600);
   //linkSerial.begin(600);
   pinMode(OUT_PIN, OUTPUT);
   pinMode(IN_PIN, OUTPUT);
   while (!Serial) continue; 
   dht.begin();

   while(!sim808.init()) {
      delay(1000);
      Serial.print("Sim808 init error\r\n");
  }
  delay(3000);  
   Serial.print("Sim808 iniciado\r\n"); 
  //*********** Attempt DHCP *******************
  while(!sim808.join(F("airtelwap.es"), F("wap@wap"), F("wap125"))) {
      Serial.println("Sim808 join network error");
      delay(2000);
  }
 Serial.print("Conectado a Vodafone\r\n"); 
  //************ Successful DHCP ****************
  Serial.print("IP Address is ");
  Serial.println(sim808.getIPAddress());
  
 //************* Turn on the GPS power************
  if( sim808.attachGPS())
      Serial.println("Open the GPS power success");
  else 
      Serial.println("Open the GPS power failure");
  delay(2000);  

 
 

  //************* Close TCP or UDP connections **********
  //sim808.close();

  //*** Disconnect wireless connection, Close Moving Scene *******
  //sim808.disconnect();
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
   //serializeJson(delta, Serial1);
   Serial1.println();
   //Serial.println();
   
 //}
}
void loop(){
   if (sim808.getGPS()) {
        lati = sim808.GPSdata.lat;
        Serial.println(lati);
        longi = sim808.GPSdata.lon;
        Serial.println(longi);
        //sim808.detachGPS();
    }
    else {
        // No gps, abort
        Serial.println ("No GPS");
        return;
    }
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
   Serial.print("Nivel combustible: ");
   Serial.print(nivel);
   Serial.println();
   
   sendSigK(tankKey, nivel,sourcetankKey ); //send percent level via UDP
   sendSigK(TemperatureKey, t, sourceTemperatureKey); //send percent level via UDP
   sendSigK(HumidityKey, h, sourceHumidityKey); //send percent level via UDP

   
   
   
    //*********** Establish a TCP connection ************
  if(!sim808.connect(TCP,"api.thingspeak.com", 80)) {
      Serial.println("Connect error");
  }else{
      Serial.println("Connect api.thingspeak.com success");
  }
//*********** Send a GET request *****************
 
  Serial.println("waiting to fetch...");

  String lnk;
  lnk="GET /update?api_key=CXU2HVOMLMB3SNB2&long="+String(longi,7)+"&lat="+String(lati,7)+"&field1="+ String(t) +"&field2="+ String(h) +"&field4="+ String(nivel) +" HTTP/1.0\r\n\r\n";
  char http_cmd[(lnk.length())+1];
  lnk.toCharArray(http_cmd,(lnk.length()+1));
  Serial.println(http_cmd);                              // if u want to see the link made uncomment this
  Serial.println(sizeof(http_cmd));                     //size of link made
  sim808.send(http_cmd, sizeof(http_cmd)-1);
  Serial.println(" send... ");
  
  delay (15000);
  
}
