/* ABT-MLX90614
 *  Written by Antbot *  
 */
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <dht11.h>
#define DHT11PIN 2
dht11 DHT11;

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
int startFlag=0;
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

int wb_H=0;
int wb_L=30;
/////////////////////////////////////////
void setup() {
  int i;
  float wTemp;
  Serial.begin(9600);
  Serial1.begin(2400);
  pinMode (musicpin, OUTPUT);
  pinMode (A0, INPUT);
  pinMode (A1, INPUT);
  pinMode (A2, INPUT);
  pinMode (A3, INPUT);
  pinMode (7, INPUT);
  pinMode (8, INPUT);
  
//  mlx.begin();
  delay(100);
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
  lcd.setCursor(1,0);
  lcd.print("antbot.icoc.in");
  lcd.setCursor(1,1);
  lcd.print("PM25+TEMP+HUMI");
  delay(2000);

}

int frmCnt=0;
int crosPos=0;
int oldVal=0;

void loop() {
    int frameDelay=25;
    float boTemp;
    float sum;
    float pm25;
    char tmbuff[2];
    char disbuff[16];
    String s;        
    char str1[32];
    char str2[32];
    float val_pm25;
    int chk=0;
    int i;
    if(!startFlag)
    {
      startFlag=1;
      lcd.setCursor(1,0);      
      lcd.print("              ");
      lcd.setCursor(1,1);      
      lcd.print("               ");
      }
  //  val_pm25 = calcPM25();//夏普PM2.5传感器     
   // oldVal=(int)val_pm25;   
   // sprintf(str1,"PM2.5:%03d",oldVal);    
   // Serial.println(str1);
    chk = DHT11.read(DHT11PIN);
      delay(200);
  if(chk==DHTLIB_OK)
  {
    dtostrf(DHT11.temperature,2,1,disbuff);
    s = "T:"+String(disbuff)+"*C";
    lcd.setCursor(0,0);
    lcd.print(s);
    dtostrf(DHT11.humidity,2,1,disbuff);
    s = "H:"+String(disbuff)+"%";
    lcd.setCursor(9,0);
    lcd.print(s);
  }
  delay(500);
  pm25=calcPM25();
  sprintf(str2,"PM2.5:%d (oC)    ",(int)pm25);
    Serial.println(str2);
    if((int)pm25>0)
    {
      lcd.setCursor(6,1);      
      lcd.print("        ");
      dtostrf(pm25,3,1,disbuff);
      s = "PM25:"+String(disbuff);
      lcd.setCursor(0,1);
      lcd.print(s);
      if(pm25<35) s=":D";
      else if(pm25<75) s=":)";
      else if(pm25<115) s=":o";
      else if(pm25<150) s=":|";
      else if(pm25<250) s=":(";
      else s=":X";
      lcd.setCursor(14,1);
      lcd.print(s);
      }
    
  delay(500);
  
  
}
float calcPM25()//计算均值，采集5个点，去掉最大最小值，然后求平均数
{
  float val[5],vpm;
  float vmax,vmin,vsum;
  int i,j;
  vsum = 0.0;
  for(i=0;i<5;i++)
  {
    j=0;
    while(1)
    {
      vpm = GetPM25Data_sharp();
      if(vpm>1.0)
      break;
      j++;
      if(j==5)
      return 0.0;
      }
      val[i] = vpm;
      vsum+=vpm;
    }
    vmax = val[0];
    vmin = val[0];
    for(i=0;i<4;i++)
    {
      if(vmax<val[i+1])
      vmax = val[i+1];
      if(vmin>val[i+1])
      vmin = val[i+1];
      }
     vsum-=vmax;
     vsum-=vmin;
     return vsum/3.0; 
 }
float GetPM25Data_sharp()//读取PM2.5传感器,波特率：2400； 校验位：无； 停止位：1 位； 数据位：8；数据包长度为7字节
{
  int cnt,pmval,readcmd[7];
  unsigned char gdata,eFlag,rbytes=0;
  float pm25;
  cnt=0;
   eFlag =0;
  while(Serial1.available()>0)
  //while(Serial.available()>0)
  {
    gdata = Serial1.read();//保存接收字符 
    //gdata = Serial.read();//保存接收字符 
    if(gdata==0xAA&&eFlag==0)
     {
        eFlag=1;        
    }
    if(eFlag==1)
    {
        readcmd[rbytes++]=gdata;
    }    
    delay(2);
    cnt++;
    if(cnt>100)
    return 0;
    if(rbytes==7)//完整帧
    {
      break;
      }   
    }
    if(rbytes==0)
     return 0;
    //if(readcmd[6]!=0xFF)
    // return 0;
  pmval = readcmd[1];
  pmval<<=8;
  pmval+=readcmd[2];
  pm25 = pmval*5.0/1024.0;//计算PM2.5值
  pm25*=800.0;
  if(pm25>999)
  pm25=0;
  return pm25;
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
  float setVal;
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
                
                      if(aimDis<400)
                      aimDis++;
                      else
                      aimDis=0;
                      setVal = aimDis/10.0;
                      dtostrf(setVal,2,1,tmbuff);
                      s = String(tmbuff)+"*C";
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
                      aimDis=400;
                      setVal = aimDis/10.0;
                      dtostrf(setVal,2,1,tmbuff);
                      s = String(tmbuff)+"*C";
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
