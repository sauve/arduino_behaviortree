"""
arduuinowriter.py

Created be Germain Sauve

"""

class ArduinoWriter():
    """
    Serialise data in .h or text for arduino compiler
    """

    def __init__(self):
        self.output = None

    def open( self, path ):
        self.output = open(path, 'wt')

    def close(self):
        self.output.close()

    

