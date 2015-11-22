

#include "UI.h"

#include "SoftwareSerial.h"

#include <ChibiOS_AVR.h>


// Declare a semaphore with an inital counter value of zero.
// SEMAPHORE_DECL(sem, 0);
//------------------------------------------------------------------------------
// Thread 1, turn the LED off when signalled by thread 2.

// 64 byte stack beyond task switch and interrupt needs
static WORKING_AREA(waThread1, 32);

static msg_t Thread1(void *arg) {
  while(true) {
  
  
 
 
    // Wait for signal from thread 2.
   //  chSemWait(&sem);

    // Turn LED off.
   // Serial.print("x");
    //delay(200);
  }
  return 0;
}
//------------------------------------------------------------------------------
// Thread 2, turn the LED on and signal thread 1 to turn the LED off.

// 64 byte stack beyond task switch and interrupt needs
static WORKING_AREA(waThread2, 128);

static msg_t Thread2(void *arg) {
 // noInterrupts();
 InitUI();
 //interrupts();
  RunUI();
   
  return 0;  
}

//------------------------------------------------------------------------------
// main thread runs at NORMALPRIO
void chSetup() {


  // start blink thread
 //chThdCreateStatic(waThread1, sizeof(waThread1),
 //   NORMALPRIO , Thread1, NULL);

  chThdCreateStatic(waThread2, sizeof(waThread2),
    NORMALPRIO+1 , Thread2, NULL);

}



void setup() {
   
}

void loop() {

    chBegin(chSetup);

  // main thread
   // InitUI();
   Serial.begin(115000);
   Serial.print("init");

  
   while(true) {
   Serial.print("x\n"); 
   }

}


