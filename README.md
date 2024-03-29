# arduino_behaviortree
behavior tree simple example

Simple project implementing a behavior tree with the arduino ATMEGA and SAMD architecture.

# Code

## Project structure

- behaviortree_test : Arduino project
- python : Python utilty code
- doc : autogenerated documentation

## Compilation

Tested with Arduino IDE 1.8.5

## Main classes

* BehaviorTreeNode
* BehaviorTreeVisitor
* BehaviorTree
* BlackBoard
* BehaviorHandler

### BehaviorTreeNode

Encapsulate the basic element of a node. Methodes for initialization, debug and memory management.

### BehaviorTreeVisitor

Use to navigate through the node hierarchically. Use an list to maintain the traversal memory wihout allocating memory dynamically.

### BehaviorTree

Contains a pool of a finite amount of BehaviorNode and methodes to add an remove them.  

### BlackBoard

Contains and managed a finite amount of data slots accessible via a key in a form of a number between 0 and 254.

### BehaviorHandler

Implement node  processing based on the type and data properties.

# Standard Behavior nodes 

* Composite
	* Sequence
	* Loop
	* Random
	* Parallel
	* Selector
* Decorator
	* Inverter
	* Success
	* Failure
* Blackboard
	* SetBBValue
	* ClearBBValue
	* WaitForBBValue 
	* HasBBKey
* Events -- IN DVELOPMENT
	* WaitForEvent
	* WaitForEventTimeout
	* InRecentEvents
	* RaiseEvent
* Specialized
	* Proxy



# Processing of Event and subtrees
To help the behavior to respond more quickly and optimse the main traversal, some node can't be triggereg when event are raised.

When a WaitForEvent and WaitForEventTimoeut node are ticked for the first time, they are added in the tree schedular and their status set to NODE_STATUS_EVENTDRIVEN and all compsite and decorator node won't proces them. When an event is added to the tree, a flag is set to process the schedule nodes. This can be call more often than the full tree processing.

These node are a special kind of decorator since they will excecute their children when the event is raised. They can also take the SUCCESS state if no children is set but the event is raised. For the timeout version, it will take the FAILURE state on timeout.

# Serial communication

If a new line start with ", it is considered the start of a json object. If there isn't a : and { on the same line, than the object is rejected and will wait for the beginning of another




