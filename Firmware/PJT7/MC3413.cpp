#include "MC3413.h"
#include "Arduino.h"
#include <Wire.h>


MC3A::MC3A()
{
}

void MC3A::init()
{
  MC3413_I2C_SEND(REG_MODE, PARAM_STANDBY);                        // Change to STANDBY mode
  MC3413_I2C_SEND(REG_SRTFR, PARAM_RATE_16HZ|PARAM_TAP_LATCH);     // Set sample rate & tap feature
  MC3413_I2C_SEND(REG_OUTCFG, PARAM_RANGE_2g|PARAM_RES_8Bits);     // Set resolution & range
  
  MC3413_I2C_SEND(REG_INTEN,PARAM_INT_DISABLE);
//  MC3413_I2C_SEND(REG_INTEN,PARAM_ACQ_INT_EN);
//  MC3413_I2C_SEND(REG_INTEN,PARAM_TIZPEN|PARAM_TIZNEN);
//  MC3413_I2C_SEND(REG_INTEN,PARAM_TIXPEN|PARAM_TIXNEN|PARAM_TIYPEN|PARAM_TIYNEN|PARAM_TIZPEN|PARAM_TIZNEN);
  
  MC3413_I2C_SEND(REG_TAPEN,PARAM_TAP_DISABLE);
//  MC3413_I2C_SEND(REG_TAPEN,PARAM_TAPEN|PARAM_TAPZPEN|PARAM_TAPZNEN|PARAM_THRDUR_LOW);
//  MC3413_I2C_SEND(REG_TAPEN,PARAM_TAPEN|PARAM_TAPXPEN|PARAM_TAPXNEN|PARAM_TAPYPEN|PARAM_TAPYNEN|PARAM_TAPZPEN|PARAM_TAPZNEN|PARAM_THRDUR_HIGH);
//  
  MC3413_I2C_SEND(0x0A,0x02);
  MC3413_I2C_SEND(0x0B,0x02);
  MC3413_I2C_SEND(0x0C,0x02);
  
  MC3413_I2C_SEND(0x07,PARAM_WAKE);                           // Change to WAKE mode
}

void MC3A::gotoSingleMode(void )
{
  MC3413_I2C_SEND(REG_MODE, PARAM_STANDBY);                        // Change to STANDBY mode
  MC3413_I2C_SEND(REG_SRTFR, PARAM_RATE_16HZ|PARAM_TAP_LATCH);     // Set sample rate & tap feature
  MC3413_I2C_SEND(REG_OUTCFG, PARAM_RANGE_2g|PARAM_RES_8Bits);     // Set resolution & range
  MC3413_I2C_SEND(REG_INTEN,PARAM_INT_DISABLE);
  MC3413_I2C_SEND(REG_TAPEN,PARAM_TAP_DISABLE);
  MC3413_I2C_SEND(0x07,PARAM_WAKE);                           // Change to WAKE mode
}

void MC3A::gotoSTANDBY(void)
{
  MC3413_I2C_SEND(REG_MODE, PARAM_STANDBY);                        // Change to STANDBY mode
}

void MC3A::gotoWAKE(void)
{
  MC3413_I2C_SEND(REG_MODE, PARAM_WAKE);                        // Change to STANDBY mode
}

void MC3A::gotoIntMode(void)
{
  MC3413_I2C_SEND(REG_MODE, PARAM_STANDBY);                        // Change to STANDBY mode
  MC3413_I2C_SEND(REG_SRTFR, PARAM_RATE_16HZ|PARAM_TAP_LATCH);     // Set sample rate & tap feature
  MC3413_I2C_SEND(REG_OUTCFG, PARAM_RANGE_2g|PARAM_RES_8Bits);     // Set resolution & range
  
  MC3413_I2C_SEND(REG_INTEN,PARAM_TIXPEN|PARAM_TIXNEN|PARAM_TIYPEN|PARAM_TIYNEN|PARAM_TIZPEN|PARAM_TIZNEN);
  MC3413_I2C_SEND(REG_TAPEN,PARAM_TAPEN|PARAM_TAPXPEN|PARAM_TAPXNEN|PARAM_TAPYPEN|PARAM_TAPYNEN|PARAM_TAPZPEN|PARAM_TAPZNEN|PARAM_THRDUR_HIGH); 
  MC3413_I2C_SEND(0x0A,0x02);
  MC3413_I2C_SEND(0x0B,0x02);
  MC3413_I2C_SEND(0x0C,0x02);
  
  MC3413_I2C_SEND(REG_MODE,PARAM_WAKE);                           // Change to WAKE mode
}

void MC3A::getValues(int *x,int *y,int *z)
{
  int DATA=0;
  Wire.beginTransmission(MC3413_ADDRESS);
  Wire.write(0x0D);
  Wire.endTransmission();
  Wire.requestFrom(MC3413_ADDRESS, 6); // read a byte
  if(Wire.available()){
    DATA = Wire.read();
    *x = (Wire.read()*256)+DATA;
    
    DATA = Wire.read();
    *y = (Wire.read()*256)+DATA;
    
    DATA = Wire.read();
    *z = (Wire.read()*256)+DATA;
  }
  readReg(REG_SR);
}

void MC3A::MC3413_I2C_SEND(unsigned char REG_ADDRESS, unsigned char DATA){
  Wire.beginTransmission(MC3413_ADDRESS);
  Wire.write(REG_ADDRESS);
  Wire.write(DATA);
  Wire.endTransmission();
}

int MC3A::readReg(unsigned char REG_ADDRESS){
  Wire.beginTransmission(MC3413_ADDRESS);
  Wire.write(REG_ADDRESS);
  Wire.endTransmission();
  Wire.requestFrom(MC3413_ADDRESS, 1); // read a byte
  if(Wire.available()){
    return(Wire.read());
  }
  return(0xFF);
}

//int MMA::Normalization(int data){  //Data Normalization 0~63 to -32~31
//    if( (data&0x20)==0x20 ){
//      data = data | 0xFFC0;
//    }
//    return data;
//}

// Set the default object
MC3A MC3413 = MC3A();










