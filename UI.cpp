#include "UI.h"


  // For better pressure precision, we need to know the resistance
  // between X+ and X- Use any multimeter to read it
  // For the one we're using, its 300 ohms across the X plate
  

  static Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
  static GFX gfx(&tft, 240,320);
  TouchScreen ts  = TouchScreen(XP, YP, XM, YM, 300);


  #define MINPRESSURE 200 // inital 200-700
  #define MAXPRESSURE 700
  // static TSPoint t;
  TSPoint waitOneTouch() {
  
      TSPoint p;
      
      do {
        p= ts.getPoint(); 
      
        pinMode(XM, OUTPUT); //Pins configures again for TFT control
        pinMode(YP, OUTPUT);
      
      } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
      
  return p;
}


  Adafruit_GFX_Button btn;
  void InitUI() {
       
       
       btn = Adafruit_GFX_Button();
       
       tft.begin(0x9341); // SDFP5408
       tft.setRotation(90);
  
       
  }

  void RunUI() {

      
       // gfx.drawPixel(60,60,RED);
       gfx.drawLine(0,0,320,200,0xFF);

       //Adafruit_GFX_Button *btn = new Adafruit_GFX_Button();
        btn.initButton(&gfx, 100,20,100,10,0xFF, 0xFFFF,0x0F, "Hello", 1);
        btn.drawButton();

      while(true) {

        
/*
       int16_t x = ts.readTouchX();
       if (x < 0) x = 0;
       
       int16_t y = ts.readTouchY();
       if (y < 0) y = 0;

        if (y > 200) y = 200;
        if (x > 200) x = 200;
        */
       
       
        // chThdSleepMicroseconds(10000);
         gfx.drawPixel(50 +random(100), 50 + random(100),RED);
         //noInterrupts();
        TSPoint t = waitOneTouch();
        // interrupts();
       
       
         gfx.drawPixel(t.x, t.y,GREEN);
      }
  
  }


