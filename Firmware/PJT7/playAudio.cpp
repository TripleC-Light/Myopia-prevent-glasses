#include "Arduino.h"
#include "playAudio.h"

playAudio::playAudio()
{
}

void playAudio::play( char index ){
  switch( index ){
    case powerOn:
      tone(speaker_pin,1047,100);
      delay(120);
      tone(speaker_pin,1047,100);
      break;
      
    case buttonPush:
      tone(speaker_pin,1047,100);
      delay(120);
      break;
      
    case alarmBeep:
      tone(speaker_pin,1568,100);
      delay(120);
      tone(speaker_pin,1568,100);
      delay(120);
      tone(speaker_pin,1568,100);
      delay(120);
      break;
      
    case inDeepSleep:
      tone(speaker_pin,659,300);
      delay(300);
      tone(speaker_pin,587,400);
      delay(400);
      tone(speaker_pin,523,500);
      delay(500);
      break;
      
    case outDeepSleep:
      tone(speaker_pin,523,200);
      delay(200);
      tone(speaker_pin,587,200);
      delay(200);
      tone(speaker_pin,659,200);
      delay(200);
      break;
      
    case inSetMode:
      tone(speaker_pin,1047,300);
      delay(400);
      break;
      
    case outSetMode:
      tone(speaker_pin,1047,300);
      delay(300);
      tone(speaker_pin,1047,100);
      delay(120);
      break;
  }
}

