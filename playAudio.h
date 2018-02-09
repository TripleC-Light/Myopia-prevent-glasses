//#include "musicPitchList.h"

#ifndef playAudio_h
#define playAudio_h

class playAudio
{
  public:
    playAudio();
    void play( char index );
    char speaker_pin = 2;
    typedef enum audioList{
      powerOn = 0,
      buttonPush,
      alarmBeep,
      inDeepSleep,
      outDeepSleep,
      inSetMode,
      outSetMode
    }; 
  private:
};

extern playAudio pAudio;

#endif

