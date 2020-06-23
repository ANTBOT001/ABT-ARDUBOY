/* ABT-HC04
 *  Written by Antbot *  
 */
 #include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "RTClib.h"
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LiquidCrystal lcd(6, 10, 9, 14, 15, 16);
//RTC_DS1307 RTC;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnSELECT1 6
#define btnNONE   5
#define BellStart 5


int musicpin=5;
int bellFlag=0;
int soundFlag=0;
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
byte bell5[8] =
{
 B00000,
 B00011,
 B00111,
 B11101,
 B11101,
 B00111,
 B00011,
 B00000
};
byte bell6[8] =
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
int TrigPin = 2; 
int EchoPin = 3; 
int wb_H=0;
int wb_L=30;
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
  pinMode(TrigPin, OUTPUT); 
  pinMode(EchoPin, INPUT); 
  for(i=0;i<8;i++)
  {
    bell3[i] = ~bell1[i];
    bell4[i] = ~bell2[i];
    bell6[i] = ~bell5[i];
    }  
  lcd.createChar(0, bell);
  lcd.createChar(1, bell1);
  lcd.createChar(2, bell2);
  lcd.createChar(3, bell3);
  lcd.createChar(4, bell4);
  lcd.createChar(5, bell5); 
  lcd.createChar(6, bell6);
  lcd.begin(16, 2);
  ////////////////////////////////////  
  wb_H = ReadCfg(0);  
  wb_L = ReadCfg(1);   
  ////////////////////////////////////
  char tmbuff[16];
  mat1 = 1;
  lcd.setCursor(BellStart-3,1);
  lcd.write(mat1);
  mat2 = 2;
  lcd.setCursor(BellStart-2,1);
  lcd.write(mat2);
  mat1 = 6;
  lcd.setCursor(12,1);
  lcd.write(mat1);
  sprintf(tmbuff,"%04dCM",((wb_H<<8)+wb_L));
  String s = String(tmbuff);
   lcd.setCursor(BellStart,1);
   lcd.print(s);
  

}

int frmCnt=0;
int crosPos=0;


void loop() {
    int frameDelay=25;
    float distance;
    char tmbuff[2];
    char disbuff[16];
    String s;    
    char sHour[4];
    char sMin[4];
    char sSec[4];    
    int x;
    // 产生一个10us的高脉冲去触发TrigPin 
        digitalWrite(TrigPin, LOW); 
        delayMicroseconds(2); 
        digitalWrite(TrigPin, HIGH); 
        delayMicroseconds(10);
        digitalWrite(TrigPin, LOW); 
    // 检测脉冲宽度，并计算出距离
        distance = pulseIn(EchoPin, HIGH) / 58.00;
        Serial.print(distance); 
        Serial.print("cm"); 
        Serial.println(); 
     
    sprintf(disbuff,"%04dCM",int(distance));
    s = String(disbuff);
    lcd.setCursor(5,0);
    lcd.print(s);

    if(int(distance)<((wb_H<<8)+wb_L)/2)
    {
      bellFlag=4;          
      }else if(int(distance)<((wb_H<<8)+wb_L))
      {
        bellFlag=2;    
        }else{        
        bellFlag=0; 
        delay(1000);    
        } 
          Warnning(bellFlag);
    //////////////////////////////////
   if(crosPos==0&&getKey()==btnSELECT)
   {
        SetClock();
    }else if(getKey()==btnLEFT)
    {
      soundFlag = 1;
      mat1 = 5;
      lcd.setCursor(12,1);
      lcd.write(mat1);
      }else if(getKey()==btnRIGHT)
    {
      soundFlag = 0;
      mat1 = 6;
      lcd.setCursor(12,1);
      lcd.write(mat1);
      }
  
}
void Warnning(int b)
{
  int i;
  for(i=0;i<b;i++)
  {
    if((i%2)==0)
    {
      mat1 = 3;        
      mat2 = 4;       
      }else{
      mat1 = 1;        
      mat2 = 2;  
        }
       lcd.setCursor(BellStart-3,1);
       lcd.write(mat1); 
       lcd.setCursor(BellStart-2,1);
       lcd.write(mat2); 
       if(soundFlag) 
       {
        digitalWrite(musicpin,(i%2));       
          }
       delay(1000/b);
    }
    digitalWrite(musicpin,0);  
  }
//设置
void SetClock(){
  int setCnt=0;
  char tmbuff[16];
  String s;
  int noact=0;
  int aimDis = ((wb_H<<8)+wb_L);
    crosPos = 1;
        setCnt = 0;
        lcd.setCursor(BellStart+crosPos+2,1);
        lcd.blink();
        for(;;)
        {             
             if(setCnt>3&&getKey()==btnSELECT1)
             {     
                 break;
              }             
             if(getKey()==btnUP)
              {
                noact=0;
                lcd.noBlink();
                
                      if(aimDis<300)
                      aimDis++;
                      else
                      aimDis=0;
                      sprintf(tmbuff,"%04dCM",aimDis);
                      s = String(tmbuff);
                      lcd.setCursor(BellStart+crosPos-1,1);               
                      lcd.print(s);  
                      lcd.setCursor(BellStart+crosPos+2,1);                    
                      lcd.blink();
                }else if(getKey()==btnDOWN)
              {
                noact=0;
                lcd.noBlink();
                      if(aimDis>0)
                      aimDis--;
                      else
                      aimDis=300;
                      sprintf(tmbuff,"%04dCM",aimDis);
                      s = String(tmbuff);
                      lcd.setCursor(BellStart+crosPos-1,1);              
                      lcd.print(s);
                      lcd.setCursor(BellStart+crosPos+2,1);                      
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
          wb_H = aimDis>>8;
          wb_L = aimDis&0xff;
          WriteCfg(0,wb_H); 
          WriteCfg(1,wb_L);
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
