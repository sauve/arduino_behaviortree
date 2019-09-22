"""
arduuinowriter.py

Created be Germain Sauve

"""

class ArduinoWriter():
    """
    Serialise data in .h or .cpp for arduino compiler
    """

    def __init__(self):
        self.output = None

    def open( self, path ):
        self.output = open(path, 'wt')

    def close(self):
        self.output.close()

    def write16bitHEX(self, value):
        i = int(value % 256)
        self.output.write(hex(i))
        self.output.write(', ')
        i = int((value / 256) % 256)
        self.output.write(hex(i))

    def write32bitHEX(self, value):
        i = value % 256
        self.output.write(hex(i))
        self.output.write(', ')
        i = (value / 256) % 256
        self.output.write(hex(i))

    def WriteComment(self, comment):
        for line in comment.splitlines():
            self.output.write('// ')
            self.output.write(line)
            self.output.write('\n')

    def WriteBlockComment(self, comment):
        self.output.write('/*\n')
        for line in comment.splitlines():
            self.output.write(' * ')
            self.output.write(line)
            self.output.write('\n')
        self.output.write(' */\n')

    def writeVariableDef(self, typename, varname, isConst=False, progmem=False):
        """
        Write variable defition with type. const and progmem optional
        """
        if isConst:
            self.output.write('const ')
        self.output.write(typename + " ")
        if progmem:
            self.output.write('progmem ')
        self.output.write(varname + " ")

    def writeArray( self, typename, arrayname,  values, progmem=False ):
        """
        Write array with type and progmem option
        """
        self.writeVariableDef( typename, arrayname, True, progmem)
        self.output.write('[] ={')
        first = True
        for i in values:
            if first == True:
                first = False
            else:
                self.output.write(', ')
            self.output.write(str(i))
        self.output.write('};\n')

    def writeBehaviorBank(self, bbank, progmem):
        self.WriteComment("Behaviore Bank" + bbank.name)
        # write size of element
        # write position of first element
        bbanksSizes = []
        bbanksIndexes = []
        lastIdx = 0
        for v in bbank.nodebanks.values():
            totalnodes = v.getTotalNodes()
            bbanksSizes.append(totalnodes)
            bbanksIndexes.append(str(lastIdx))
            lastIdx += totalnodes
        self.writeArray( 'byte', 'bbank_Sizes_' + bbank.name, bbanksSizes, True )
        self.writeArray( 'int', 'bbank_Indexes_' + bbank.name, bbanksIndexes, True )
        # write elements
        self.writeVariableDef('byte', 'bt_Data_' + bbank.name, True, progmem)
        self.output.write('[] = ')
        self.output.write('{')
        addCome = False
        for v in bbank.nodebanks.values():
            if addCome:
                 self.output.write(', ')
            self.WriteBehaviorNode(v.root, 0)
            self.output.write('0x00')
            addCome = True
        self.output.write('};')
        self.output.write('\n\n')

    def WriteBehaviorNode( self, bnode, depth):
        ret = depth
        self.output.write(hex(bnode.ArduType))
        self.output.write(', ')
        # doit demander le data en int
        self.write16bitHEX(bnode.ArduData)
        self.output.write(', ')
        if bnode.child != None:
             self.output.write('0x02')
             self.output.write(', ')
             childmax = self.WriteBehaviorNode(bnode.child, depth + 1)
             ret = childmax
             if bnode.next != None:
                 self.output.write(str(depth-childmax))
                 self.output.write(', ')
                 self.WriteBehaviorNode(bnode.next, depth)
                 ret = depth
        elif bnode.next != None:
            self.output.write('0x01')
            self.output.write(', ')
            ret = self.WriteBehaviorNode(bnode.next, depth)
        return ret

    def WriteBehaviorTree( self, btree, progmem):
        self.WriteComment("Behaviore Tree " + btree.name)
        self.writeVariableDef( 'byte', 'bt_' + btree.name, True, progmem)
        self.output.write('[] = ')
        self.output.write('{')
        # Parcour l'arbre est serialise l'info et le déplacement
        # sauvegarde data puis si next write sinon c'est plus haut que sera fait le move up
        self.WriteBehaviorNode(btree.root, 0)

        self.output.write('0x00')
        self.output.write('};')
        self.output.write('\n\n')
        

