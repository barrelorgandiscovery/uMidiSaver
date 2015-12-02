#include "UI.h"


#include <ChibiOS_AVR.h>

#include "SPFD5408_Adafruit_GFX.h"
#include "SPFD5408_Adafruit_TFTLCD.h"
#include "SPFD5408_TouchScreen.h"
#include "GFX.h"
#include "GUI.h"
#include "Application.h"

// *** SPFD5408 change -- End

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7


#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

// Original values
//#define TS_MINX 150
//#define TS_MINY 120
//#define TS_MAXX 920
//#define TS_MAXY 940

// Calibrate values
#define TS_MINX 125
#define TS_MINY 85
#define TS_MAXX 965
#define TS_MAXY 905



#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4



  // For better pressure precision, we need to know the resistance
  // between X+ and X- Use any multimeter to read it
  // For the one we're using, its 300 ohms across the X plate
  #define SENSITIVITY 300

  static Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
  static GFX gfx(&tft, 240,320);
  
  static TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSITIVITY);


  #define MINPRESSURE 100 // inital 200-700
  #define MAXPRESSURE 700

  TSPoint getPoint() {
      TSPoint p; 
      p= ts.getPoint(); 
      pinMode(XM, OUTPUT); //Pins configures again for TFT control
      pinMode(YP, OUTPUT);
      return p;
  }

  bool isTouched(TSPoint p) {
      return !((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  }
  
  
  TSPoint waitOneTouch() {
      TSPoint p;
      do {
        p= ts.getPoint();
        pinMode(XM, OUTPUT); //Pins configures again for TFT control
        pinMode(YP, OUTPUT);
         chThdSleepMicroseconds(1000);
      } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
      
  return p;
}


 
  void InitUI() {
       
       tft.begin(0x9341); // SDFP5408
       tft.setRotation(90);
  
  }

  void drawCross(uint16_t x, uint16_t y, uint16_t color) {
      gfx.drawLine(x - 10, y - 10, x + 10, y + 10, color);
      gfx.drawLine(x - 10, y + 10, x + 10, y - 10, color);
  }

 
  void RunUI() {

        GUI::InitGFX(&gfx);
        
        // calibration
        drawCross(10,10, RED);
        TSPoint first = waitOneTouch();
        chThdSleepMilliseconds(1000);
        drawCross(230,310, RED);
        TSPoint second = waitOneTouch();

        GUI::Scale xc = GUI::Scale(first.x,10,  second.x, 230);
        GUI::Scale yc = GUI::Scale(first.y,10, second.y, 310);
        

        long ticks = millis();
        App::mainApp.startup();
   
      while(true) {

        // chThdSleepMicroseconds(10000);
         
        TSPoint t = getPoint();
        if (isTouched(t)) {
          GUI::TouchMessage m( NULL, xc.convert(t.x), yc.convert(t.y)) ;
          App::mainApp.dispatchMessage(&m);
          //gfx.drawPixel(50 +random(100), 50 + random(100),random(100)); // RED
        }

        if (millis() - ticks > 100) {
          GUI::TimerMessage m(millis() - ticks);
          App::mainApp.dispatchMessage(&m);
          ticks = millis();
        }
         
        // gfx.drawPixel(50 +random(100), 50 + random(100),random(100)); // RED
       
      }
  
  }


