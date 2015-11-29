
#ifndef _GUI_H
#define _GUI_H

#include "GFX.h"

namespace GUI {
  
  void InitGFX(GFX *gfx);



   class Scale {
        private:
          float a;
          float b;
  
        public:
          Scale(float x1, float y1, float x2, float y2);
          float convert(float x);
    };
    
  

  ////////////////////////////////////////////
  // messages

  // base message
  class Message {
     public:
        Message(uint8_t _msg, void *_sender);
        uint8_t msg;
        void *sender;
  };

  // touch message
  #define TOUCH_MSG 1
  class TouchMessage : public Message {
      public:
        TouchMessage(void *_sender, uint16_t _x, uint16_t _y);
        uint16_t x;
        uint16_t y;
  };


  ///////////////////////////////////////////////////
  // Widget

  class Rect {
    public:
      uint16_t x;
      uint16_t y;
      uint16_t width;
      uint16_t height;

      Rect(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height);

      bool intersect(Rect *another);
    
  };

  ////////////////////////////////////////////////////////////////////////////////////
  // Components

  class BaseUI {

      public:
        // dispatch the message and return if the message is consumed
        virtual bool dispatchMessage(Message *msg) = 0;

        // send a new message
        virtual void sendMessage(Message *msg) = 0;

  };


  // every widget has a dispatchMessage
  class BaseWidget : public BaseUI {
     public:
      BaseWidget(BaseUI *_parent);
      // draw the button
      virtual void draw() = 0;
      // dispatch the message and return if the message is consumed
      virtual bool dispatchMessage(Message *msg) = 0;
      virtual void sendMessage(Message *msg);
    
    protected:
      // every widgets has a parent, for dispatching
      BaseUI *parent;
      
  };

  // wrapper
  class Button : BaseWidget {

      public:
          Button(BaseUI *_parent,uint16_t _x, uint16_t _y, uint16_t _width,uint16_t _height, char *_str);
          virtual void startup();
          virtual void draw();
          virtual bool dispatchMessage(Message *msg);

      private:
          Adafruit_GFX_Button btn;
          uint16_t x;
          uint16_t y; uint16_t width;
          uint16_t height;
           char *str;
          
    
  };


}

#endif
