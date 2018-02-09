#include "Arduino.h"
#ifndef MC3413_h
#define MC3413_h

//I2C Device Address-------------------------------------------------------------
#define MC3413_ADDRESS 0x4C             //This is the I2C address for our chip.

//I2C Registers
#define REG_SR                0x03
#define REG_INTEN             0x06
#define REG_MODE              0x07
#define REG_SRTFR             0x08
#define REG_TAPEN             0x09
#define REG_OUTCFG            0x20

//Parameter Offsets to use in the Command Register
#define PARAM_STANDBY         0x00
#define PARAM_WAKE            0x01
#define PARAM_INTN_actLOW     0x00
#define PARAM_INTN_actHIGH    0x80

#define PARAM_RATE_32HZ       0x00
#define PARAM_RATE_16HZ       0x01
#define PARAM_RATE_8HZ        0x02
#define PARAM_RATE_4HZ        0x03
#define PARAM_RATE_2HZ        0x04
#define PARAM_RATE_1HZ        0x05
#define PARAM_RATE_0_5HZ      0x06
#define PARAM_RATE_0_25HZ     0x07
#define PARAM_RATE_64HZ       0x08
#define PARAM_RATE_128HZ      0x09
#define PARAM_RATE_256HZ      0x0A
#define PARAM_TAP_LATCH       0x80

#define PARAM_RES_6Bits       0x00
#define PARAM_RES_7Bits       0x01
#define PARAM_RES_8Bits       0x02
#define PARAM_RES_10Bits      0x03
#define PARAM_RES_12Bits      0x04
#define PARAM_RES_14Bits      0x05
#define PARAM_RANGE_2g        0x00
#define PARAM_RANGE_4g        0x10
#define PARAM_RANGE_8g        0x20
#define PARAM_RANGE_16g       0x30
#define PARAM_RANGE_12g       0x40

#define PARAM_INT_DISABLE     0x00
#define PARAM_TIXPEN          0x01
#define PARAM_TIXNEN          0x02
#define PARAM_TIYPEN          0x04
#define PARAM_TIYNEN          0x08
#define PARAM_TIZPEN          0x10
#define PARAM_TIZNEN          0x20
#define PARAM_ACQ_INT_EN      0x80

#define PARAM_TAP_DISABLE     0x00
#define PARAM_TAPXPEN         0x01
#define PARAM_TAPXNEN         0x02
#define PARAM_TAPYPEN         0x04
#define PARAM_TAPYNEN         0x08
#define PARAM_TAPZPEN         0x10
#define PARAM_TAPZNEN         0x20
#define PARAM_THRDUR_LOW      0x00
#define PARAM_THRDUR_HIGH     0x40
#define PARAM_TAPEN           0x80


class MC3A
{
  public:
    MC3A();
    void init();
    void getValues(int *x,int *y,int *z);
    int readReg(unsigned char REG_ADDRESS);
    void gotoSingleMode(void);
    void gotoIntMode(void);
    void gotoSTANDBY(void);
    void gotoWAKE(void);
  private:
	  void MC3413_I2C_SEND(unsigned char REG_ADDRESS, unsigned  char DATA);
//    int Normalization(int data);
};

extern MC3A MC3413;

#endif 
