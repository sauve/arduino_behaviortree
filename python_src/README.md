# Installation

* Tested on Python 3.7.1
* Create Virtual Environment

## dependancies

* Use Flask 
* File requirements.txt 

## Configuration

File arduinoconfig.json has the COM port and baudrate to use for communication with the board

# running

* Activate virtual environment
* Python Path/To/Main

# service URL

* /reset : Will close and reopen the com port
* /config : display the current config
* /btree : displat the last Behavior Tree received
* /update : Read the state from the arduino Serial