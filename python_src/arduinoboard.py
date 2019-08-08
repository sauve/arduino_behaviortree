"""

By Germain Sauve

"""

from arducomm import SerialArduino

class ArduinoBoard():

    def __init__(self):
        self.serial = SerialArduino()
        self.runningBTree = None
        self.runningBBoard = None

    def connect(self):
        self.serial.open('COM5', 57600)

    def disconnect(self):
        self.serial.close()
        
    def update(self):
        """
        Read the data from the serial and update internal state
        """
        print("update")
        # look for a new line starting with { end ending with }
        # try to parse json
