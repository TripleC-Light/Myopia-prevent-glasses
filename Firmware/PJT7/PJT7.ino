#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <EEPROM.h>
#include "MC3413.h"
#include "VL53L0X.h"
#include "playAudio.h"

//#define dispSYSMODE
//#define dispSENSORvalue
//#define dispSENSORvalue_MC3A

#define LED_pin  8
#define MC3A_Int_pin  2
#define Speaker_pin  9
#define TTPint_pin  3

#define defaultAlarmLevel 350                 // unit:mm
#define ALARMtime 0                           // 近距離被偵測到的次數，超過將發出警告聲
#define LayDowntime 35                         // 動作靜止被偵測到的次數，超過將進入睡眠
#define setDuration 300                       // unit:ms
#define settingTimeout 20000                  // 單位：ms
#define resetTimeout 3000                     // 單位：ms

unsigned int alarmLevel = defaultAlarmLevel;
typedef enum sysMode{
  InitMode=0,
  MC3Amode,
  LayDownMode,
  VL53mode,
  ALARMmode,
  LEDmode,
  TTPinterrupt,
  setMode,
  resetToDefault,
  gotoSleep
}; 

typedef enum LAYMode{
  LAYdown=0,
  pickUP
}; 

struct EEPalarm {
  int alarmLevel;
};
EEPalarm EEPalarm;

char sysState = InitMode;
unsigned long TTPdownTime, TTPupTime, TTPtimeDuration;
int beepDuration;
char LAYstate = pickUP;
playAudio pAudio = playAudio();
VL53L0X VL53;
unsigned int VL53range;
int x,y,z;
int oldx,oldy,oldz;
int dx,dy,dz;
char LayDowntimeCounter=0;
int ALARMtimeCounter;

void setup() {
  
  Serial.begin(9600);
  Serial.println(" ");
  Serial.println("==========================");
  Serial.println("==  PTJ7 Total Program  ==");
  Serial.println("==========================");
  pinMode(LED_pin, OUTPUT);
  pinMode(Speaker_pin, OUTPUT);
  pinMode(MC3A_Int_pin, INPUT_PULLUP);
//  pinMode(MC3A_Int_pin, INPUT);
  pAudio.speaker_pin = Speaker_pin;

  EEPROM.get(0x01, EEPalarm);
  Serial.print("- EEPalarm.alarmLevel = ");
  Serial.println(EEPalarm.alarmLevel);
  if( EEPalarm.alarmLevel>=1000 || EEPalarm.alarmLevel<=0){
    EEPROM.update(0x01, defaultAlarmLevel&0x00FF);
    EEPROM.update(0x02, (defaultAlarmLevel>>8)&0x00FF);
    EEPROM.get(0x01, EEPalarm);
    Serial.print("- UpdateEEPalarm.alarmLevel = ");
    Serial.println(EEPalarm.alarmLevel);
  }else{
    alarmLevel = EEPalarm.alarmLevel;
  }
  Serial.print("- alarmLevel = ");
  Serial.println(alarmLevel);
  Wire.begin();
  Wire.setClock(50000);                     // I2C 設為50KHz 才不會掉資料
  
  MC3413.init();
  MC3413.getValues(&x,&y,&z);
  oldx = x;
  oldy = y;
  oldz = z;
//  MC3413.gotoSTANDBY();
  VL53.init();
  VL53.setTimeout(50);
  VL53.setMeasurementTimingBudget(20000);
  pAudio.play(pAudio.powerOn);
  sysState = VL53mode;
 
  /*** Setup the WDT ***/
  ADMUX &= 0x3F;                            // Close ADC
  ADCSRA &= 0x7F;                           // Close ADC
  MCUSR &= ~(1 << WDRF);                    // Clear the reset flag.
  WDTCSR |= (1 << WDCE) | (1 << WDE);       // In order to change WDE or the prescaler, we need to set WDCE (This will allow updates for 4 clock cycles).
//  WDTCSR = 1<<WDP0 | 1<<WDP3;               // 8.0 seconds
//  WDTCSR = 1 << WDP0 | 1 << WDP2;           // 0.5 seconds
  WDTCSR = 1 << WDP3;                       // 4 seconds
//  WDTCSR = 1 << WDP0 | 1 << WDP1;           // 0.125 seconds
  WDTCSR |= _BV(WDIE);                      // Enable the WD interrupt (note no reset).
}

void loop() {
    
  switch ( sysState ){
    case InitMode:
      MC3413.init();
      VL53.init();
      VL53.setTimeout(50);
      VL53.setMeasurementTimingBudget(20000);
      ALARMtimeCounter = 0;
      sysState = VL53mode;
      break;
      
    case MC3Amode:
#ifdef dispSYSMODE
      Serial.println("[MC3Amode]");
      delay(10);
#endif
//      MC3413.gotoWAKE();
      MC3413.getValues(&x,&y,&z);
      dx = abs(x-oldx);
      dy = abs(y-oldy);
      dz = abs(z-oldz);
#ifdef dispSENSORvalue_MC3A
      Serial.print("x: ");
      Serial.print(x);
      Serial.print(" y: ");
      Serial.print(y);
      Serial.print(" z: ");
      Serial.println(z);
      delay(20);
#endif
//      MC3413.gotoSTANDBY();
      sysState = LayDownMode;
      break;
      
    case LayDownMode:
#ifdef dispSYSMODE
      Serial.println("[LayDownMode]");
      delay(15);
#endif
      if( abs(z)>50 || y<-40 || (x<-40&y<-20)){
        LayDowntimeCounter = 0;
        if( LAYstate==pickUP ){
          LAYstate = LAYdown;
          pAudio.play(pAudio.inDeepSleep);
#ifdef dispSYSMODE
          Serial.println("[LAYdown]");
          delay(10);
#endif
        }
        sysState = gotoSleep;
      }else if(dx<5 && dy<5 && dz<5){
          if( LayDowntimeCounter>=LayDowntime ){
            LayDowntimeCounter = 0;
            if( LAYstate==pickUP ){
              LAYstate = LAYdown;
              pAudio.play(pAudio.inDeepSleep);
#ifdef dispSYSMODE
          Serial.println("[No move LAYdown]");
          delay(20);
#endif
            }
          }else{
            LayDowntimeCounter++;
            if( LAYstate==pickUP ){
              sysState = LEDmode;
            }else{
              sysState = gotoSleep;
            }
          }
      }else{
        if( LAYstate==LAYdown ){
          LAYstate = pickUP;
          pAudio.play(pAudio.outDeepSleep);    
#ifdef dispSYSMODE
          Serial.println("[pickUP]");
          delay(10);
#endif
        }
        oldx = x;
        oldy = y;
        oldz = z;
        LayDowntimeCounter = 0;
        sysState = gotoSleep;
      }
      break;
      
    case VL53mode:
#ifdef dispSYSMODE
      Serial.println("[VL53mode]");
      delay(10);
#endif
      if( LAYstate==pickUP ){
        VL53range = VL53.readRangeSingleMillimeters();
        if( VL53range<alarmLevel ){
          if( ALARMtimeCounter>=ALARMtime ){
            ALARMtimeCounter = 0;
            sysState = ALARMmode;
          }else if( VL53range>65530 ){
            Serial.println("VL53 TIMEOUT");
          }else{
            ALARMtimeCounter++;
            sysState = MC3Amode;
          }
        }else{
            ALARMtimeCounter = 0;
            sysState = MC3Amode;
        }
      }else{
        sysState = MC3Amode;
      }
#ifdef dispSENSORvalue
      Serial.print("Range:");
      Serial.print(VL53range);
      Serial.println("mm");
      delay(15);
#endif
      break;

    case ALARMmode:
      pAudio.play(pAudio.alarmBeep);
      sysState = MC3Amode;
      break;
      
    case LEDmode:
      digitalWrite(LED_pin, HIGH);
      delayMicroseconds(500);
      digitalWrite(LED_pin, LOW);
      TTPdownTime=0;
      sysState = gotoSleep;
      break;
      
    case TTPinterrupt:
#ifdef dispSYSMODE
      Serial.println("[TTPinterrupt]");
      delay(15);
#endif
      pAudio.play(pAudio.buttonPush);
      if( TTPdownTime==0 ){
        while(digitalRead(TTPint_pin)==HIGH);
        TTPdownTime = millis();
        while(digitalRead(TTPint_pin)==LOW){
          TTPupTime = millis()-TTPdownTime;
          if( TTPupTime>setDuration ){
            break;
          }
        }
        if( TTPupTime>setDuration ){
          Serial.println("-");
          delay(5);
          sysState = gotoSleep;
        }else{
          pAudio.play(pAudio.buttonPush);
          while(digitalRead(TTPint_pin)==HIGH);
          Serial.println("+");
          delay(5);
          sysState = setMode;
        }
        TTPdownTime = 0;
      }
      break;
      
    case setMode:
#ifdef dispSYSMODE
      Serial.println("[setMode]");
      delay(10);
#endif
      pAudio.play(pAudio.inSetMode);
      TTPupTime = millis();
      while( digitalRead(TTPint_pin)==LOW ){
        beepDuration = VL53.readRangeSingleMillimeters();
#ifdef dispSENSORvalue
        Serial.print("Range: ");
        Serial.print(beepDuration);
        Serial.println("mm");
        delay(15);
#endif
        pAudio.play(pAudio.buttonPush);
        if(beepDuration>500){
          delay(500);
        }else{
          delay(beepDuration);
        }
        TTPtimeDuration = millis() - TTPupTime;
        if( TTPtimeDuration>settingTimeout ){
          pAudio.play(pAudio.outSetMode);
          sysState = gotoSleep;
          break;
        }
      }

      TTPupTime = millis();
      pAudio.play(pAudio.buttonPush);
      while( digitalRead(TTPint_pin)==HIGH ){
        if( (millis() - TTPupTime)>resetTimeout ){
          pAudio.play(pAudio.powerOn);
          pAudio.play(pAudio.outSetMode);
          sysState = resetToDefault;
          break;
        }
      }

      if( (TTPtimeDuration<=settingTimeout) && (sysState!=resetToDefault) ){
        pAudio.play(pAudio.outSetMode);
        alarmLevel = beepDuration;
        EEPROM.update(0x01, beepDuration&0x00FF);
        EEPROM.update(0x02, (beepDuration>>8)&0x00FF);
        sysState = gotoSleep;
      }
      break;

    case resetToDefault:
#ifdef dispSYSMODE
      Serial.println("[resetToDefault]");
      delay(20);
#endif
      alarmLevel = defaultAlarmLevel;
      EEPROM.update(0x01, defaultAlarmLevel&0x00FF);
      EEPROM.update(0x02, (defaultAlarmLevel>>8)&0x00FF);
      sysState = gotoSleep;
      break;
      
    case gotoSleep:
      sysState = VL53mode;
      enterSleep();
      break;
  }
}

/***************************************************
    Name:        ISR(WDT_vect)
    Returns:     Nothing.
    Parameters:  None.
    Description: Watchdog Interrupt Service. This
                 is executed when watchdog timed out.
 ***************************************************/
ISR(WDT_vect){
  /* The program will continue from here after the WDT timeout*/
  sleep_disable();        /* First thing to do is disable sleep. */
  power_all_enable();     /* Re-enable the peripherals. */
  WDTCSR &= 0xBF;
//  Serial.println("Wake!");
//  delay(10);  
}

/***************************************************
    Name:        enterSleep
    Returns:     Nothing.
    Parameters:  None.
    Description: Enters the arduino into sleep mode.
 ***************************************************/
void enterSleep(void){
  attachInterrupt(1, wakeISR_TTP, RISING);
//  attachInterrupt(0, wakeISR_MMA, RISING);

  //set_sleep_mode(SLEEP_MODE_PWR_SAVE);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_SAVE for lowest power consumption. */
  sleep_enable();

  /* Now enter sleep mode. */
//  Serial.println("Sleep");
//  delay(6);
  WDTCSR |= _BV(WDIE);
  sleep_mode();
}

void wakeISR_TTP(){
  sleep_disable();        /* First thing to do is disable sleep. */
  power_all_enable();     /* Re-enable the peripherals. */
  WDTCSR &= 0xBF;
  sysState = TTPinterrupt;
  Serial.println("[WAKE by TTP]");
  delay(50);
}

//void wakeISR_MC3A(){
//  sleep_disable();        /* First thing to do is disable sleep. */
//  power_all_enable();     /* Re-enable the peripherals. */
//  WDTCSR &= 0xBF;
//  sysState = VL53mode;
////  pAudio.play(pAudio.outDeepSleep);
//  Serial.println("[WAKE by 3-Axia]");
//  delay(50);
//}

