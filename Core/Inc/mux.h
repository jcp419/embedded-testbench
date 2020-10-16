#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f3xx_hal.h"
#include "smbus.h"


const int I2C_MUX_ADDRESS = 0x70;
const int MUX_CMD = 0xCC;  // filler address

int *channel_map = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 }

typedef struct {
    Bus *i2cbus;
    int channel_list[8];
} Mux;

// takes in the i2c bus and the channels to be written 
// to on the mux (0 - 7), and the number of channels 
Mux *new_mux(Bus *i2cbus, int channels, int *channel_list);

// Requires : channel is an actual register ie 0x01 (0x00 is not one of these)
// IF you want to read data, talk to only one sensor at a time -
// ie 0x01 talks to 0 only, 0x02 talks to 1 only,
// 0x04 talks to 2 only... 0x40 talks to 8 only
// IF you want to write to multiple rgb sensors you can
// "combine" the above ie 0x02 + 0x01 = 0x03 will write to 0 AND 1.
// 0xff will write to ALL rgb sensors
// Modifies which channel you are talking to
void select(Mux *mux, int channel);
