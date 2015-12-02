
#ifndef _SSMIDI_H
#define _SSMIDI_H



#include <SdFat.h>



extern "C" {
  
  // static char bufferState[128/8]; // for gui showing the state of the read
  
  // buffer des ordres sauvegardes dans la SDCard
  
  // static char saveBuffer[400];

  
  void midiDaemon();

  

}



#endif
