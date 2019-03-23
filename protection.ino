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
const int analogIn2 = A8;

int ref_value = 0;
int ref_LastPercent = 0;


void setup() {
tft.begin();
tft.setRotation(1);
Serial.begin(9600);
tft.fillScreen(BACKCOLOR);
tft.drawRect(2, 2, 476, 316, TFT_WHITE); // Draw bezel line
tft.setTextColor(TFT_WHITE);  // Text colour
tft.drawCentreString("YO8RXP 300W HF 1-30 Mhz AMPLIFIER", 240,10, 4);
tft.drawFastHLine(2, 35, 476, TFT_WHITE);
tft.drawFastHLine(2, 283, 476, TFT_WHITE);
tft.drawCentreString("| PTT | ALARM | ONLINE | 3.5 MHZ |", 240,290, 4);


//tft.fillRect(100, 10, 30, 280, BARCOLOR);
 // primele 2 sunt originea x si y (pornesc din stanga sus), next value este latimea lui x
 // al patrulea este latimea lui y adica o latime y de 300 incepe de sus s se termina jos

drawScale(); 
}

void loop(){ 
int ref_newPercent;
ref_value = analogRead(analogIn1);
ref_newPercent = int((ref_value/1024.0)* 100.0);

if (ref_newPercent != ref_LastPercent){
drawBar(ref_newPercent); 
Serial.println(ref_newPercent);
Serial.println(ref_LastPercent);

}

}

void drawScale(){ 
tft.drawCentreString("FWD", 30,75, 2);  
tft.drawCentreString("REF", 30,110, 2);
tft.drawFastHLine(70, 80,  400, TFT_GREEN ); // FWD horizontal line
tft.drawFastHLine(70, 120, 400, TFT_GREEN ); // REF horizontal line 

}


void drawBar (int ref_Per){
// primele 2 sunt originea x si y (pornesc din stanga sus), next value este latimea lui x
 // al patrulea este latimea lui y adica o latime y de 300 incepe de sus s se termina jos
 
if(ref_Per < ref_LastPercent){
tft.fillRect(61, 20 + (100-ref_LastPercent), 30, ref_LastPercent - ref_Per, BACKCOLOR); 
delay(60);
}
else{
tft.fillRect(61, 20 + (100-ref_Per), 30, ref_Per - ref_LastPercent, BARCOLOR);
} 
ref_LastPercent = ref_Per; 

}


