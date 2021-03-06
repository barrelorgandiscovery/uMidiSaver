
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
  
  
}

void App::ShowMidiWidget::resetPosition() {
  current = 0;
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
    GUI::Scale s(0, ext.y + ext.height,  127, ext.y);
    for (int i = 0 ; i < 128; i ++) {
      if (ss_get_note_state(i)) {
        gfx->drawPixel(current + ext.x, s.convert(i), RED);
      } 
    }
  }
  
}
       

///////////////////////////////////////////////////////////////////////////////////
// Record screen

static const GUI::Rect extStop = {1,270,238,50};
static GUI::Button stop(&App::recordScreen,&extStop,"Stop",3);
static App::ShowMidiWidget midiWidget(&App::recordScreen, {0,0,240,240});

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
// Main Screen - Home

// components used for the screens
static const GUI::Rect extRecord = {1,150,238,50};
static GUI::Button record(&App::mainScreen,&extRecord,"Enregistrer",3);


App::MainScreen::MainScreen() : GUI::BaseWidget(&mainApp) { // reference the main app as parent
}

void App::MainScreen::startup() {
  record.startup();
}

void App::MainScreen::draw() {
   GFX *gfx = GUI::getGFX();
   gfx->setCursor(5,10);
   gfx->setTextSize(1);
   gfx->setTextColor(WHITE);
   gfx->print(F("uMidiSaver - Record Midi into SD Card"));
   gfx->setCursor(5,25);
   gfx->print(F("P. Freydiere - 2015"));
   record.draw();
}

 // dispatch the message and return if the message is consumed
bool App::MainScreen::dispatchMessage(GUI::Message *msg) {
  return record.dispatchMessage(msg);
}

void App::MainScreen::sendMessage(GUI::Message *msg) {
  //Serial.print(F("main screen message"));
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
      ss_change_command(SS_COMMAND_END);   
      changeCurrent(&mainScreen);
    } else if (msg->msg == APP_MESSAGE_RECORD) {
      ss_change_command(SS_COMMAND_START);
      midiWidget.resetPosition();
      changeCurrent(&recordScreen);
    }
}




