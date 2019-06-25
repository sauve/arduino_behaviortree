# Basic todo

Tracking of simple todos

## Arduino code

- Node implementation
 + Add getChildLength and goToChild to the visitor to implement random 
 + Implement Loop by using half of data as nbr other half as variable ( could use balckboard )
 + Implement delete subtree
 + Implement deserialization and proxy node
- Blackboard
 + manage free 
 + manage force index
- Event
 + Implement Behavior Event and add list to tree ( last event )
 + Add scheduler / visitor 
 + Process tree with detect event trigger
 + Add multiple root for tree with event trigger
- Pre-emptive
 + support ticktype for forward/backward traversal
 + implement yield in control node
- Tests
 + Code create and display
 + Code create and process
 + Code create and process/delete
 + Code create and process/delete/proxy
 + Code create and use blackboard
 + Code create and use event
 + Code create and use multiple root with event 
## Python code

- Simple BehaveNode with getData and getType for arduino
- Simple ArduinoWriter with complete data and index array
- Serialize and deserialize JSON