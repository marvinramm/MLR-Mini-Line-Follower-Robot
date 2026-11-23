#ifndef PINS_H
#define PINS_H

/////////////Motor Driver (TB6612FNG)///////////////

#define PWMA           6
#define PWMB           7

////////////Shiftregister (74HC595PW)///////////////

#define SHCP           5
#define STCP           4
#define DS             3

#define SR_AIN1        0
#define SR_AIN2        1
#define SR_BIN1        2
#define SR_BIN2        3
#define SR_STBY        4
#define SR_RED         5
#define SR_GREEN       6
#define PWR_EN         7

/////////////////////Inputs/////////////////////////

#define PT_L           0
#define PT_R           1

/////////////////////Outputs/////////////////////////

#define WHITE          10
#define RED            8
#define GREEN          2

///////////////////////I2C///////////////////////////

#define SDA            21
#define SCL            20

#endif
