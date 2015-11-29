
#include "GUI.h"
#include "Application.h"

namespace App {

 App::Application mainApp;
 App::MainScreen mainScreen;

}
 
static GUI::Button record(&App::mainScreen, 150,100,50,40,"Enregistrer");

    
App::MainScreen::MainScreen() :  GUI::BaseWidget(&mainApp) {
  // record = GUI::Button(this, 50,100,100,50,"Enregistrer");
}

  void App::MainScreen::draw() {
   
  record.draw();
 }
 
 // dispatch the message and return if the message is consumed
  bool App::MainScreen::dispatchMessage(GUI::Message *msg) {
    return record.dispatchMessage(msg);
 }



       // start the application
       void App::Application::startup() {
          current = &mainScreen;
           record.startup();
          record.draw();
       }
       
       bool App::Application::dispatchMessage(GUI::Message *msg) {
         return current->dispatchMessage(msg);
       }
       
       void App::Application::sendMessage(GUI::Message *msg) {

        
       }




