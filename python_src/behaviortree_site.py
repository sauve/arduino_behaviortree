"""


"""

from flask import Flask, render_template
from ardu_behavior import Node, BehaviorTree
from arducomm import SerialArduino
from arduinoboard import ArduinoBoard

app = Flask(__name__)
arduino = ArduinoBoard()
# arduino.connect()

@app.route('/')
def index():
    return render_template('index.html', titre="Arduino web connection")

    
@app.route('/connect')
def connect():
    arduino.connect()
    return 'Arduino Behavior tree Site'

@app.route('/disconnect')
def disconnect():
    arduino.disconnect()
    return 'Arduino Behavior tree Site'


@app.route('/serial')
def serialtest():
    ser = SerialArduino()
    ser.open('COM5', 57600)
    ret = ser.readLine()
    ret += ser.readLine()
    ser.close()
    return ret

@app.route('/update')
def updatearduino():
    print("Update") 
    ret = arduino.serial.readLine()
    return ret

@app.route('/debug')
def debugbtree():
    """
    Create Btree than debut print and call template to render with bullet point
    """
    tree = BehaviorTree()
    tree.root = Node()
    tree.root.setup("sequence", 1, True)
    n = Node().setup("DebugPrint", 20, False)
    tree.root.child = n
    tree.debugPrint()
    btrees = tree.flattenTree()
    return render_template('btree.html', titre="Behavior tree", btrees=btrees)


@app.route('/btree')
def getBehaviorTree():
    """
    Serialise the behavior tree in json?
    """
    btrees = ['sequence','parallel','loop','random','selector','proxy']
    return render_template('btree.html', titre="Behavior tree", btrees=btrees)


if __name__ == "__main__":
    app.run(debug=True)
