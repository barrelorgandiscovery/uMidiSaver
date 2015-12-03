

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

///////////////////////////////////////////////////////////////
// rect struct
namespace GUI {
bool rect_intersect(const Rect * r, const Rect *another) {
  
  if( (r->y + r->height )< another->y )
    return false;

  if( (r->x + r->width) < another->x)
    return false;

  if ( r->x > (another->x + another->width))
    return false;
 
  if ( r->y > (another->y + another->height))
    return false;
 
 
  return true;
}

bool rect_contains(const Rect *rect, uint16_t x,uint16_t y) {
  Rect r = {x,y,0,0};
  return rect_intersect(rect, &r);
}

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

 #define OUTLINE_COLOR 0xFF
 #define  FILL_COLOR  0xFFFF
 #define TEXT_COLOR 0x0F

 GUI::Button::Button(BaseUI *_parent,const GUI::Rect *_rect,char *_str, uint8_t _text_size) : BaseWidget(_parent),
    rect(_rect),str(_str), text_size(_text_size) {
  
 }

 void GUI::Button::startup() {
 }

 void GUI::Button::_draw(bool inverted) {
    
   uint16_t fill, outline, text;
  
     if (! inverted) {
       fill = FILL_COLOR;
       outline = OUTLINE_COLOR;
       text = TEXT_COLOR;
     } else {
       fill =  TEXT_COLOR;
       outline = OUTLINE_COLOR;
       text = FILL_COLOR;
     }
  
     _gfx->fillRoundRect(rect->x , rect->y , rect->width, rect->height, min(rect->width,rect->height)/4, fill);
     _gfx->drawRoundRect(rect->x , rect->y , rect->width, rect->height, min(rect->width,rect->height)/4, outline);
     
     
     _gfx->setCursor(rect->x + rect->width/2 - strlen(str)*3*text_size, rect->y + rect->height/2 -4*text_size);
     _gfx->setTextColor(text);
     _gfx->setTextSize(text_size);
     _gfx->print(str);
   
 }
 
 void GUI::Button::draw() {
    _draw();

 }

 bool GUI::Button::dispatchMessage(Message *msg) {
  
    if (msg->msg != TOUCH_MSG) {
       return false;
    }

    TouchMessage *tm = (TouchMessage *)msg;
    if (rect_contains(rect, tm->x, tm->y)) {
      // blink
      _draw(true);
      _draw(false);
      Message m(BUTTON_PRESSED, this);
      sendMessage(&m);
      return true;
    }

    return false;
 }

 
         


