# Basic todo

Tracking of simple todos

## Arduino code

* Handler
	* Processing time statistic
	* review debug outout for easy external parsing
		* header, etc
* Node implementation
	* Optimise Sequence and Selector by usind node data to jump to current child node to run
	* Add simple Blaclboard util node ( setBBValue, clearBBValue )
* Blackboard
	* manage free 
* Event
	* Implement Behavior Event and add list to tree ( last event )
	* Add scheduler / visitor 
	* Process tree with detect event trigger
	* Add multiple root for tree with event trigger
* Pre-emptive
	* support ticktype for forward/backward traversal
	* implement yield in control node
* Tests
	* Code create and use blackboard
	* Code create and use event
	* Code create and use multiple root with event 
## Python code

* Simple BehaveNode with getData and getType for arduino
* Simple ArduinoWriter with complete data and index array
* Serialize and deserialize JSON