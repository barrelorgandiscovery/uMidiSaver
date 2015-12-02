
#include "GUI.h"
#include "Application.h"
#include "SSMidi.h"

namespace App {

 // global instances
 App::Application mainApp;
 App::MainScreen mainScreen;
 App::RecordScreen recordScreen;

}

///////////////////////////////////////////////////////////////////////////////////
// Specific Widgets


App::ShowMidiWidget::ShowMidiWidget(GUI::BaseWidget *_parent, GUI::Rect _rect) : GUI::BaseWidget(_parent), ext(_rect), current(0) {
  
}
void App::ShowMidiWidget::startup() {
  // saveBuffer[3] = 10;
  
}


void App::ShowMidiWidget::draw() {
   GFX *gfx = GUI::getGFX();
   gfx->fillRect(ext.x, ext.y, ext.width, ext.height, SHOWMIDI_BACKCOLOR);
  
}

bool App::ShowMidiWidget::dispatchMessage(GUI::Message *msg) {
  if (msg->msg == TIMER_MSG) {
    current++;
    if (current >= ext.width)
    {
      current = 0;
      draw(); // clean the widget
    }
    GFX *gfx = GUI::getGFX();
    gfx->drawPixel(current + ext.x, ext.y, GREEN);
    gfx->drawPixel(current + ext.x, ext.y + ext.height - 1, GREEN);
    
  }
  
}
       

///////////////////////////////////////////////////////////////////////////////////
// Record screen

static GUI::Button stop(&App::recordScreen, 1,270,238,50,"Stop",3);
static App::ShowMidiWidget midiWidget(&App::recordScreen, GUI::Rect(0,0,240,240));

App::RecordScreen::RecordScreen() : GUI::BaseWidget(&mainApp) {
  
}

void App::RecordScreen::startup() {
    stop.startup();
    midiWidget.startup();
}


void App::RecordScreen::draw() {
  stop.draw();
}

 // dispatch the message and return if the message is consumed
bool App::RecordScreen::dispatchMessage(GUI::Message *msg) {
  bool b = stop.dispatchMessage(msg);
  b |= midiWidget.dispatchMessage(msg);
}

void App::RecordScreen::sendMessage(GUI::Message *msg) {
  if (msg->msg == BUTTON_PRESSED && msg->sender == &stop){
    GUI::Message m(APP_MESSAGE_GO_HOME, this);
    GUI::BaseWidget::sendMessage(&m);    
  }
}


///////////////////////////////////////////////////////////////////////////////////
// Main Screen

// components used for the screens
static GUI::Button record(&App::mainScreen, 1,1,238,50,"Enregistrer",3);


App::MainScreen::MainScreen() : GUI::BaseWidget(&mainApp) { // reference the main app as parent
}

void App::MainScreen::startup() {
  record.startup();
}

void App::MainScreen::draw() {
  record.draw();
}

 // dispatch the message and return if the message is consumed
bool App::MainScreen::dispatchMessage(GUI::Message *msg) {
  return record.dispatchMessage(msg);
}

void App::MainScreen::sendMessage(GUI::Message *msg) {
 Serial.print(F("main screen message"));
  if (msg->msg == BUTTON_PRESSED && msg->sender == &record){
    
    GUI::Message m(APP_MESSAGE_RECORD, this);
    GUI::BaseWidget::sendMessage(&m);    
  }
}

///////////////////////////////////////////////////////////////////////////////////
// Main application


       // start the application
void App::Application::startup() {
    clearScreen();

    // start all the screens
   
    mainScreen.startup();
    recordScreen.startup();
    
    current = &mainScreen;
    current->draw();
}

       
bool App::Application::dispatchMessage(GUI::Message *msg) {
    // reroute the messages to the current view
    return current->dispatchMessage(msg);
}

void App::Application::clearScreen() {
  GUI::getGFX()->fillScreen(BLACK);  
}

void App::Application::changeCurrent(GUI::BaseWidget *next) {
  current = next;
  clearScreen();
  current->draw();
}
       
void App::Application::sendMessage(GUI::Message *msg) {
    if (msg->msg == APP_MESSAGE_GO_HOME) {
      changeCurrent(&mainScreen);
    } else if (msg->msg == APP_MESSAGE_RECORD) {
      changeCurrent(&recordScreen);
    }
}




