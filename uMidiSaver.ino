#include <EEPROM.h>

#include "UI.h"

// #include "HardwareSerial.h"

#include <ChibiOS_AVR.h>
#include <SdFat.h>
#include "SSMidi.h"



//------------------------------------------------------------------------------
// Thread 1, manage the serial and midi saving

// 128 byte stack beyond task switch and interrupt needs
static WORKING_AREA(waThread1, 128);

static msg_t Thread1(void *arg) {
 
   ss_comm_daemon();
  return 0;
}

// static WORKING_AREA(waThread2, 128);

static msg_t Thread2(void *arg) {
  InitUI();
  RunUI();
  return 0;
}
//------------------------------------------------------------------------------
// main thread runs at NORMALPRIO


void setup() {

  cli();
  halInit();
  chSysInit();
  
  // start deamon thread
  Thread *t1 = chThdCreateStatic(waThread1, sizeof(waThread1),
                                  NORMALPRIO , Thread1, NULL);
  //main thread handle the UI
  InitUI();
  RunUI();

}

void loop() {


}


