// Encoder

// #ifdef ENCODER_ENABLE

#include "encoder.h"
#include "smbus.h"

Encoder* new_encoder(SMBus* i2cBus, _Bool A1_power, _Bool A2_power){
    Encoder* encoder = (Encoder*) malloc(sizeof(Encoder));

    if ((A1_power) && (A2_power)) {
        encoder->address = device_slave_address_both_power;
    }
    else if (A1_power) {
        encoder->address = device_slave_address_a1_power;
    }
    else if (A2_power) {
        encoder->address = device_slave_address_a2_power;
    }
    else {
        encoder->address = device_slave_address_none_power;
    }

    encoder->i2cBus = i2cBus;

    return encoder;
}

uint16_t read_raw_angle(Encoder* encoder) {

    uint8_t angle_high_data = read_byte_data(encoder->i2cBus, encoder->address, 0xFE);
    uint8_t angle_low_data = read_byte_data(encoder->i2cBus, encoder->address, 0xFF);
    
    uint8_t angle_low_data_modified = (uint8_t)angle_low_data & (uint8_t)0x3F;

    uint16_t angle_raw = (uint16_t)(angle_high_data << 6) | (uint16_t)angle_low_data_modified;

    return angle_raw;
}

float get_angle_degrees(Encoder* encoder) {

    uint16_t angle_raw = read_raw_angle(encoder);
    float degrees_proportion = 180.0 * (float)angle_raw;
    float degrees = degrees_proportion / ((float)(raw_to_180_degrees_conversion_factor));

    return degrees;
}


void deleteEncoder(Encoder* encoder){
    free(encoder);
}

// #endif
