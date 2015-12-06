

#include "SSMidi.h"
#include <ChibiOS_AVR.h>
#include <EEPROM.h>


// main SD object
static SdFat SD;
static SdFile myFile;

static char ss_state[16] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};


void ss_reset_notes_state() {
  for (int i = 0 ; i < 16 ; i ++) {
    ss_state[i] = 0;
  }
}

bool ss_get_note_state(uint8_t note) {
  if (note > 127)
    return false;
  uint8_t index = note >> 3;
  uint8_t m = note - (index << 3);
  return (ss_state[index] & (1 << m)) != 0;
}

void ss_set_note_state(uint8_t note, bool state) {
  if (note > 127)
    return;
  uint8_t index = note >> 3;
  uint8_t m = note - (index << 3);
  if (state) {
    ss_state[index] = ss_state[index] | 1 << m;
  } else {
    ss_state[index] = ss_state[index] & ~( 1 << m );
  }
}

///////////////////////////////////////////////////////////
// communication handling

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

////////////////////////////////////////////////////////
// file handling

int getNextNo() {

      int fileNo;
      EEPROM.get(100, fileNo);
      fileNo +=1;
      if (fileNo > 10000) {
        fileNo = 0;
      }
      EEPROM.put(100, fileNo);
      return fileNo;
  
}

////////////////////////////////////////////////////////
// midi state machine

struct midi_parser_state {
  uint8_t running_state;
} parser_state;

void emitNote(bool onoff, unsigned long ticks, uint8_t note, uint8_t velocity) {

  myFile.print(onoff);
  myFile.print(' ');
  
  myFile.print(ticks);
  myFile.print(' ');

  myFile.print(note);
  myFile.print(' ');

  myFile.print(velocity);
  myFile.println();
  
   
}

uint8_t blockingSerialRead() {
  int r = Serial.read();
  while(r < 0) {
    r = Serial.read();
  }
  return r;
}


#define NOTE_ON 0x09
#define NOTE_OFF 0x08


/////////////////////////////////////////////////////////
// main procedure for the midi sub system

void ss_comm_daemon() {

   Serial.begin(31250);

   //Serial.begin(115000);
   
   if (!SD.begin(10)) { // chip select on 10
      Serial.println(F("initialization failed!"));
      return;
   }
   // Serial.println(F("initialization done."));
        

   while (true) {
      // wait for the start of the record
      ss_wait_for_command(SS_COMMAND_START); 
      ss_reset_notes_state();

      // read the file no in the eeprom
      // on address 100 there is the file no
      int16_t fileNo = getNextNo();
     
      
      // create the file
      {
        char file[11];
        sprintf(file, "F%05d.ev", fileNo);
        // open the file for write at end like the Native SD library
        if (!myFile.open(file, O_RDWR | O_CREAT | O_AT_END)) {
              Serial.print(F("opening file for write failed"));
              return;
        }
      }
    
      while (ss_comm_command != 'E') { // while the user has not pressed stop

          uint8_t b = blockingSerialRead();
          // command sent

          uint8_t cmd = (b >> 4);
          if (cmd == NOTE_ON || cmd == NOTE_OFF) {
             ss_set_note_state(1,!ss_get_note_state(1));
             uint8_t note = blockingSerialRead();
             uint8_t velocity = blockingSerialRead();
             bool isOn = cmd == NOTE_ON && velocity != 0;
             emitNote(isOn, millis(), note,velocity);
             ss_set_note_state(note, isOn);
          } else 
          {
             // other events .. to handle
          }
         
      } // while not end

      // close the file:
      myFile.close();
      ss_wait_for_command(SS_COMMAND_END); 
  }   //while true
}

