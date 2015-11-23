#include "UI.h"


  // For better pressure precision, we need to know the resistance
  // between X+ and X- Use any multimeter to read it
  // For the one we're using, its 300 ohms across the X plate
  #define SENSITIVITY 300

  static Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
  static GFX gfx(&tft, 240,320);
  TouchScreen ts  = TouchScreen(XP, YP, XM, YM, SENSITIVITY);


  #define MINPRESSURE 100 // inital 200-700
  #define MAXPRESSURE 700
  
  
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


  Adafruit_GFX_Button btn;
  void InitUI() {
       
       
       btn = Adafruit_GFX_Button();
       
       tft.begin(0x9341); // SDFP5408
       tft.setRotation(90);
  
       
  }

  void drawCross(uint16_t x, uint16_t y, uint16_t color) {
      gfx.drawLine(x - 10, y - 10, x + 10, y + 10, color);
      gfx.drawLine(x - 10, y + 10, x + 10, y - 10, color);
  }

  Scale::Scale(float x1, float y1, float x2, float y2) {
    a = (y2 - y1) / (x2 - x1);
    b = y1 - a * x1; 
  }

  float Scale::convert(float x) {
    return x * a + b;
  }

  void RunUI() {

        // calibration
        drawCross(10,10, RED);
        TSPoint first = waitOneTouch();
        chThdSleepMilliseconds(3000);
        drawCross(230,310, RED);
        TSPoint second = waitOneTouch();

        Scale xc = Scale(first.x,10,  second.x, 230);
        Scale yc = Scale(first.y,10, second.y, 310);
        

      
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
       
       
         gfx.drawPixel(xc.convert(t.x), yc.convert(t.y),GREEN);
      }
  
  }


