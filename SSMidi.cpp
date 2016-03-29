

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
/*
inline void emitNote(bool onoff, unsigned long ticks, uint8_t note, uint8_t velocity) {

  myFile.write(onoff);
  
  myFile.write(ticks);
  
  myFile.write(note);
  
  myFile.write(velocity);
  
   
}
*/
inline uint8_t blockingSerialRead() {
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
        char file[9];
        file[8] = '\0';
        sprintf(file, "F%05d.e", fileNo);
        // open the file for write at end like the Native SD library
        if (!myFile.open(file, O_RDWR | O_CREAT | O_AT_END)) {
              Serial.print(F("opening file for write failed"));
              return;
        }
      }

      uint8_t running_status; // current running status
      uint8_t command_read = 0; // has read command
    
      while (ss_comm_command != 'E') { // while the user has not pressed stop

          // read a byte
          uint8_t b = blockingSerialRead();
          // command sent

      cmd_received:

          command_read = 0;
          if ( b > 0x7F) {
            // command
            running_status = b;
            command_read = 1;
          }
          

          const uint8_t cmd = (running_status >> 4); // shift running status
          if (cmd == NOTE_ON || cmd == NOTE_OFF) {


            
             // ss_set_note_state(1,!ss_get_note_state(1));
             uint8_t note = b;

             if (command_read > 0) {
                note = blockingSerialRead();
             }
             // we ignore the channel
             uint8_t velocity = blockingSerialRead();
             bool isOn = (cmd == NOTE_ON && velocity != 0);
             emitNote(isOn, millis(), note,velocity);
             ss_set_note_state(note, isOn);

          
          }  else if ( cmd == 0x0A || cmd == 0x0B || cmd == 0x0E) 
          {
            // 2 bytes to read
            b = blockingSerialRead();
            if (b > 0x7F) goto cmd_received;
            b = blockingSerialRead();
            if (b > 0x7F) goto cmd_received;
            
          } else if (cmd == 0x0C || cmd == 0x0D) {
            // 1 byte to read
            b = blockingSerialRead();
            if (b > 0x7F) goto cmd_received;
            
          } else if (cmd == 0x0F) {
              if (running_status == 0xFF) {
                // we have a FF command
                running_status = 0;
                // nb of byte to read
                uint8_t n = blockingSerialRead();
                while(n-->0) {
                  // read the other bytes
                  blockingSerialRead();
                }
              } else if (running_status == 0xF8 || running_status == 0xFA
                          || running_status == 0xFB || running_status == 0xFC) {

                  // no other bytes              
              }

          }
         
      } // while not end

      // close the file:
      myFile.close();
      ss_wait_for_command(SS_COMMAND_END); 
  }   //while true


}




