
#include "GFX.h";

GFX::GFX(Adafruit_TFTLCD *tft, int16_t w, int16_t h): Adafruit_GFX( w,  h) {
     this->_tft = tft;
};

 void GFX::drawPixel(int16_t x, int16_t y, uint16_t color) {
    _tft->drawPixel(x, y, color);
};


 void GFX::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
 _tft->drawFastVLine(x, y, h, color);
  
 }
  void GFX::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color){
 _tft->drawFastHLine(x, y, w, color);
  
 }





 
