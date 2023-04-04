# Use PySerial to talk to serial ports.
import serial

# On *nix, this will be /dev/<something>
ser = serial.Serial('COM13', 9600)

# Data to send looks like this:
#   1 byte | sync byte (0xFF)
# 75 bytes | color data (R1, G1, B1, R2, G2, B2 ... R25, G25, B25)
# 25 bytes | servo positions (0-255 each)
# 
# all data is in order, so controller will write colors[0] and positions[0] to the first LED/servo

colors = [
    [100, 255, 100],
    [100, 255, 100],
    [100, 255, 100],
    [100, 255, 100],
    [100, 255, 100],
    [100, 255, 100],
    [100, 255, 100],
    [100, 255, 100],
    [100, 255, 100],
    [100, 255, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100],
    [100, 254, 100]
]
colors_flattened = [channel for color in colors for channel in color]

positions = [
    30, 60, 90, 120, 150,
    30, 60, 90, 120, 150,
    30, 60, 90, 120, 150,
    30, 60, 90, 120, 150,
    30, 60, 90, 120, 150
]

ser.write(b'\xff')
ser.write(bytes(colors_flattened + positions))