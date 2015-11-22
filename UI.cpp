#include "UI.h"


  // For better pressure precision, we need to know the resistance
  // between X+ and X- Use any multimeter to read it
  // For the one we're using, its 300 ohms across the X plate
 // static TouchScreen ts;//  = TouchScreen(XP, YP, XM, YM, 300);

  static Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
  static GFX gfx(&tft, 240,320);

  #define MINPRESSURE 200
  #define MAXPRESSURE 700

  /*
  TSPoint waitOneTouch() {
     TSPoint t;
     
      do {
          
              t = ts.getPoint();
           
        } while ( (t.z < MINPRESSURE) || (t.z > MAXPRESSURE));

  
        
        return t;

    
  }
*/
  Adafruit_GFX_Button btn;
  void InitUI() {
       
       
       btn = Adafruit_GFX_Button();
       
       tft.begin(0x9341); // SDFP5408
       tft.setRotation(90);
  
       
  }

  void RunUI() {

       gfx.drawPixel(60,60,RED);
       gfx.drawLine(0,0,320,200,0xFF);

       //Adafruit_GFX_Button *btn = new Adafruit_GFX_Button();
        btn.initButton(&gfx, 100,20,100,10,0xFF, 0xFFFF,0x0F, "Hello", 1);
        btn.drawButton();

      while(true) {

       // p = waitOneTouch();
       gfx.drawPixel(50 +random(100), 50 + random(100),RED);
      }
  
  }


