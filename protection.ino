#include <avr/dtostrf.h>
#include <TFT_HX8357_Due.h> // Hardware-specific library
TFT_HX8357_Due tft = TFT_HX8357_Due(); // Invoke custom library
#define TFT_GREY 0x5AEB
// Color Pallette
#define BACKCOLOR 0x0000
#define BARCOLOR 0x0620
#define SCALECOLOR 0xFFFF
//Analog Measurement Declarations
const int analogIn1 = A0;
const int analogIn2 = A7;
float vswr ;
int ref_value = 0;
int ref_LastPercent = 0;
int fwd_value = 0;
int fwd_LastPercent = 0;
char vswr_printout[4];
float old_vswr ;
unsigned long time1;
unsigned long time2;

void setup() {
Serial.begin(9600) ; 
time1 = millis();
tft.begin();
tft.setRotation(1);
tft.fillScreen(BACKCOLOR);
tft.drawRect(2, 2, 476, 316, TFT_WHITE); // Draw bezel line
tft.setTextColor(TFT_WHITE);  // Text colour
tft.drawCentreString("YO8RXP 300W HF 1-30 Mhz AMPLIFIER", 240,10, 4);
tft.drawFastHLine(2, 35, 476, TFT_WHITE);
tft.drawRect(20, 200, 440, 50, TFT_WHITE);



tft.setTextColor(TFT_RED);
tft.drawCentreString("TRANSMITING !", 150,215, 4);
tft.setTextColor(TFT_WHITE);


tft.drawFastHLine(2, 283, 476, TFT_WHITE);
tft.drawCentreString("| PTT | ALARM | ONLINE | 3.5 MHZ |", 240,290, 4);
drawScale(); 
}

void loop(){ 
  
int fwd_newPercent;
int ref_newPercent;
float vswr = (float(fwd_value+ref_value)/(fwd_value-ref_value)) ;
fwd_value = analogRead(analogIn1);
fwd_newPercent = int((fwd_value/1024.0)* 100.0)*4;
ref_value = analogRead(analogIn2);
ref_newPercent = int((ref_value/1024.0)* 100.0)*4;


if (fwd_newPercent != fwd_LastPercent){
drawBar_FWD(fwd_newPercent); 
  }
if (ref_newPercent != ref_LastPercent){
drawBar_REF(ref_newPercent); 
  }

time2 = millis();

if ( (time2 -time1) >= 200  ){
      screen(vswr);
      time1 = millis();
    }

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
     

tft.drawCentreString("FWD", 30,85, 2);  
tft.drawCentreString("REF", 30,125, 2);
tft.fillRect(70,80,400,30,TFT_GREY);
tft.fillRect(70,120,400,30,TFT_GREY);
}


void drawBar_FWD (int fwd_Per){
if(fwd_Per < fwd_LastPercent){
tft.fillRect(70 + fwd_Per, 80 , (fwd_LastPercent-fwd_Per) , 30, TFT_GREY); 
}
else{
tft.fillRect(70 +fwd_LastPercent,80 , (fwd_Per - fwd_LastPercent),30 , TFT_GREEN);
} 
fwd_LastPercent = fwd_Per; 
}

void drawBar_REF (int ref_Per){
if(ref_Per < ref_LastPercent){
tft.fillRect(70 + ref_Per, 120 , (ref_LastPercent-ref_Per) , 30, TFT_GREY); 
}
else{
tft.fillRect(70 +ref_LastPercent,120 , (ref_Per - ref_LastPercent),30 , TFT_GREEN);
} 
ref_LastPercent = ref_Per; 
}

void screen (float test){
String sensorVal = String(test);
sensorVal.toCharArray(vswr_printout, 4);  
tft.fillRect(382,201,50,39,TFT_BLACK);
tft.drawCentreString("SWR =",340,215,4);
tft.drawCentreString(vswr_printout,400,215,4);
}



