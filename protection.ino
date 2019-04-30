#include <TFT_HX8357.h> // Hardware-specific library
#include <OneWire.h> 
#include <EEPROM.h>

#define TFT_GREY 0x5AEB
#define BACKCOLOR 0x0000
#define BARCOLOR 0x0620
#define SCALECOLOR 0xFFFF
TFT_HX8357 tft = TFT_HX8357(); // Invoke custom library

int store_address = 0;
volatile long rotaryCount = 0;
static byte PinA = 0 ;
static byte PinB = 0 ;  
int sw_pin_temp = 1 ; 
const int analogIn1 = A0;
const int analogIn2 = A7;
float vswr = 0;
int ref_value = 0;
int ref_LastPercent = 0;
int fwd_value = 0;
int fwd_LastPercent = 0;
char vswr_printout[4];
char sensor1[3];
char fan_spd[3];
float old_vswr ;
unsigned long time1;
unsigned long time2;
int fault_param = 0;
String error = "0" ; 
int mode = 0 ;
int sensor =  20 ;
float fan_speed = 0 ; 
int band = EEPROM.read(0); 
char band1[10];
int freq_temp = 0;

int16_t dallas(int x,byte start) 
{
  OneWire ds(x); 
  byte i; 
  byte data[2]; 
  int16_t result; 
  do
{
  ds.reset();
  ds.write(0xCC); // Skip Command 
  ds.write(0xBE); // Read 1st 2 bytes 
  for ( i = 0; i < 2; i++) data[i] = ds.read(); 
  result=(data[1]<<8) | data[0]; 
  result>>=4; if (data[1]&128) result|=61440; 
  if (data[0]&8) ++result; 
  ds.reset(); 
  ds.write(0xCC); // skip Command 
  ds.write(0x44,1); // start conversion
  if (start) delay(1000); 
  } while (start--); 
  return result;
}

void sw_press(){ byte sw_pin = digitalRead (18) ;} // nothing  to do here yet 

void encoder (){
  byte newPinA = digitalRead (21); byte newPinB = digitalRead (20);
  if (PinA == 1 && PinB == 1 )
   {
     if (newPinA == HIGH && newPinB == LOW ) { band -= 1; }
     if (newPinA == LOW && newPinB == HIGH ) { band += 1; }  
   }  
  PinA = newPinA;  PinB = newPinB;
  if (band  < 1){band = 7;}
  if (band > 7){band = 1;}
  bandpass_filters(band) ;
  EEPROM.write(0, band);
}  

void setup() {
    //Serial.begin(9600) ; 
    time1 = millis();
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(BACKCOLOR);
    tft.drawRect(2, 2, 476, 316, TFT_WHITE); // Draw bezel line
    tft.setTextColor(TFT_WHITE); // Text colour
    tft.drawCentreString("YO8RXP 400W HF 1-30 Mhz AMPLIFIER", 240,10, 4);
    tft.drawFastHLine(2, 35, 476, TFT_WHITE);
    tft.drawRect(20, 130, 440, 80, TFT_GREEN); tft.drawFastHLine(20, 160, 440, TFT_WHITE);
    tft.drawCentreString("Temperature        |        Fan Speed ",240,135,4);tft.drawCentreString("%",400,175, 4);    
    tft.drawRect(20, 220, 440, 50, TFT_GREEN);
    tft.drawCentreString("SWR =",345,235,4);
    tft.setTextColor(TFT_GREEN);
    tft.drawCentreString("RECEIVE !", 150,235, 4);
    tft.setTextColor(TFT_WHITE);
    tft.drawFastHLine(2, 283, 476, TFT_WHITE);
    tft.drawString("Filters  Freq  Set  On                         Mhz ", 10,290, 4);
    drawScale(); 
    digitalWrite (20, HIGH);   //  Encoder intrerupt pin 3
    digitalWrite (21, HIGH);   //  Encoder intrerupt pin 3 
    digitalWrite (18, HIGH);   //  PTT inrerupt pin 4
    digitalWrite (19, HIGH);   //  Encoder SW intrerupt pin 5
    pinMode(0, OUTPUT) ; //   just to disable TX0
    pinMode(1, OUTPUT) ; //   1.8 Mhz bandpass selection
    pinMode(2, OUTPUT) ; //   3.5 Mhz bandpass selection
    pinMode(3, OUTPUT) ; //     7 Mhz bandpass selection
    pinMode(4, OUTPUT) ; // 10-14 Mhz bandpass selection
    pinMode(5, OUTPUT) ; // 18-21 Mhz bandpass selection
    pinMode(6, OUTPUT) ; // 24-28 Mhz bandpass selection
    pinMode(7, OUTPUT) ; //    50 Mhz bandpass selection
    pinMode(8, OUTPUT) ; //    PTT output low power relay  - receive LOW , transmit HIGH 
    digitalWrite(8, LOW) ; // Set relay  on receive
    pinMode(10, OUTPUT) ; //    PTT output High Power relay  - receive LOW , transmit HIGH 
    digitalWrite(10, LOW) ; // Set relay on receive
    pinMode(9, OUTPUT) ;       // PWM FAN Controll
    attachInterrupt (2, encoder, CHANGE);   // pin 20
    attachInterrupt (3, encoder, CHANGE);   // pin 21
    attachInterrupt (5, sw_press, CHANGE);   // pin 18
    attachInterrupt (4, ptt, CHANGE);   // pin 19
    dallas(A4,1); 
    //relay_zero(); // put all relays to off
    bandpass_filters(band);
}

void drawScale(){ 
    tft.drawFastHLine(70, 60, 400, TFT_WHITE);
    for (int i = 0;  i <= 400; i+=20) {tft.drawFastVLine(70+i , 60, 15, TFT_WHITE); }
    for (int i = 10; i <= 390; i+=20) { tft.drawFastVLine(70+i , 60, 7, TFT_WHITE); }
    tft.drawCentreString("0", 70,40, 2); 
    tft.drawCentreString("100", 170,40, 2); 
    tft.drawCentreString("200", 270,40, 2); 
    tft.drawCentreString("300", 370,40, 2); 
    tft.drawCentreString("400", 460,40, 2); 
    tft.drawCentreString("FWD", 30,77, 2); 
    tft.drawCentreString("REF", 30,100, 2);
    tft.fillRect(70,80,400,15,TFT_GREY);
    tft.fillRect(70,100,400,15,TFT_GREY);
    }

void drawBar_FWD (int fwd_Per){
    if(fwd_Per < fwd_LastPercent){
        tft.fillRect(70 + fwd_Per, 80 , (fwd_LastPercent-fwd_Per) , 15, TFT_GREY); 
    } else{ tft.fillRect(70 +fwd_LastPercent,80 , (fwd_Per - fwd_LastPercent),15 , TFT_GREEN);  } 
    fwd_LastPercent = fwd_Per; 
}
void drawBar_REF (int ref_Per){
    if(ref_Per < ref_LastPercent){
        tft.fillRect(70 + ref_Per, 100 , (ref_LastPercent-ref_Per) , 15, TFT_GREY); 
    } else{ tft.fillRect(70 +ref_LastPercent,100 , (ref_Per - ref_LastPercent),15 ,TFT_RED); } 
    ref_LastPercent = ref_Per;
    if (ref_Per > 10){fault(); }
}

void swr (float test){
    String sensorVal = String(test);
    sensorVal.toCharArray(vswr_printout, 4); 
    if (mode == 1 ) {
        if (int(vswr) >= 2  ) {fault();} 
        tft.fillRect(380,221,50,39,TFT_BLACK);
        tft.drawCentreString(vswr_printout,400,235,4);
    } else {
        tft.fillRect(380,221,50,39,TFT_BLACK);
        tft.drawCentreString(" N/A",400,235,4);}
        temperature() ;
}

void(* resetFunc) (void) = 0;//declare reset function at address 0
void fault (){
    error = "1";
    digitalWrite(8, LOW);
    tft.fillScreen(TFT_RED);
    tft.drawCentreString("LAST SWR OVER 2.0 !!!",200,115,4);
    tft.drawCentreString("Wait 5 seconds for auto  RESET !",200,215,4);
    delay(5000) ;
    resetFunc() ;
}

void ptt(){
  
  if(error == "0"){
        tft.fillRect(70,230,150,30,TFT_BLACK);   
    if (digitalRead (19) == LOW) { 
      tft.setTextColor(TFT_RED);  tft.drawCentreString("TRANSMIT !", 150,235, 4);
      tft.setTextColor(TFT_WHITE);  mode = 1 ;
      long start = millis();
      digitalWrite(10, HIGH);
      delay(10000);
      digitalWrite(8, HIGH);
       }  
    else {
      tft.setTextColor(TFT_GREEN);  
      tft.drawCentreString("RECEIVE !", 150,235, 4);  
      tft.setTextColor(TFT_WHITE);mode = 0  ;
      digitalWrite(8, LOW);
      delay(1000);
      digitalWrite(10, LOW);
      }
  } 
}

  
void fan (int speed) {
    if (speed < 20 ) { speed = 20 ; }
    if (speed > 100 ) { speed = 100 ; }
    String fan_pwm = String(speed);fan_pwm.toCharArray(fan_spd, 4);
    tft.fillRect(340,170,40,30,TFT_BLACK); tft.drawCentreString(fan_spd, 360,175, 4); 
    analogWrite(9 ,speed*2.55) ; 
    }
  
void temperature() {
    sensor = dallas(A4,0) ;
    int sensor_tmp = 0;
    if (sensor != sensor_tmp){
      tft.fillRect(25,170,150,30,TFT_BLACK);  
      String probe = String(sensor);probe.toCharArray(sensor1, 4);   tft.drawCentreString("*C", 140,175, 4);   
      tft.drawCentreString(sensor1, 100,175, 4);  
      fan_speed = int((sensor - 40)*2.5) ;  // set fan 25% at 50 degrees
      fan(fan_speed) ; 
      sensor_tmp = sensor ;
    }
  }    

void relay_zero() {
  //digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
}

void bandpass_filters(int freq){
  if (freq != freq_temp ){
    relay_zero() ;
    tft.fillRect(250,286,80,28,TFT_WHITE); tft.setTextColor(TFT_BLACK);
    if (freq == 1)   {String bnd = "1.8"; bnd.toCharArray(band1, 10);   digitalWrite(1, HIGH);}
    if (freq == 2) {  String bnd = "3.5"; bnd.toCharArray(band1, 10);   digitalWrite(2, HIGH);}
    if (freq == 3) {    String bnd = "7"; bnd.toCharArray(band1, 10);   digitalWrite(3, HIGH);}
    if (freq == 4) {String bnd = "10-14"; bnd.toCharArray(band1, 10);   digitalWrite(4, HIGH);}
    if (freq == 5) {String bnd = "18-21"; bnd.toCharArray(band1, 10);   digitalWrite(5, HIGH);}
    if (freq == 6) {String bnd = "24-28"; bnd.toCharArray(band1, 10);   digitalWrite(6, HIGH);}
    if (freq == 7) {   String bnd = "50"; bnd.toCharArray(band1, 10);   digitalWrite(7, HIGH);}
    tft.drawCentreString(band1, 290,290, 4);   
    tft.setTextColor(TFT_WHITE);
    freq_temp = freq;
    //Serial.println(band1);
  }
}
void loop(){
    int fwd_newPercent;
    int ref_newPercent;
    fwd_value = analogRead(analogIn1);
    ref_value = analogRead(analogIn2);
    fwd_newPercent = int((fwd_value/1024.0)* 100.0)*4;
    ref_newPercent = int((ref_value/1024.0)* 100.0)*4;
    if (fwd_newPercent >= 2 ){ vswr = (float(fwd_value+ref_value)/(fwd_value-ref_value)) ; } else { vswr = 1; }
    if (fwd_newPercent != fwd_LastPercent){ drawBar_FWD(fwd_newPercent);   }  
    if (ref_newPercent != ref_LastPercent){ drawBar_REF(ref_newPercent);   }
    time2 = millis();
    if ( (time2 -time1) >= 200 ){swr(vswr);time1 = millis();}
    
}

