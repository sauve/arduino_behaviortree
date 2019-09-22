"""
ardu_behavior.py

Created by Germain Sauve

"""
from arducomm import SerialArduino


class Node():

    def __init__(self):
        self.child = None
        self.next = None
        self.parent = None
        self.name = "unkown"
        self.deflabel = ''
        self.ArduType = 0
        self.ArduData = 0
        self.state = 0
        self.priority = 0
        self.canHaveChild = False

    def addChild(self, node):
        # check if alreayd child then require canHaveMultipe and to addNext to child
        if self.canHaveChild:
            self.child = node
        return node

    def addNext(self, node):
        # check if next alreayd exist, if so move to the end
        self.next = node
        return node

    def setup( self, nname, atype, canChild):
        self.name = nname
        self.ArduType = atype
        self.canHaveChild = canChild
        self.state = 0
        self.priority = 0
        return self

    def fromJSON( self, jsondata):
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

    def countChild(self):
        ret = 1
        if self.child != None:
            curnode = self.child
            while curnode != None:
                ret += curnode.countChild()
                curnode = curnode.next
        return ret

class BlackBoard():

    def __init__(self):
        self.data = {}

    def toJSON(self):
        ret = {}
        return ret

    def fromJSON(self, jsondata):
        return self


class BehaviorNodeDefinition():

    def __init__(self, jsonData=None):
        self.label = 'unknown'
        self.ardutype = 0
        self.canHaveChild = False
        self.canHaveMultipleChhild = False
        self.datatype = 0
        if jsonData is not None:
            self.fromJSON(jsonData)

    def setup(self, label, ardutype, datatype, canChild, canMChild):
        self.label = label
        self.ardutype = ardutype
        self.datatype = datatype
        self.canHaveChild = canChild
        self.canHaveMultipleChhild = canMChild
        return self
    
    def fromJSON(self, jsonData):
        self.label = jsonData['label']
        self.ardutype = jsonData['ardutype']
        self.datatype = jsonData['datatype']
        self.canHaveChild = jsonData['canHaveChild']
        self.canHaveMultipleChhild = jsonData['canHaveMultipleChhild']
        return self

    def toJSON(self):
        ret = {}
        ret['label'] = self.label
        ret['ardutype'] = self.ardutype
        ret['datatype'] = self.datatype
        ret['canHaveChild'] = self.canHaveChild
        ret['canHaveMultipleChhild'] = self.canHaveMultipleChhild
        return ret

    def setupNodeFromDefinition( self, node, nodeName, nodeData):
        node.setup(nodeName, self.ardutype, self.canHaveChild)
        return node
        

        
class BehaviorDefinitions():

    def __init__(self, jsonData = None):
        self.name = 'Definitions'
        self.nodeDefitions = {}
        if jsonData is not None:
            self.fromJSON(jsonData)

    def getDefition(self, label):
        ret = None
        if label in self.nodeDefitions:
            ret = self.nodeDefitions[label]
        return ret

    def setupNodeFromDefinition(self, label, node, nodeName, nodeData):
        defn = self.getDefition(label)
        if defn is not None:
            defn.setupNodeFromDefinition( node, nodeName, nodeData)
        return node

    def createNodeFromDefinition(self, label, nodeName, nodeData):
        ret = Node()
        defn = self.getDefition(label)
        if defn is not None:
            defn.setupNodeFromDefinition( ret, nodeName, nodeData)
        return ret

    def addNodeDefinition(self, ndef):
         self.nodeDefitions[ndef.label] = ndef

    def fromJSON( self, jsonData):
        self.name = jsonData['name']
        for nd in jsonData['nodedefs']:
            self.addNodeDefinition(BehaviorNodeDefinition(nd))
        return self

    def toJSON(self):
        ret = {}
        ret['name'] = self.name
        defnode = []
        for n in self.nodeDefitions.values():
            defnode.append(n.toJSON())
        ret['nodedefs'] = defnode
        return ret


class BehaviorTree():

    def __init__(self):
        self.root = None
        self.name = "Unnamed"
        self.isSubtree = False

    def debugPrint(self):
        if self.root != None:
            self.root.debugPrint(0)

    def flattenTree(self):
        return self.root.flatten()

    # fromJSON and toJSON
    def fromJSON( self, jsondata):
        return self

    def getTotalNodes(self):
        ret = 0
        curnode = self.root
        while curnode != None:
            ret += curnode.countChild()
            curnode = curnode.next
        return ret

    # Add method to merge btree and extract subtree
    # CreateFromNode
    # InsertInto

class BehaviorBank():

    def __init__(self):
        self.name = "Unnamed"
        self.nodebanks = {}

    def addBank( self, banknumber, btree ):
        self.nodebanks[banknumber] = btree

    def getSubtree(self, subname):
        """
        Look inside for a tree name subname 
        Return a tuple with the bank number and btree
        """
        return (None, None)
