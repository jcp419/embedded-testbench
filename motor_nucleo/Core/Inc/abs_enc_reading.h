/*
 * abs_enc_read.h
 *
 *  Created on: Oct 26, 2020
 *      Author: Raymond Liu
 */

#ifndef INC_ABS_ENC_READING_H_
#define INC_ABS_ENC_READING_H_

#include "stm32f3xx_hal.h"
#include "stdint.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define RAW_TO_180_DEGREES_CONVERSION_FACTOR 8192.0
#define TRUE 1
#define FALSE 0

enum {
	device_slave_address_none_power = 0x40,
	device_slave_address_a1_power = 0x41,
	device_slave_address_a2_power = 0x42,
	device_slave_address_both_power = 0x43,
};

typedef struct {
    I2C_HandleTypeDef *i2c;
    HAL_StatusTypeDef ret;
    uint8_t buf[30];
    uint8_t DMA;
} SMBus;

typedef struct {
	int address;
	SMBus* i2cBus;
} AbsEncoder;

SMBus *new_smbus(I2C_HandleTypeDef *hi2c);

long read_word_data(SMBus *smbus, uint8_t addr, char cmd);

void disable_DMA(SMBus *smbus);

void del_smbus(SMBus *smbus);

AbsEncoder* new_abs_encoder(SMBus* i2cBus, _Bool A1_power, _Bool A2_power); // 1 if pin connected to power, 0 if pin connected to ground

int read_raw_angle(AbsEncoder* abs_encoder);

float get_angle_degrees(AbsEncoder* abs_encoder);

void deleteEncoder(AbsEncoder*);

AbsEncoder* abs_encoder_init(I2C_HandleTypeDef* abs_encoder_handle);

void read_abs_enc(AbsEncoder* abs_encoder, uint8_t channel);

#endif /* INC_ABS_ENC_READING_H_ */
