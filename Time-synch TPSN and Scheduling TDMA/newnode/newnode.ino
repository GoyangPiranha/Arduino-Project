#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <TimerOne.h>

#define S0     6
#define S1     5
#define S2     4
#define S3     3
#define OUT    2

int   g_count = 0;    // count the frequecy
int   g_array[3];     // store the RGB value
int   g_flag = 0;     // filter of RGB queue
float g_SF[3];        // save the RGB Scale factor
int   R = 0;
int   G = 0;
int   B = 0;


// Init TSC230 and setting Frequency.
void TSC_Init()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);

  digitalWrite(S0, LOW);  // OUTPUT FREQUENCY SCALING 2% x10
  digitalWrite(S1, HIGH);
}

// Select the filter color
void TSC_FilterColor(int Level01, int Level02)
{
  if (Level01 != 0)
    Level01 = HIGH;

  if (Level02 != 0)
    Level02 = HIGH;

  digitalWrite(S2, Level01);
  digitalWrite(S3, Level02);
}

void TSC_Count()
{
  g_count ++ ;
}

void TSC_Callback()
{
  switch (g_flag)
  {
    case 0:
      Serial.println("->WB Start");
      TSC_WB(LOW, LOW);              //Filter without Red
      break;
    case 1:
      //Serial.print("->Frequency R=");
      //Serial.println(g_count);
      g_array[0] = g_count * 10;
      TSC_WB(HIGH, HIGH);            //Filter without Green
      break;
    case 2:
      //Serial.print("->Frequency G=");
      //Serial.println(g_count);
      g_array[1] = g_count * 10;
      TSC_WB(LOW, HIGH);             //Filter without Blue
      break;

    case 3:
      //Serial.print("->Frequency B=");
      //Serial.println(g_count);
      Serial.println("->WB End");
      g_array[2] = g_count * 10;
      TSC_WB(HIGH, LOW);             //Clear(no filter)
      break;
    default:
      g_count = 0;
      break;
  }
}

void TSC_WB(int Level0, int Level1)      //White Balance
{
  g_count = 0;
  g_flag ++;
  TSC_FilterColor(Level0, Level1);
  Timer1.setPeriod(100000);             // set 1s period dari 1000000 diganti 100000
}
char simpanjadwal[12];
int simpanjadwalint[12];
char Sched2[3], Leveltemp[2], jadwal2[3], TDMApacknumb2[2];
char RandNumber[5], LevelCheck[2];
char AlamatSendiri[5];
char sloted[5];
char AlamatParent[5];
char AlamatTerima[32];
char DataTerima[32];
char DataKirim[32];
char LevelKirim[2], TimeStamp1[4], TimeStamp2[4], TimeStamp4[4];
int  ubahmenit, ubahjam, detik,detikx, milidetik, menit, jam, ubahdetik, ubahmili, Level, tunggu, TS1, TS2, TS3, TS4, TimeStampInt;
char buffjam[3], buffmenit[3], buffdetik[3], buffmili[4];
unsigned long Latency, milibaru, detikbaru, menitbaru, jambaru;
int Sched, jadwal, jadwalpatent;
//unsigned long buffjam2, buffmenit2, buffdetik2, buffmili2;
boolean terdaftar, tersinkron;
int x,TDMApacknumb, detiklama, LevelClear;
char  milidetik2[4], detik2[3], menit2[3], jam2[3];

/*==========COMMON NODE===========*/

void setup(){
  TSC_Init();
  Serial.begin(9600);
  Timer1.initialize();             // defaulte is 1s
  Timer1.attachInterrupt(TSC_Callback);
   delay(1000);
  for (int i = 0; i < 3; i++) {
    Serial.println(g_array[i]);
  }
  g_SF[0] = 255.0 / 579.973491 ;    //R Scale factor
  g_SF[1] = 255.0 / 598.707950 ;   //G Scale factor
  g_SF[2] = 255.0 / 819.271348 ;   //B Scale factor
 
  Serial.println(g_SF[0]);
  Serial.println(g_SF[1]);
  Serial.println(g_SF[2]);

  Mirf.spi = &MirfHardwareSpi;
  Mirf.init(); 
  Mirf.setRADDR((byte *)"node");  
  Mirf.payload = 32;
  Mirf.channel = 101;
  Mirf.config();
  Serial.println("Waiting...");
  terdaftar = 0;
  tersinkron = 0;
  tunggu = 20000;
//  while(tunggu%17000 < 2000 || tunggu < 14000){
//    tunggu = random(analogRead(A0),25000);
//  }
  x = 0;
  TDMApacknumb = 0;
  Sched = 0;
}
void loop(){
  R = g_array[0] * g_SF[0];
  G = g_array[1] * g_SF[1];
  B = g_array[2] * g_SF[2];

  if (R > 255) {
    R = 255;
  }
  if (G > 255) {
    G = 255;
  }
  if (B > 255) {
    B = 255;
  }
  if(Mirf.dataReady()){
           Mirf.getData((byte *)&DataTerima);
            LevelCheck[0] = DataTerima[8];
            LevelClear = strtoul(LevelCheck,NULL,0);
            if(!terdaftar && DataTerima[0] == 'B' && DataTerima[1] != 'B'){
              discovered();
              Serial.print("waktu tunggu ");
              Serial.println(tunggu);
              Serial.print("idle...");
            }else if(DataTerima[0] == 'S'){
              waktu();
              TS4 = milidetik;
              detiklama = detik;
              buffering();
              synchronized();
              waktu();
            }else if(DataTerima[0] == 'R' && LevelCheck != LevelKirim && tersinkron){
               //Serial.print("Levelcheck, Levelkirim: ");
               //Serial.print(LevelCheck);
               //Serial.println(LevelKirim);
               waktu();
               sprintf(TimeStamp2,"%d",milidetik);
               TimeStampInt = milidetik;
               answer_sync();
               waktu();
            }else if(DataTerima[0] == 'T' && LevelClear == (Level+1)){
               TDMArecv();
            }
      }else{
        if (!tersinkron && terdaftar && millis()%tunggu==1){
            synchronize();
//            tunggu = random(30000,30399);
              tunggu = 20000;
            delay(1);
        }
        if(tersinkron){
            if(milidetik%999==0){
                //if(x==0 && detik > detikbaru ){ubahdetik=ubahdetik-1;}
                cetakwaktu();
                delay(3);
                x++;
                if(x < 7){
                  discovery();
                }  
            }
            if(menit >= 1)
            {
              detikx=detik%10;
            }
            if(detikx==jadwal && menit >= 1){
                TDMAsend();
               delay(850);
                //jadwal = jadwal + 2;
                if((jadwal-jadwalpatent)>3){jadwal = jadwalpatent;}
            }
         }
      }
   waktu();
}

void discovered(){
  //catat alamat parent
  for (int d=0; d<4; d++){
  AlamatParent[d] = DataTerima[d+1]; 
  }

  Serial.println("Paket Broadcast diterima!");
  Serial.print("Alamat Parent adalah: ");
  Serial.println(AlamatParent);
  
  //catat level
  Serial.println(DataTerima[5]);
  Leveltemp[0] = DataTerima[5];
  Level = strtoul(Leveltemp, NULL, 0);
  Level = Level+1;
  Serial.print("Node bergabung di hierarki level: ");
  Serial.println(Level);
  
  //ubah alamat sendiri
  UbahAlamat();
  Serial.print("Node berubah alamat menjadi: ");
  Serial.println(AlamatSendiri);
  Mirf.setRADDR((byte *)AlamatSendiri);
  
  terdaftar = 1;
}

void UbahAlamat(){
  int Acak = random(analogRead(A0),9999); //generate random number
  while(Acak<1000){
  Acak = random(analogRead(A0),9999);
  }
  sprintf(AlamatSendiri,"%d", Acak);
  }

void synchronize(){
  waktu();
  Mirf.setTADDR((byte *)AlamatParent);
  strcpy(DataKirim,"R");
  strcat(DataKirim,AlamatSendiri);
  sprintf(TimeStamp1,"%d",milidetik);
  if(milidetik<10){
  strcat(DataKirim,"0");
  strcat(DataKirim,"0");
  }else if(milidetik<100){ 
  strcat(DataKirim,"0");
  }
  strcat(DataKirim,TimeStamp1);
  sprintf(LevelKirim,"%d", Level);
  strcat(DataKirim,LevelKirim);
  //strcat(DataKirim,Latency);
  Latency = millis();
  Mirf.send((byte *)&DataKirim);
  while(Mirf.isSending()){
    Serial.println("Request Sinkronisasi terkirim!");
                Serial.println(DataKirim);
                delay(2);
    }
}

void synchronized(){
   Serial.print("Waktu awal: ");
   waktu();
   cetakwaktu();
   Serial.println("");
   waktu();
   Latency = millis() - Latency;
   ubahwaktu();
   waktu();
   cetakwaktu();
   Serial.println("(Tersinkronisasi!))");
   Serial.print("Dari paket: ");
   Serial.println(DataTerima);
   //Serial.print("Delay total: ");
   Serial.print(Latency);
   //Serial.print(", delay propagasi: ");
   Serial.println(TS4);
   //Serial.print("Jadwal: ");
   Serial.println(jadwalpatent);
   Serial.print("Slot: ");
   if(jadwalpatent%2==0)
   {
     Serial.println(jadwalpatent/2);  
   }
   else
   {
     Serial.println((jadwalpatent/2)+1);  
   }
   for(int i=0;i<Level;i++)
   {
     //Serial.print("jadwal terpakai : ");
     Serial.println(simpanjadwalint[i]);
   }
   if (TS4<20 && TS4>0){
   tersinkron = 1;
   }
   //sched();
   //Serial.println(ubahmili);
   //Serial.println(ubahdetik);
}

void waktu(){    
  milidetik = millis()%1000;
  detik = (millis() + ubahmili)/1000;
  milidetik = milidetik + ubahmili;
  detik = detik + ubahdetik;
  menit = detik/60;
  menit = menit + ubahmenit;
  jam=menit/60;
  jam = jam + ubahjam;
  
  if(milidetik>999){//reset milidetik
  milidetik = milidetik%1000; 
  }else if(milidetik<0){
  milidetik = 1000+milidetik; 
  }
  
  if(detik>59){//reset detik
  detik=detik%60;
  }else if(detik<0){
  detik = detik + 60;
  }
  
  if(menit>59){//reset menit
  menit=menit%60;
  }else if(menit<0){
  menit = menit + 60;
     
  if(jam>23){//reset jam
  jam=jam%24;
  }
}
}
 
void cetakwaktu(){
  Serial.print(jam);Serial.print(":");
  Serial.print(menit);Serial.print(":");
  Serial.print(detik);Serial.print(":");    
  Serial.print(milidetik);
  //delay(1000);
}
 
void ubahwaktu(){
  ubahmili = milibaru - milidetik;
  ubahdetik = detikbaru - detiklama;
  ubahmenit = menitbaru - menit;
  ubahjam = jambaru - jam;
  ubahmili = ubahmili + TS4;
}

void discovery(){
        sprintf(LevelKirim,"%d", Level);
        Mirf.setTADDR((byte *)"node");
        strcpy(DataKirim,"B"); //first, adding header character
        strcat(DataKirim,AlamatSendiri); //concat 2 var
        strcat(DataKirim,LevelKirim);
        Mirf.send((byte *)&DataKirim);
        Serial.println("Paket discovery terkirim!");
        while(Mirf.isSending()){
    
                //Serial.println(AlamatSendiri);
                //Serial.println(DataKirim);
                //delay(2);
    }
}

void kirimwaktu(){
  waktu();
  sprintf(milidetik2,"%d", milidetik);
  sprintf(detik2,"%d", detik);
  sprintf(menit2,"%d", menit);
  sprintf(jam2,"%d", jam);
  
  strcpy(DataKirim,"S"); //first, adding header character
  if(jam < 10){strcat(DataKirim,"0");}
  strcat(DataKirim,jam2);
  strcat(DataKirim,":");
  if(menit < 10){strcat(DataKirim,"0");}
  strcat(DataKirim,menit2);
  strcat(DataKirim,":");
  if(detik < 10){strcat(DataKirim,"0");}
  strcat(DataKirim,detik2);
  strcat(DataKirim,":");
  if(milidetik < 10){
  strcat(DataKirim,"0");
  strcat(DataKirim,"0");
  }else if(milidetik < 100){
  strcat(DataKirim,"0");
  }
  strcat(DataKirim,milidetik2);
}

void answer_sync(){
  TDMAsched();
  for (int e=0; e<4; e++){
  AlamatTerima[e] = DataTerima[e+1]; 
  }
  for (int f=0; f<3; f++){
  TimeStamp1[f] = DataTerima[f+5]; 
  }
  Serial.print("Ada permintaan sinkronisasi dari node ");
  Serial.println(AlamatTerima);
  
  Mirf.setTADDR((byte *)AlamatTerima);//kirim balasan
  kirimwaktu();
  strcat(DataKirim,TimeStamp1);//timestamp1
  if(TimeStampInt<10){
  strcat(DataKirim,"0");
  strcat(DataKirim,"0");
  }else if(TimeStampInt<100){
  strcat(DataKirim,"0");
  }
  strcat(DataKirim,TimeStamp2);//timestamp2
  sprintf(Sched2,"%d", Sched);
  strcat(DataKirim,Sched2); 
   Serial.print(Sched2);
   Serial.println("+");
  for(int i=0;i<Level;i++)
  {
   sprintf(Sched2,"%d",simpanjadwalint[i]);
   strcat(DataKirim,Sched2);
   Serial.print(Sched2);
   Serial.println("+");
  }
  Mirf.send((byte *)&DataKirim);
  while(Mirf.isSending()){
      Serial.print("Paket synchronization terkirim pada ");
      cetakwaktu();
  }
}

void buffering(){
               buffjam[0] = DataTerima[1];
               buffjam[1] = DataTerima[2];
               buffmenit[0] = DataTerima[4];
               buffmenit[1] = DataTerima[5];
               buffdetik[0] = DataTerima[7];
               buffdetik[1] = DataTerima[8];
               buffmili[0] = DataTerima[10];
               buffmili[1] = DataTerima[11];
               buffmili[2] = DataTerima[12]; 
               TimeStamp2[0] = DataTerima[16];
               TimeStamp2[1] = DataTerima[17];
               TimeStamp2[2] = DataTerima[18];
               milibaru = strtoul(buffmili,NULL,0);
               detikbaru = strtoul(buffdetik,NULL,0);
               menitbaru = strtoul(buffmenit,NULL,10);
               jambaru = strtoul(buffjam,NULL,10);
               TS1 = strtoul(TimeStamp1,NULL,0);
               TS2 = strtoul(TimeStamp2,NULL,0);
               TS3 = milibaru;
               TS4 = ((TS2 - TS1) + (TS4 - TS3))/2;
               jadwal2[0] =  DataTerima[19];
//               jadwal2[1] =  DataTerima[20];
               jadwal = strtoul(jadwal2,NULL,10);
               jadwalpatent = jadwal;
               for(int i = 0;i<Level;i++)
               {
                jadwal2[0] =  DataTerima[19+i];
                simpanjadwalint[i] = strtoul(jadwal2,NULL,10);
                delay(5); 
               }
}

void TDMAsend(){
  char x[3];
  TDMApacknumb = TDMApacknumb + 1;
  TDMApacknumb = TDMApacknumb%10; 
  sprintf(TDMApacknumb2,"%d", TDMApacknumb);
  Mirf.setTADDR((byte *)"1111");
  strcpy(DataKirim,"T");
  strcat(DataKirim,AlamatSendiri);
  strcat(DataKirim,TDMApacknumb2);
  strcat(DataKirim,"0");
  sprintf(LevelKirim,"%d", Level);
  strcat(DataKirim,LevelKirim);
  sprintf(x,"%d", R);
  strcat(DataKirim,x);
  sprintf(x,"%d", G);
  strcat(DataKirim,x);
  sprintf(x,"%d", B);
  strcat(DataKirim,x);
  Mirf.send((byte *)&DataKirim);
  Serial.println("Data terkirim!");
  //Serial.println(AlamatParent);
}

void TDMArecv(){
  for (int c=0; c<4; c++){
  AlamatTerima[c] = DataTerima[c+1]; 
  }
  Serial.print("Data diterima dari ");
  Serial.print(AlamatTerima);
  Serial.print("(");
  Serial.print(DataTerima[5]);
  Serial.print(")");
  Serial.println();
}

void TDMAsched(){
 boolean cek;
do
{
  cek = false;
  Sched = random(0,10);
    while(Sched>9)
  {
    Sched = random(0,9);  
  }
  for(int i=1;i<Level;i++)
  {
    if(Sched==simpanjadwalint[i]);
    {
      cek = true;
    }
  }
}while(cek);
}
