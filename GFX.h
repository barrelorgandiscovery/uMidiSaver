
#ifndef _GFX_H
#define _GFX_H


#include "SPFD5408_Adafruit_GFX.h"    // Core graphics library
#include "SPFD5408_Adafruit_TFTLCD.h"


class GFX : public Adafruit_GFX {

private:
  Adafruit_TFTLCD *_tft;

public:
  
  GFX(Adafruit_TFTLCD *tft, int16_t w, int16_t h);
  
   virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
   virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
   virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

 
  
};

#endif
