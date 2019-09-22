"""
 arducomm.py
 Created by Germain Sauve

"""

import serial

class SerialArduino():

    def __init__(self):
        self.ser = None
        self.portname = None
        self.portspeed = None

    def open(self, name, speed):
        print( name, speed )
        self.portname = name
        self.portspeed = speed
        self.ser = serial.Serial(name, speed, timeout=1)

    def write(self, text):
        self.ser.write(text)

    def close(self):
        self.ser.close()

    def readData(self):
        return "False"

    def readLine(self):
        return self.ser.readline().decode("utf-8")
    




