"""
ardu_behavior.py

Created by Germain Sauve

"""
from arducomm import SerialArduino


class Node():

    def __init__(self):
        self.child = None
        self.next = None
        self.name = "unkown"
        self.ArduType = 0
        self.ArduData = 0
        self.canHaveChild = False

    def setup( self, nname, atype, canChild):
        self.name = nname
        self.ArduType = atype
        self.canHaveChild = canChild
        return self

    def debugPrint(self, depth):
        strprint = ""
        for d in range(0, depth):
            strprint += "-"
        strprint += " " + self.name
        strprint += " data= " + str(self.ArduData)
        print(strprint)
        if self.child != None:
            self.child.debugPrint(depth + 1)
        if self.next != None:
            self.next.debugPrint(depth)

    def flatten(self):
        ret = []
        ret.append(self.name)
        if self.child != None:
            ret.extend(self.child.flatten())
        if self.next != None:
            ret.extend(self.next.flatten())
        return ret

class BlackBoard():

    def __init__(self):
        self.data = {}

    def parseFromSerial(self, serialtext):
        print(serialtext)



class BehaviorTree():

    def __init__(self):
        self.root = None

    def debugPrint(self):
        if self.root != None:
            self.root.debugPrint(0)
    
    def parseFromSerial(self, serialtext):
        print(serialtext)

    def flattenTree(self):
        return self.root.flatten()
