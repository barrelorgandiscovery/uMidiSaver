

#include "SSMidi.h"
#include <ChibiOS_AVR.h>

// main SD object
static SdFat SD;
static SdFile myFile;

SEMAPHORE_DECL(sem_com, 0);
static char ss_comm_command = SS_COMMAND_WAIT;

void ss_change_command(char command) {
    ss_comm_command = command;
    chSemWait(&sem_com);
}


void ss_wait_for_command(char command) {

    while (command != ss_comm_command) {
        chThdSleepMilliseconds(200);
    }
    chSemSignal(&sem_com);

}

void ss_comm_daemon() {

   if (!SD.begin(10)) { // chip select on 10
            Serial.println(F("initialization failed!"));
            return;
          }
          Serial.println(F("initialization done."));
        

  while (true) {
      // wait for the start of the record
      ss_wait_for_command(SS_COMMAND_START); 
    
      while (ss_comm_command != 'E') {
         
         
         //  SD.open("/");
         // open the file for write at end like the Native SD library
          if (!myFile.open("test.txt", O_RDWR | O_CREAT | O_AT_END)) {
            Serial.print(F("opening test.txt for write failed"));
          }
          // if the file opened okay, write to it:
          Serial.print(F("Writing to test.txt..."));
          long m = millis();
          for (int i = 0 ; i < 1000; i ++) {
          myFile.println(F("testing 1, 2, 3."));
          }
          // close the file:
          myFile.close();
          Serial.print(F("perfs :"));
          Serial.print(millis() - m);
          Serial.println();
      } // while not end
      ss_wait_for_command(SS_COMMAND_END); 
  } //while true
}

