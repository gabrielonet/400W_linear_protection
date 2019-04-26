#include <TFT_HX8357.h> // Hardware-specific library
TFT_HX8357 tft = TFT_HX8357(); // Invoke custom library
volatile long rotaryCount = 0;
static byte PinA = 0 ;
static byte PinB = 0 ;  
int sw_pin_temp = 1 ; 
#define TFT_GREY 0x5AEB
// Color Pallette
#define BACKCOLOR 0x0000
#define BARCOLOR 0x0620
#define SCALECOLOR 0xFFFF

//Analog Measurement Declarations
const int analogIn1 = A0;
const int analogIn2 = A7;
float vswr = 0;
int ref_value = 0;
int ref_LastPercent = 0;
int fwd_value = 0;
int fwd_LastPercent = 0;
char vswr_printout[4];
float old_vswr ;
unsigned long time1;
unsigned long time2;
int fault_param = 0;
int mode = 0 ;
int sensor =  50 ;

// Interrupt Service Routine
void sw_press()
{
  byte sw_pin = digitalRead (18) ;
  Serial.println(sw_pin);
}

void encoder ()
{
  byte newPinA = digitalRead (2);
  byte newPinB = digitalRead (3);
  
  if (PinA == 0 && PinB == 0 )
   {
     if (newPinA == HIGH && newPinB == LOW )
       { rotaryCount += 1;  Serial.println (rotaryCount); }
     if (newPinA == LOW && newPinB == HIGH )
       { rotaryCount -= 1;  Serial.println (rotaryCount);}  
   }  
 PinA = newPinA;  PinB = newPinB;
}  // end of isr

void setup() {
    Serial.begin(9600) ; 
    time1 = millis();
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(BACKCOLOR);
    tft.drawRect(2, 2, 476, 316, TFT_WHITE); // Draw bezel line
    tft.setTextColor(TFT_WHITE); // Text colour
    tft.drawCentreString("YO8RXP 400W HF 1-30 Mhz AMPLIFIER", 240,10, 4);
    tft.drawFastHLine(2, 35, 476, TFT_WHITE);
    tft.drawRect(20, 200, 440, 50, TFT_WHITE);
    tft.drawCentreString("SWR =",345,215,4);
    tft.setTextColor(TFT_GREEN);
    tft.drawCentreString("RECEIVE !", 150,215, 4);
    tft.setTextColor(TFT_WHITE);
    tft.drawFastHLine(2, 283, 476, TFT_WHITE);
    tft.drawCentreString("1.8     3.5     7     10-14     18-21     24-28     50", 240,290, 4);
    drawScale(); 
    digitalWrite (2, HIGH);   // activate pull-up resistors
    digitalWrite (3, HIGH); 
    digitalWrite (18, HIGH);
    digitalWrite (19, HIGH);
    attachInterrupt (0, encoder, CHANGE);   // pin 2
    attachInterrupt (1, encoder, CHANGE);   // pin 3
    attachInterrupt (5, sw_press, CHANGE);   // pin 18
    attachInterrupt (4, ptt, CHANGE);   // pin 18
}

void drawScale(){ 
    tft.drawFastHLine(70, 60, 400, TFT_WHITE);
    for (int i = 0; i <= 400; i+=20) {
        tft.drawFastVLine(70+i , 60, 15, TFT_WHITE);
    }
    for (int i = 10; i <= 390; i+=20) {
        tft.drawFastVLine(70+i , 60, 7, TFT_WHITE);
    }
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
}

void swr (float test){
    String sensorVal = String(test);
    sensorVal.toCharArray(vswr_printout, 4); 
    if (mode == 1 ) {
        //if (int(vswr) > 1.8 ) {fault(1);} 
        tft.fillRect(380,201,50,39,TFT_BLACK);
        tft.drawCentreString(vswr_printout,400,215,4);
    } else {
        tft.fillRect(380,201,50,39,TFT_BLACK);
        tft.drawCentreString(" N/A",400,215,4);}
        temperature() ;
}

void fault (int fault_param){
    tft.fillScreen(TFT_RED);
    tft.drawCentreString("LAST SWR OVER 1.7 !!!",200,115,4);
    tft.drawCentreString("Enter MENU then select RESET !",200,215,4);
    delay(2000);
}

void ptt(){
  tft.fillRect(70,210,150,30,TFT_BLACK);   
  if (digitalRead (19) == LOW) { tft.setTextColor(TFT_RED);  tft.drawCentreString("TRANSMIT !", 150,215, 4);  tft.setTextColor(TFT_WHITE);  mode = 1 ;}  
  else {tft.setTextColor(TFT_GREEN);  tft.drawCentreString("RECEIVE !", 150,215, 4);  tft.setTextColor(TFT_WHITE);mode = 0  ;}
  } 

void temperature() {
    tft.fillRect(70,150,10,30,TFT_BLACK);  
    tft.drawCentreString("sensor", 75,155, 4); 
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

