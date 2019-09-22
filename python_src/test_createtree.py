"""

By Germain Sauvé

Todo:
- defined data type ( no data, 2 byte, 1 int)
- add methode to load definition form file
- make sure base can load and add custom node def
- serialize in cpp for arduino as behavior bank

"""
import json
from  ardu_behavior import BehaviorTree, Node, BehaviorDefinitions, BehaviorNodeDefinition, BehaviorBank
import arduinowriter

basepath = 'c:/test/arduino/'

def CreateBehaviorTestBTree( ardupath, defpath ):
    writer = arduinowriter.ArduinoWriter()
    writer.open(ardupath)

    # load definition
    bdefs = BehaviorDefinitions()
    with open(defpath, 'rt') as injson:
        jsondata = json.load(injson)
        bdefs.fromJSON(jsondata)
    

    btree = BehaviorTree()
    btree.name = "TestEvent"
    rnode = Node()
    btree.root = bdefs.setupNodeFromDefinition('Sequence', rnode, 'Base loop', 0)
    # should be able to setup with type from definition
    # should be able to create new from defition
    cnode = btree.root.addChild(bdefs.createNodeFromDefinition('DebugPrint', 'First print', 11))
    anode = cnode.addNext(bdefs.createNodeFromDefinition('Sequence', 'Call Alarm Print', 0))
    anode.addChild(bdefs.createNodeFromDefinition('SetAlarm', 'Set alarm for event', 0x0101))
    cnode = anode.addNext(bdefs.createNodeFromDefinition('DebugPrint', 'Second print', 33))
    cnode = cnode.addNext(bdefs.createNodeFromDefinition('WaitForEvent', 'Wait Alarm', 1))
    cnode = cnode.addNext(bdefs.createNodeFromDefinition('DebugPrint', 'Alarm Receive', 44))
    # serialize the btree for arduino
    writer.WriteBehaviorTree(btree, True)
    writer.close()


def CreateAndSaveBehaviorBank( ardupath, defpath ):
    writer = arduinowriter.ArduinoWriter()
    writer.open(ardupath)

    # load definition
    bdefs = BehaviorDefinitions()
    with open(defpath, 'rt') as injson:
        jsondata = json.load(injson)
        bdefs.fromJSON(jsondata)

    btree = BehaviorTree()
    btree.name = "BT1"
    btree.root = bdefs.createNodeFromDefinition('Sequence', 'Base loop', 0)
    cnode = btree.root.addChild(bdefs.createNodeFromDefinition('DebugPrint', 'First print', 11))
    cnode = cnode.addNext(bdefs.createNodeFromDefinition('DebugPrint', 'Alarm Receive', 44))

    bt2 = BehaviorTree()
    bt2.name = "BT2"
    bt2.root = bdefs.createNodeFromDefinition('Sequence', 'Base loop', 0)
    cnode = bt2.root.addChild(bdefs.createNodeFromDefinition('DebugPrint', 'First print', 22))
    cnode = cnode.addNext(bdefs.createNodeFromDefinition('DebugPrint', 'Alarm Receive', 33))

    bbank = BehaviorBank()
    bbank.name = "BBankTest"
    bbank.addBank(0, btree)
    bbank.addBank(1, bt2)

    writer.writeBehaviorBank(bbank, True)
    writer.close()

def CreateBasicDefition( path ):
    bdefs = BehaviorDefinitions()
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Sequence', 1, 0, True, True))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Selector', 2, 0, True, True))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Random', 3, 0, True, True))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Parallel', 4, 0, True, True))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Loop', 5, 0, True, True))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('NoType', 0, 0, False, False))
    # decorator
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Succeeder', 6, 0, True, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Inverter', 7, 0, True, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Fail', 8, 0, True, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Proxy', 11, 0, True, False))
    # Blacboard
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('SetBBValue', 16, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('ClearBBValue', 17, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('WaitForBBValue', 18, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('HasBBKey', 19, 0, False, False))
    # Debug
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('DebugPrint', 20, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Delay', 21, 0, False, False))
    # Event
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('WaitForEvent', 25, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('WaitForEventTimeout', 26, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('InRecentEvent', 27, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('RaiseEvent', 28, 0, False, False))
    # save to file
    with open(path, 'w') as outfile:
        json.dump(bdefs.toJSON(), outfile)

def loadBaseDefAndAddCustom( basedefpath, custompath):
    bdefs = BehaviorDefinitions()
    with open(basedefpath, 'rt') as injson:
        jsondata = json.load(injson)
        bdefs.fromJSON(jsondata)
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('SetLED', 40, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('PlayFrequency', 41, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Turn', 42, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('Move', 43, 0, False, False))
    bdefs.addNodeDefinition(BehaviorNodeDefinition().setup('SetAlarm', 50, 2, False, False))
    with open(custompath, 'w') as outfile:
        json.dump(bdefs.toJSON(), outfile)

if __name__ == '__main__':
    # CreateBasicDefition(basepath + 'basedef.json')
    # loadBaseDefAndAddCustom(basepath + 'basedef.json', basepath + 'customdef.json')
    CreateBehaviorTestBTree( basepath + 'btree.cpp', basepath + 'customdef.json')
    CreateAndSaveBehaviorBank( basepath + 'btreebank.cpp', basepath + 'customdef.json') 