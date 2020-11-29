import smbus
# import numpy as np
import time
from math import ceil

# Assuming using AS5048B
# Connect to BLANK : TSSOP14 Pin  to Beaglebone Pin
# Connect to GND : 13 to Cape P9 1
# Connect to A1: Cape P9 2
# Connect to A2: Cape P9 43
# Connect to PWR/5V: Cape P9 7 or 8 
# Connect to PWR/3V: Cape P9 3 or 4
# Connect to SDA: 1 to Cape P9 20
# Connect to SCL: 2 to Cape P9 19

# bus initialization
# We initialize bus 2 because that is the only free on on the beaglebone.
bus = smbus.SMBus(2)

# I2C Address
OLD_I2C_ADDRESS = 0x40

# Set new I2C slave address to 0x44
# bus.write_byte_data(OLD_I2C_ADDRESS, 0x15, 0x01)
# I2C_ADDRESS = 0x44

I2C_ADDRESS = OLD_I2C_ADDRESS

# Set new I2C slave address to 0x44
# bus.write_byte_data(0x40, 0x15, 0x01)
# Enable special programming mode
# bus.write_byte_data(I2C_ADDRESS_REGISTER, 0x03, 0xFD)
# Enable automatic programming procedure
# bus.write_byte_data(I2C_ADDRESS_REGISTER, 0x03, 0x08)
# Enable automatic programming procedure
# bus.write_byte_data(I2C_ADDRESS_REGISTER, 0x03, 0x00)

# Relevant register
AngleLow = 0xFE
AngleHigh = 0xFF

# This value represents 180 degrees
MaxValue = 8190.0

# bus.read_byte_data(I2C_ADDRESS, register)

while(True):

    # read the raw data

    AngleLowByte = bus.read_byte_data(I2C_ADDRESS, AngleLow)
    AngleHighByte = bus.read_byte_data(I2C_ADDRESS, AngleHigh)
    AngleData = ( AngleHighByte << 8 ) | AngleLowByte



    # use this code in the future

    # change the value of Angle Data if angle is over 180
    if AngleData > MaxValue:
        AngleData = -AngleData

    Degrees = 180 * (MaxValue + AngleData) / (MaxValue)
    Degrees180 = Degrees - 180
    print("Angle in Degrees: " + str(Degrees) + "; Degrees w/out 180:" +
    str(Degrees180) + "; Raw Angle Data: " + str(AngleData))

