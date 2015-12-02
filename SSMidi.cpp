

#include "SSMidi.h"

static SdFat SD;


void midiDaemon() {

 SdFile myFile;

   if (!SD.begin(10)) { // chip select on 10
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));

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
  


  
}

