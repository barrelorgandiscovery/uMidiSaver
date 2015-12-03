
#ifndef _SSMIDI_H
#define _SSMIDI_H



#include <SdFat.h>
#include <ChibiOS_AVR.h>

#define SS_COMMAND_WAIT 'W'
#define SS_COMMAND_START 'S'
#define SS_COMMAND_END 'E'

extern "C" {
  
  void ss_comm_daemon();


  // rendez vous implementation, 
  // for communication
  void ss_wait_for_command(char command);
  void ss_change_command(char command);

}



#endif
