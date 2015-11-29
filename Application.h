
#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "GUI.h"

namespace App {

  class MainScreen : public GUI::BaseWidget {
    public:
       MainScreen();
       
       virtual void draw();
       // dispatch the message and return if the message is consumed
       virtual bool dispatchMessage(GUI::Message *msg);

   
  };

  
  
  class Application : public GUI::BaseUI {
    public:
       GUI::BaseWidget *current;
       

       // start the application
       virtual void startup();
       
       virtual bool dispatchMessage(GUI::Message *msg);
       virtual void sendMessage(GUI::Message *msg);
    
  };

  extern Application mainApp;
  extern MainScreen mainScreen;

}



#endif
