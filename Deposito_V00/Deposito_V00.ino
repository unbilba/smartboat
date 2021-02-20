const int OUT_PIN = A4;
const int IN_PIN = A0;
long int acumulado=0;
long int actualizado=0;
long int nivel=0;
void setup(){
   Serial.begin(9600);
   Serial1.begin(9600);
   pinMode(OUT_PIN, OUTPUT);
   pinMode(IN_PIN, OUTPUT);
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
   Serial.print("Valor en pF: ");
   Serial.print(actualizado);
   Serial.print(" Nivel:");
   Serial1.print(" Nivel:");
   Serial.print(nivel);
   Serial1.print(nivel);
    Serial.println("%");
   Serial1.println("%");
   
   delay(100);
}
