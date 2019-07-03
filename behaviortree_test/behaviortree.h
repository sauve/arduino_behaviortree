/* behaviortree.h
 * by Germain Sauvé
 * 
 * IN DEVELOPMENT - NOT COMPLETE
 * 
 */

#ifndef __BEHAVIOR_TREE__
#define __BEHAVIOR_TREE__

#include <Arduino.h>

#define __DEBUG__


#ifndef __MAXBEHAVIORTREENODE__
#define __MAXBEHAVIORTREENODE__ 32
#endif

#ifndef __MAXBTREEDEPTHVISITOR__
#define __MAXBTREEDEPTHVISITOR__ 16
#endif

#ifndef __MAXBBELEMENTS__
#define __MAXBBELEMENTS__ 16
#endif

#ifndef __MAXBEVENTS__
#define __MAXBEVENTS__ 4
#endif

#ifndef __MAXBSCHEDULENODE__
#define __MAXBSCHEDULENODE__ 8
#endif

// list of basic node type

// list of node status

// composite

#define BEHAVE_NOTYPE 0
#define BEHAVE_SEQUENCE 1
#define BEHAVE_SELECTOR 2
#define BEHAVE_RANDOM 3
#define BEHAVE_PARALLEL 4
#define BEHAVE_LOOP 5
// decotator
#define BEHAVE_SUCCEEDER 6
#define BEHAVE_INVERTER 7
#define BEHAVE_FAIL 8

// Proxy, Delete, autre 
#define BEHAVE_DELETE 10 // Delete subtree on success
#define BEHAVE_PROXY 11

 // Debug 20 - 24
#define BEHAVE_DEBUGPRINT 20
#define BEHAVE_DELAY 21

// Event 30 - 39


// custom actions 40+



// Node status
#define NODE_STATUS_UNTOUCH 0
#define NODE_STATUS_SUCCESS 1
#define NODE_STATUS_FAILURE 2
#define NODE_STATUS_RUNNING 3
#define NODE_STATUS_DELETE 4
#define NODE_STATUS_EVENTDRIVEN 5

#define BEHAVE_NODE_NO_INDEX 255


// Blackboard constants
#define BLACKBOARD_NOKEY 255

class BlackBoard
{
protected:
  int elements[__MAXBBELEMENTS__];
  byte elemKey[__MAXBBELEMENTS__];
  byte elemState[__MAXBBELEMENTS__];
public:
  void init();
  int setNewElement( int value );
  boolean releaseElement( int key);
  boolean hasKey(int key );
  int get( int key );
  boolean set(int key, int value );
  boolean forceElementValue( int key, int value );
};



class BehaviorEvent
{
public:
  byte type;
  int data;
};

class BehaviorBank
{
  int totalElements;
  const byte* behaviorIndexesSizes;
  const int* bheaviorIndexes;
  const char* behaviorDatas;
public:
  void init( const byte* sizes, const int* indexes, const char* datas, int total );
  byte getNbrNodes( int idx );
  char* getDataPtr( int idx );
};

class BehaviorTreeNode
{
protected:
  
public:
  int data;
  byte type;
  byte state;
  byte child;
  byte next;

  void init( byte type, int data);
  void clear();

  byte getState()
  {
    return this->state;
  }
  byte getPriority()
  {
    return 0;  
  }
  void setState(byte state)
  {
    this->state  = state;
  }
  void setPriority(byte priority)
  {
    return;
  }

  #ifdef __DEBUG__
  void debugPrint(byte depth);
  #endif 
};


class BehaviorTreeVisitor
{
protected:
  BehaviorTreeNode* nodes;
  byte maxNode;
  byte endnode;
  byte currentNode;
  byte depthvisited[__MAXBTREEDEPTHVISITOR__];
  byte depth;
public:
  BehaviorTreeVisitor();
  BehaviorTreeVisitor(BehaviorTreeNode* n, byte maxn, byte start);
  void init(BehaviorTreeNode* n, byte maxn, byte start);
  
  byte getIndex()
  {
    return currentNode;
  }
  
  byte getDepth()
  {
    return this->depth;
  }
  
  boolean hasReachEnd();
  
  boolean moveNext();
  boolean moveUp();
  boolean moveDown();
  boolean moveToChild( byte idx );

  boolean hasNext();
  boolean hasChild();

  BehaviorTreeNode* current()
  {
    return &(this->nodes[this->currentNode]);
  }

  byte getChildLength();
};


class BehaviorTree
{
protected:
  BehaviorTreeNode nodes[__MAXBEHAVIORTREENODE__];
  byte freeNodes[__MAXBEHAVIORTREENODE__];
  byte nbrFree;
  byte root;

  BehaviorEvent events[__MAXBEVENTS__];
  byte scheduleNodes[__MAXBSCHEDULENODE__];

  int popFree();
  void PushFree(int idx);

  boolean fillSubNodeState(byte node, byte state);
  boolean fillSubNodePriority(byte node, byte priority);
  
  void deleteNode(byte parent, byte node);
public:
  void init();


  int setRoot(byte type, int data);
  // Insertion
  int addChild( byte parent, byte type, int data );
  int addNext( byte previous, byte type, int data );

  boolean deserialize( byte nodeparent, const byte* data );
  boolean deserialize_flash(byte nodeparent, byte* data );

  boolean fillSubTreeState(byte parent, byte state);
  boolean fillSubTreePriority(byte parent, byte priority);

  // Visitor
  void initVisitor( BehaviorTreeVisitor& visitor );


  // management
  boolean deleteChildNode( byte parent, byte node);
  byte getFreeNodes()
  {
    return this->nbrFree;
  }
  void clean();

#ifdef __DEBUG__
  void debugPrintNode(byte node, byte depth);
  void debugPrint();
#endif 
};


#endif
