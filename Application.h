
#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "GUI.h"

namespace App {

  // used defined messages
  #define APP_MESSAGE_GO_HOME 100
  #define APP_MESSAGE_RECORD 101
  
  

  ////////////////////////////////////////////////////////////////////////
  // spacific widgets

  #define SHOWMIDI_BACKCOLOR 0x0000
  #define SHOWMIDI_BORDER_COLOR 0x07E0
  

  class ShowMidiWidget : public GUI::BaseWidget {
    public:
       ShowMidiWidget(GUI::BaseWidget *parent, GUI::Rect rect);
       virtual void startup();
       virtual void draw();
       // dispatch the message and return if the message is consumed
       virtual bool dispatchMessage(GUI::Message *msg);
       

    private:
       GUI::Rect ext;
       uint16_t current;

  };

  ///////////////////////////////////////////////////////////////////////
  // screens

  // record screen
  class RecordScreen : public GUI::BaseWidget {
    public:
       RecordScreen();
       virtual void startup();
       virtual void draw();
       // dispatch the message and return if the message is consumed
       virtual bool dispatchMessage(GUI::Message *msg);
       virtual void sendMessage(GUI::Message *msg);

  };


  // main screen for the application
  class MainScreen : public GUI::BaseWidget {
    public:
       MainScreen();
       virtual void startup();
       virtual void draw();
       // dispatch the message and return if the message is consumed
       virtual bool dispatchMessage(GUI::Message *msg);
       virtual void sendMessage(GUI::Message *msg);

  };

  
  // top level application
  class Application : public GUI::BaseUI {
    public:
       GUI::BaseWidget *current;

       // start the application
       virtual void startup();
       
       virtual bool dispatchMessage(GUI::Message *msg);
       virtual void sendMessage(GUI::Message *msg);
       void changeCurrent(GUI::BaseWidget *next);
    private:
       void clearScreen();
    
  };

  ///////////////////////////////////////////////////////
  // global instances

  extern Application mainApp;
 
}



#endif
