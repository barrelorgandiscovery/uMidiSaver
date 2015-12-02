

#include "GUI.h"

static GFX *_gfx;

void GUI::InitGFX(GFX *gfx) {
  _gfx = gfx;
}

GFX *GUI::getGFX() {
  return _gfx;
}


  GUI::Scale::Scale(float x1, float y1, float x2, float y2) {
    a = (y2 - y1) / (x2 - x1);
    b = y1 - a * x1; 
  }

  float GUI::Scale::convert(float x) {
    return x * a + b;
  }


// rect
GUI::Rect::Rect(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height) : x(_x),y(_y), height(_height), width(_width)  {

}





bool GUI::Rect::intersect(Rect *another) {
  
  if( (y + height )< another->y )
    return false;

  if( (x + width) < another->x)
    return false;

  if ( x > (another->x + another->width))
    return false;
 
  if ( y > (another->y + another->height))
    return false;
 
 
  return true;
}



///////////////////////////////////////////////////////////////
// messages


GUI::Message::Message(uint8_t _msg, void *_sender) : msg(_msg), sender(_sender) {
  
}

GUI::TouchMessage::TouchMessage( void *_sender, uint16_t _x, uint16_t _y) : Message(TOUCH_MSG,_sender), x(_x), y(_y) {
  
}

GUI::TimerMessage::TimerMessage(long _ticks) : Message(TIMER_MSG, NULL), ticks(_ticks) {
  
}

 ///////////////////////////////////////////////////////////////
 // base widget

 GUI::BaseWidget::BaseWidget(BaseUI *_parent) {
    parent = _parent;
 }

 void GUI::BaseWidget::sendMessage(Message *msg) {
    if (parent != NULL) {
      parent->sendMessage(msg);
    }
 }

 ///////////////////////////////////////////////////////////////
 // button widget


 GUI::Button::Button(BaseUI *_parent, uint16_t _x, uint16_t _y, uint16_t _width,uint16_t _height, char *_str, uint8_t _text_size) : BaseWidget(_parent),
    x(_x),y(_y),width(_width),height(_height),str(_str), text_size(_text_size) {
  
 }

 void GUI::Button::startup() {
    btn = Adafruit_GFX_Button();
    btn.initButton(_gfx, x + width / 2,y + height / 2,width,height,0xFF, 0xFFFF,0x0F, str, text_size);
 }
 
 void GUI::Button::draw() {
    btn.drawButton();
 }

 bool GUI::Button::dispatchMessage(Message *msg) {
  
    if (msg->msg != TOUCH_MSG) {
       return false;
    }

    TouchMessage *tm = (TouchMessage *)msg;
    if (btn.contains(tm->x, tm->y)) {
      // blink
      btn.drawButton(true);
      btn.drawButton(false);
      Message m(BUTTON_PRESSED, this);
      sendMessage(&m);
      return true;
    }

    return false;
 }

 
         


