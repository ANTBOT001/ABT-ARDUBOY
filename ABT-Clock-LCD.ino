/* ABT-Clock
 *  Written by Antbot *  
 */
 #include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "RTClib.h"
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LiquidCrystal lcd(6, 10, 9, 14, 15, 16);
RTC_DS1307 RTC;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnSELECT1 6
#define btnNONE   5
#define BellStart 6

#define NOTE_D0 -1
#define NOTE_D1 294
#define NOTE_D2 330
#define NOTE_D3 350
#define NOTE_D4 393
#define NOTE_D5 441
#define NOTE_D6 495
#define NOTE_D7 556

#define NOTE_DL1 147
#define NOTE_DL2 165
#define NOTE_DL3 175
#define NOTE_DL4 196
#define NOTE_DL5 221
#define NOTE_DL6 248
#define NOTE_DL7 278

#define NOTE_DH1 589
#define NOTE_DH2 661
#define NOTE_DH3 700
#define NOTE_DH4 786
#define NOTE_DH5 882
#define NOTE_DH6 990
#define NOTE_DH7 112

#define WHOLE 1
#define HALF 0.5
#define QUARTER 0.25
#define EIGHTH 0.25
#define SIXTEENTH 0.625
int tune[] =
{
  NOTE_DH1,NOTE_D6,NOTE_D5,NOTE_D6,NOTE_D0,
  NOTE_DH1,NOTE_D6,NOTE_D5,NOTE_DH1,NOTE_D6
};
float duration[]=
{
  1,1,0.5,0.5,1,
  0.5,0.5,0.5,0.5,1
};
int musicpin=5;
int bellFlag=0;
byte bell[8] =
{
 B00010,
 B00111,
 B00111,
 B00110,
 B00111,
 B10110,
 B11110,
 B01010
};
byte bell1[8] =
{
 B00001,
 B00011,
 B00100,
 B01000,
 B01000,
 B01000,
 B11111,
 B00001
};

byte bell2[8] =
{
 B10000,
 B11000,
 B00100,
 B00010,
 B00010,
 B00010,
 B11111,
 B10000
};
byte bell3[8] =
{
 B00001,
 B00010,
 B00100,
 B01000,
 B10000,
 B11000,
 B00111,
 B00000
};
byte bell4[8] =
{
 B00001,
 B00010,
 B00100,
 B01000,
 B10000,
 B11000,
 B00111,
 B00000
};
byte freep[8] =
{
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000
};

int mat1=0;
int mat2=0;
/////////////////////////////////////////
struct bellTime{
    int thour;
    int tmin;
    int tsec;
};
bellTime _bTime;
bellTime _obTime;
/////////////////////////////////////////
void setup() {
  int i;
  Serial.begin(9600);
  pinMode (musicpin, OUTPUT);
  pinMode (A0, INPUT);
  pinMode (A1, INPUT);
  pinMode (A2, INPUT);
  pinMode (A3, INPUT);
  pinMode (7, INPUT);
  pinMode (8, INPUT);
  for(i=0;i<8;i++)
  {
    bell3[i] = ~bell1[i];
    bell4[i] = ~bell2[i];
    }  
  lcd.createChar(0, bell);
  lcd.createChar(1, bell1);
  lcd.createChar(2, bell2);
  lcd.createChar(3, bell3);
  lcd.createChar(4, bell4);
  lcd.createChar(5, freep); 
  lcd.begin(16, 2);
  ////////////////////////////////////
  Wire.begin();
    RTC.begin();
//RTC.adjust(DateTime(__DATE__, __TIME__));//用于校准时间，与电脑时间同步，取消注释然后上传即可同步；如果单独供电的话别忘了再加上注释重新上传
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  ////////////////////////////////////
  _bTime.thour = ReadCfg(0);
  if(_bTime.thour>23)
  _bTime.thour=0;
  _bTime.tmin = ReadCfg(1); 
  if(_bTime.tmin>59)
  _bTime.thour=0;
  ////////////////////////////////////
  char tmbuff[16];
  mat1 = 1;
  lcd.setCursor(BellStart-4,1);
  lcd.write(mat1);
  mat2 = 2;
  lcd.setCursor(BellStart-3,1);
  lcd.write(mat2);
  sprintf(tmbuff,"%02d:%02d",_bTime.thour,_bTime.tmin);
  String s = String(tmbuff);
   lcd.setCursor(BellStart,1);
   lcd.print(s);
  

}

int frmCnt=0;
int crosPos=0;


void loop() {
    int frameDelay=25;
    char tmbuff[2];
    String s;    
    char sHour[4];
    char sMin[4];
    char sSec[4];    
    int x;
    if(bellFlag>0)
    {
      bellFlag++;
      if(bellFlag<6)
      {
        if(bellFlag%2==1)
        {
           mat1 = 1;
           mat2 = 2;
          }else{
           mat1 = 3;
           mat2 = 4;
            }            
        lcd.setCursor(BellStart-4,1);
        lcd.write(mat1);      
        lcd.setCursor(BellStart-3,1);
        lcd.write(mat2);
           /////////////////
         for(x=0;x<10;x++)//循环音符的次数
        {
          tone(musicpin,tune[x]);//此函数依次播放tune序列里的数组，即每个音符
          delay(400*duration[x]);//每个音符持续的时间，即节拍duration，400是调整时间的越大，曲子速度越慢，越小曲子速度越快，自己掌握吧
          noTone(musicpin);//停止当前音符，进入下一音符
        }
           ////////////////
        }
      if(bellFlag>70)
      bellFlag=0;
      }
    DateTime nowto = RTC.now();
    Serial.print(nowto.hour(), DEC);
    Serial.print(':');
    Serial.print(nowto.minute(), DEC);
    Serial.print(':');
    Serial.print(nowto.second(), DEC);
    Serial.println();
    
    if(_bTime.thour==nowto.hour()&&_bTime.tmin==nowto.minute()&&bellFlag==0)
    {
      bellFlag =1;
      }
    sprintf(sHour,"%02d",nowto.hour());
    sprintf(sMin,"%02d",nowto.minute());
    sprintf(sSec,"%02d",nowto.second());
    s = String(sHour)+":"+String(sMin)+":"+String(sSec);
    lcd.setCursor(4,0);
    lcd.print(s);
    //////////////////////////////////
   if(crosPos==0&&getKey()==btnSELECT)
   {
        SetClock();
    }
    delay(1000); 
}
void BellSing()
{
  
  }
//设置闹铃
void SetClock(){
  int setCnt=0;
  char tmbuff[3];
  String s;
  int noact=0;
    crosPos = 1;
        setCnt = 0;
        lcd.setCursor(BellStart+crosPos-1,1);
        lcd.blink();
        for(;;)
        {             
             if(setCnt>3&&getKey()==btnSELECT1)
             {     
                 break;
              }
             
             if(getKey()==btnRIGHT&&crosPos<5)
             {
                  noact=0;
                  lcd.noBlink();
                  crosPos++;
                  lcd.setCursor(BellStart+crosPos-1,1);
                  lcd.blink();
              }else if(getKey()==btnLEFT&&crosPos>0)
             {
                 noact=0;
                  lcd.noBlink();
                  crosPos--;
                  lcd.setCursor(BellStart+crosPos-1,1);
                  lcd.blink();
              }else if(getKey()==btnUP)
              {
                noact=0;
                lcd.noBlink();
                if(crosPos>2)
                {
                      if(_bTime.tmin<59)
                      _bTime.tmin++;
                      else
                      _bTime.tmin=0;
                      sprintf(tmbuff,"%02d",_bTime.tmin);
                      s = String(tmbuff);
                      lcd.setCursor(BellStart+3,1);   
                  }else{
                    if(_bTime.thour<23)
                      _bTime.thour++;
                      else
                      _bTime.thour=0;
                      sprintf(tmbuff,"%02d",_bTime.thour);
                      s = String(tmbuff);
                      lcd.setCursor(BellStart,1);                      
                    }
                      
                      lcd.print(s);
                      lcd.setCursor(BellStart+crosPos-1,1); 
                      lcd.blink();
                }else if(getKey()==btnDOWN)
              {
                noact=0;
                lcd.noBlink();
                if(crosPos>2)
                {
                      if(_bTime.tmin>0)
                      _bTime.tmin--;
                      else
                      _bTime.tmin=59;
                      sprintf(tmbuff,"%02d",_bTime.tmin);
                      s = String(tmbuff);
                      lcd.setCursor(BellStart+3,1);   
                  }else{
                    if(_bTime.thour>0)
                      _bTime.thour--;
                      else
                      _bTime.thour=23;
                      sprintf(tmbuff,"%02d",_bTime.thour);
                      s = String(tmbuff);
                      lcd.setCursor(BellStart,1);                      
                    }
                      
                      lcd.print(s);
                      lcd.setCursor(BellStart+crosPos-1,1); 
                      lcd.blink();
                }else if(getKey()==btnNONE)
                {
                  noact++;
                  if(noact>50)
                  break;
                  
                  }
              delay(200);
              setCnt++;
          }
          //RTC.begin();
          crosPos = 0;
          lcd.noBlink();
          WriteCfg(0,_bTime.thour); 
          WriteCfg(1,_bTime.tmin);
          Serial.println("set ok!");

}

int getKey() {
   if(!digitalRead(A0))
   {
    delay(10);
    if(!digitalRead(A0))
       return btnUP;
    }
   if(!digitalRead(A1))
   {
    delay(10);
    if(!digitalRead(A1))
    return btnRIGHT;  
    }
   if(!digitalRead(A3)){ 
    delay(10);
    if(!digitalRead(A3))
    return btnDOWN;
    }
   if(!digitalRead(A2)){
    delay(10);
    if(!digitalRead(A2))
     return btnLEFT;
     }
   if(!digitalRead(7)) {
    delay(10);
    if(!digitalRead(7))
    return btnSELECT;  
    } 
    if(!digitalRead(8)) {
    delay(10);
    if(!digitalRead(8))
    return btnSELECT1;  
    } 
  return btnNONE;
}
unsigned char ReadCfg(unsigned char addr)
 {
      unsigned char val;
      val =  EEPROM.read(addr);      
      return val;
      
  }
  void WriteCfg(unsigned char addr,unsigned char val)
 {
      EEPROM.write(addr, val);
  }
