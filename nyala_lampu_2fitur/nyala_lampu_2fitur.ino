#include <SoftwareSerial.h>
SoftwareSerial SIM900A(7, 8); // RX | TX
// Connect the SIM900A TX to Arduino pin 7 RX.
// Connect the SIM900A RX to Arduino pin 8 TX.
int kondisi = 0;
int pinState = 2;
int relay = 13;
String response;
int lastStringLength = response.length();
void setup()
{
    pinMode(pinState,INPUT);
    pinMode(relay, OUTPUT);
    // start th serial communication with the host computer
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Arduino with SIM900L is ready");
 
    // start communication with the SIM900A in 9600
    SIM900A.begin(9600);  
    SIM900A.println("AT+CMGF=1");
    Serial.println("SIM900A started at 9600");
    delay(1000);
    Serial.println("Setup Complete! SIM900A is Ready!");
    SIM900A.println("AT+CNMI=2,2,0,0,0");
}
 
void loop() {
  kondisi = digitalRead(pinState);
    // Keep reading from Arduino Serial Monitor and send to SIM900A
    if (Serial.available()){
      SIM900A.write(Serial.read());      
    }
    if (SIM900A.available()>0){
      response = SIM900A.readStringUntil('\n');
    }
    if(lastStringLength != response.length()){
      Serial.println(response);
      Serial.println(response.indexOf("ON"));
      if(response.indexOf("ON") == 6){
        digitalWrite(relay,HIGH);
        KirimHidup();
        Serial.println("LAMPU ON");
      }
      if(response.indexOf("OFF") == 6){
        digitalWrite(relay,LOW);
        KirimMati();
        Serial.println("LAMPU OFF");
      }
      if(response.indexOf("LAMPU") == 8){
        if(kondisi==HIGH) {
        KondisiON();     
        Serial.println("KONDISI LAMPU");}
        else if(kondisi==LOW) {
          KondisiOFF();
          Serial.println("KONDISI LAMPU");}   
      }
      lastStringLength = response.length();
   }
}
 
void KirimHidup() {
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  SIM900A.println("AT+CMGS=\"085745160186\"\r"); // Replace x with mobile number
  delay(1000);
  SIM900A.println("Lampu Hidup");// The SMS text you want to send
  delay(100);
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
 
void KirimMati()  {
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  SIM900A.println("AT+CMGS=\"085745160186\"\r"); // Replace x with mobile number
  delay(1000);
  SIM900A.println("Lampu Mati");// The SMS text you want to send
  delay(100);
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

void KondisiON()  {
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  SIM900A.println("AT+CMGS=\"085745160186\"\r"); // Replace x with mobile number
  delay(1000);
  SIM900A.println("Kondisi Lampu ON");// The SMS text you want to send
  delay(100);
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

void KondisiOFF() {
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  SIM900A.println("AT+CMGS=\"085745160186\"\r"); // Replace x with mobile number
  delay(1000);
  SIM900A.println("Kondisi Lampu OFF");// The SMS text you want to send
  delay(100);
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
