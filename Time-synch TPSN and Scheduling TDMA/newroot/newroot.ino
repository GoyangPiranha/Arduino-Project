#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <stdio.h>
#include <string.h>
/*==========ROOT NODE===========*/
int Angka[5];
char DataKirim[32], Sched2[3];
char DataTerima[32];
char AlamatSendiri[5];
char AlamatTerima[5], LevelCheck[2];
int TimeStampInt, Sched, LevelClear;
char Level[2], TimeStamp1[4], TimeStamp2[4], TimeStamp3[4];
unsigned long Latency, milidetik, detik, menit, jam, milibaru, detikbaru, ubahmili, ubahdetik;
char  milidetik2[4], detik2[3], menit2[3], jam2[3];
long akk, akb;
bool flg = false;
void setup(){
  for (int x=0; x<4; x++){
  AlamatSendiri[x] = '1'; 
  }
  Level[0] = '0';
  Serial.begin(9600);
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init(); 
  Mirf.setRADDR((byte *)AlamatSendiri);  
  Mirf.payload = 32;
  Mirf.channel = 101;
  Mirf.config(); 
  Sched = 0;
}

void loop(){
        if(Mirf.dataReady()){
           Mirf.getData((byte *)&DataTerima);
           LevelCheck[0] = DataTerima[8];
           LevelClear = strtoul(LevelCheck,NULL,0);
            if(DataTerima[0] == 'R' && LevelClear<2){
              waktu();
              sprintf(TimeStamp2,"%d",milidetik);
              TimeStampInt = milidetik;
              synchronize();
            }else if(DataTerima[0] == 'T' /*&& LevelClear<2*/){
              TDMArecv();
            }
   }else{
            if(millis()<7000){
                discovery();
                delay(1000);         
            }else if(milidetik%999==0){
                cetakwaktu();
                delay(3);
            }
         }
    waktu();
}

void discovery(){
        Mirf.setTADDR((byte *)"node");
        strcpy(DataKirim,"B"); //first, adding header character
        strcat(DataKirim,AlamatSendiri); //concat 2 var
        strcat(DataKirim,Level);
        Mirf.send((byte *)&DataKirim);
        while(Mirf.isSending()){
    Serial.println("Paket discovery terkirim!");
                //Serial.println(AlamatSendiri);
                //Serial.println(DataKirim);
                delay(2);
    }
}

void synchronize(){
  TDMAsched();
  for (int x=0; x<4; x++){
  AlamatTerima[x] = DataTerima[x+1]; 
  }
  for (int x=0; x<3; x++){
  TimeStamp1[x] = DataTerima[x+5]; 
  }
  Serial.print("Ada permintaan sinkronisasi dari node ");
  Serial.println(AlamatTerima);
  
  Mirf.setTADDR((byte *)AlamatTerima);//kirim balasan
  kirimwaktu();//timestamp3
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
  Mirf.send((byte *)&DataKirim);
  akb = millis();
  akb = akb-akk;
  Serial.println(akb);
  while(Mirf.isSending()){
      Serial.print("Paket synchronization terkirim!");
      //cetakwaktu();
  }
  
}

void waktu(){

  milidetik=millis()%1000;
    if (!flg)
  {
  milidetik=(millis()%1000) + 100;
      flg = true;
  }
  detik=millis()/1000;
  menit=millis()/60000;
  jam=millis()/1440000;
  akk = millis();
  
  if(detik>59){//reset detik
  detik=detik%60;
  }
  
  if(menit>59){//reset menit
  menit=menit%60;
  }
  
  if(jam>23){//reset jam
  jam=jam%24;
  } 
}

void cetakwaktu(){
  waktu();
  Serial.print(jam);Serial.print(":");
  Serial.print(menit);Serial.print(":");
  Serial.print(detik);Serial.print(":");
  Serial.println(milidetik);
  //delay(1000);
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

void TDMAsched(){
  Sched = random(0,9);
}

void TDMArecv(){
  for (int x=0; x<4; x++){
  AlamatTerima[x] = DataTerima[x+1]; 
  }
  Serial.print("Data diterima dari ");
  Serial.println(AlamatTerima);
//  Serial.print("(");
//  Serial.print(DataTerima[5]);
//  Serial.println(")");
  Serial.println("Nilai Sensor : ");
//  Serial.println(DataTerima[6]);
//  Serial.println(DataTerima[7]);
  Serial.print("R :");
  Serial.print(DataTerima[8]);
  Serial.println(DataTerima[9]);
  Serial.print("G :");
    Serial.print(DataTerima[10]);
  Serial.println(DataTerima[11]);
  Serial.print("B :");
  Serial.print(DataTerima[12]);
  Serial.println(DataTerima[13]);

}
