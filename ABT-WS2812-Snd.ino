#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LiquidCrystal lcd(6, 10, 9, 14, 15, 16);
//RTC_DS1307 RTC;

#define RGBPIN            2
#define NUMPIXELS         4

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnSELECT1 6
#define btnNONE   5
#define BellStart 5

#define CNT_R  30
#define CNT_G   30
#define CNT_Y   10

int ledType = 0;//0-red;1-yellow;2-green;4-yellow;
int cntLed[4]={CNT_R,CNT_Y,CNT_G,CNT_Y};
int cntVal = 0;
int musicpin=5;
int bellFlag=0;
int soundFlag=0;
unsigned char LedRef[12] = {255,255,0,255,0,0,0,255,0,0,0,255};

int mat1=0;
int mat2=0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, RGBPIN, NEO_GRB + NEO_KHZ800);
int ontime = 300;
int ledMode = 1;
/////////////////////////////////////////
struct bellTime{
    int thour;
    int tmin;
    int tsec;
};
bellTime _bTime;
bellTime _obTime;
unsigned char ledOnFlag = 0;
int wb_H=0;
int wb_L=30;
/////////////////////////////////////////
void setup() {
  int i;
  Serial.begin(9600);
  //Serial1.begin(9600);
  pinMode (musicpin, OUTPUT);
  pinMode (A0, INPUT);
  pinMode (A1, INPUT);
  pinMode (A2, INPUT);
  pinMode (A3, INPUT);
  pinMode (7, INPUT);
  pinMode (8, INPUT);
  pinMode (0, INPUT);
  pinMode (1, INPUT);
  
 lcd.begin(16, 2);
 pixels.begin(); 
 lcd.setCursor(3,1);
        lcd.print("Free mode    ");
 for( i=0;i<NUMPIXELS;i++){        
             pixels.setPixelColor(i, pixels.Color(0,0,0));
             pixels.show(); 
            }
}

int frmCnt=0;
int crosPos=0;


void loop() {
    int i=0;       
    int x;
    
    char tmbuff[4];
   if(ledMode)
   {
       DefPlay(1,cntVal++);
       delay(1000);
    }else{
     if(ledOnFlag==0)
   {
    x = digitalRead(3);
    //Serial.print("io 1 is: ");
    Serial.println("led off!");
    if(x)
    {     
      ledOnFlag = 1;
      for( i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(255,255,200));
        pixels.show(); 
      }
      }
    
    }else{
      Serial.println("led on!");
      cntVal++;
        if(cntVal==ontime)
        {
          ledOnFlag = 0;
          cntVal = 0;
          for( i=0;i<NUMPIXELS;i++){        
             pixels.setPixelColor(i, pixels.Color(0,0,0));
             pixels.show(); 
            }
          }      
      }    
    
     delay(50);
     /////////////////////////////////////////
     //Serial.print("io 0 is: ");
    // Serial.println(digitalRead(0));
         
     ///////////////////////////////////////// 
     if(cntVal%20==0)    
     {
     sprintf(tmbuff,"%02d",cntVal/20);
     lcd.setCursor(3,0);
     lcd.print(tmbuff);
     }
    }
     if(getKey()==btnUP)//声控灯模式
     {
      ledMode = 0;
      cntVal = 0;
      lcd.setCursor(3,1);
       lcd.print("Sound Control");
      }else if(getKey()==btnDOWN)//流水灯模式
      {
        ledMode = 1;
        cntVal = 0;
        lcd.setCursor(3,0);
        lcd.print("   ");
        lcd.setCursor(3,1);
        lcd.print("Free mode    ");
        }
}
void DefPlay(int mode,int cnt)
{
  int i,j;
  if(!mode)//声控灯模式
  {
    for( i=0;i<NUMPIXELS;i++){        
             pixels.setPixelColor(i, pixels.Color(255,255,150));
             pixels.show(); 
            }    
  }else{
    cnt = cnt %4;
    for( i=0;i<NUMPIXELS;i++){ 
             j = (i+cnt)%4;    
             j*=3;   
             pixels.setPixelColor(i, pixels.Color(LedRef[j+0],LedRef[j+1],LedRef[j+2]));
             pixels.show(); 
            }
    }
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
