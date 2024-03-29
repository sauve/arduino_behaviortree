/* behaviortree_tess
 * by Germain Sauvé
 * 
 * 
 * IN DEVELOPMENT - NOT COMPLETE
 */


// Use led on pin 13 and push buttons on pin A0 and A1
#include "behaviortree.h"
#include "controls.h"


const PROGMEM byte BBankSizes[] = {3, 3};
const PROGMEM int BBankIndexes[] = { 0, 12 };
const PROGMEM char BBankData[] = {1, 0, 0, 2, 20, 11, 0, 1, 20, 22, 0, 0, 1, 0, 0, 2, 20, 33, 0, 1, 20, 44, 0, 0};

Button button1, button2;
LED led1;
Alarms alamrs;

#define Behavior_SetTimer 40
#define Behavior_WaitTimer 41
#define Behavior_SetLED 42


class BehaviorHandler
{
  BehaviorTree b_tree;
  BehaviorTreeVisitor visitor;
  BlackBoard blackboard;
  BehaviorBank bBank;
public:
  void init();
  BehaviorTree* getBehaviorTree();
  BlackBoard* getBlackBoard();
  boolean ProcessTree( int maxTime );  
  boolean ProcessScheduler();
  void debugPrint();


  byte Tick_NoType( byte tickType );
  byte Tick_Sequence( boolean tickType );
  byte Tick_Selector( boolean tickType );
  byte Tick_Random( boolean tickType );
  byte Tick_Parallel( boolean tickType );
  byte Tick_Loop( boolean tickType );

  // decorator
  byte Tick_Succeeder( boolean tickType );
  byte Tick_Inverter( boolean tickType );
  byte Tick_Fail( boolean tickType );
  byte Tick_Delete( boolean tickType );
  byte Tick_Proxy( boolean tickType );

  // blackboard
  byte Tick_SetBBValue( boolean tickType );
  byte Tick_ClearBBValue( boolean tickType );

  // event
  byte Tick_WaitForEvent( boolean tickType );
  byte Tick_WaitForEventTimeout( boolean tickType );
  byte Tick_InRecentEvent( boolean tickType );
  byte Tick_RaiseEvent( boolean tickType );

  // debug
  byte Tick_DebugPrint( boolean tickType );
  byte Tick_Delay( boolean tickType );

  // Other custom elements
  byte Tick_LED( boolean tickType );

  byte processNode(boolean tickType);
};


void BehaviorHandler::init()
{
  bBank.init( BBankSizes, BBankIndexes, BBankData, 2 );
  blackboard.init();
}


BehaviorTree* BehaviorHandler::getBehaviorTree()
{
  return &b_tree;
}


BlackBoard* BehaviorHandler::getBlackBoard()
{
  return &blackboard;
}



boolean BehaviorHandler::ProcessTree( int maxTime )
{
  b_tree.initVisitor(visitor);
  boolean canNext = true;
  while( canNext )
  {
    if ( visitor.current()->getState() == NODE_STATUS_UNTOUCH || visitor.current()->getState() == NODE_STATUS_RUNNING || visitor.current()->getState() == NODE_STATUS_EVENTRAISED )
    {
      this->processNode( false );
    }
    canNext = visitor.moveNext();
  }
  return false;
}


boolean BehaviorHandler::ProcessScheduler()
{
  // for each node in the schedular with RAISED_EVENT status
  // process this node as root with no next  
  return false;
}

void BehaviorHandler::debugPrint()
{
  b_tree.debugPrint();
  blackboard.debugPrint();
}

byte BehaviorHandler::Tick_NoType( byte tickType )
{
  byte ret = this->visitor.current()->getState();
  // set to SUCCESS
  if (ret == NODE_STATUS_RUNNING)
  {
    this->visitor.current()->setState(NODE_STATUS_SUCCESS);
    this->visitor.moveUp();
    return NODE_STATUS_SUCCESS;
  }
  else if (ret == NODE_STATUS_UNTOUCH)
  {
    this->visitor.current()->setState(NODE_STATUS_RUNNING);
    this->visitor.moveUp();
    return NODE_STATUS_RUNNING;
  }
  // heu?!
  this->visitor.moveUp();
  return ret;
}

byte BehaviorHandler::Tick_Sequence( boolean tickType )
{
  BehaviorTreeNode* mePtr = this->visitor.current();
  byte curstate = mePtr->getState();
  // In sequence, start child if not running, continue on ly if Succes
  if (curstate== NODE_STATUS_UNTOUCH || curstate == NODE_STATUS_RUNNING)
  {
    mePtr->setState(NODE_STATUS_RUNNING);
    boolean end = false;
    if ( this->visitor.moveDown() )
    {
      while( !end )
      {
        curstate = this->visitor.current()->getState();
        switch( curstate )
        {
          case NODE_STATUS_EVENTDRIVEN:
            // Wait to be triggered by event or timeout
            end = true;
            break;
          case NODE_STATUS_UNTOUCH:
          case NODE_STATUS_RUNNING:
          case NODE_STATUS_EVENTRAISED:
            this->processNode(tickType);
            end = true;
            break;
          case NODE_STATUS_SUCCESS:
            end = !this->visitor.moveNext();
            if ( end )
            {
              // if seuqnece at end and all success, set state success
              mePtr->setState(NODE_STATUS_SUCCESS);
            }
            break;
          default:
            end = true;
            mePtr->setState(NODE_STATUS_FAILURE);
            break;
        }
      }
    }
  this->visitor.moveUp();
  return mePtr->getState();
  }
}

byte BehaviorHandler::Tick_Selector( boolean tickType )
{
  BehaviorTreeNode* mePtr = this->visitor.current();
  byte curstate = mePtr->getState();
  // In sequence, start child if not running, continue on ly if Fail, stop on Success
  if (curstate == NODE_STATUS_UNTOUCH || curstate == NODE_STATUS_RUNNING)
  {
    mePtr->setState(NODE_STATUS_RUNNING);
    boolean end = false;
    if ( this->visitor.moveDown() )
    {
      while( !end )
      {
        curstate = this->visitor.current()->getState();
        switch( curstate )
        {
          case NODE_STATUS_EVENTDRIVEN:
            // Wait to be triggered by event or timeout
            this->visitor.moveUp();
            end = true;
            break;
          case NODE_STATUS_UNTOUCH:
          case NODE_STATUS_RUNNING:
          case NODE_STATUS_EVENTRAISED:
            this->processNode(tickType);
            end = true;
            break;
          case NODE_STATUS_FAILURE:
            end = !this->visitor.moveNext();
            if ( end )
            {
              // if seuqnece at end and all success, set state success
              mePtr->setState(NODE_STATUS_FAILURE);
            }
            break;
          default:
            mePtr->setState(NODE_STATUS_SUCCESS);
            end = true;
            break;
        }
      }
    }
  this->visitor.moveUp();
  return mePtr->getState();
  }
}

byte BehaviorHandler::Tick_Random( boolean tickType )
{
  BehaviorTreeNode* mePtr = this->visitor.current();
  byte curstate = mePtr->getState();
  // Select one child at random, execute et return end state
  if ( curstate == NODE_STATUS_UNTOUCH || curstate == NODE_STATUS_RUNNING)
  {
    if ( curstate == NODE_STATUS_UNTOUCH )
    {
      //select at random one of the child
      int maxC = this->visitor.getChildLength();
      // set child index in data
      this->visitor.current()->data = random(maxC);
      //Serial.print("selrandom:");
      //Serial.println(this->visitor.current()->data);
    }
    mePtr->setState(NODE_STATUS_RUNNING);
    // move to child in data
    int childIdx = this->visitor.current()->data;
    if ( this->visitor.moveToChild(childIdx))
    {
      // getstate, if eventdriven, do nothing, else process and check return
      byte childStatus = this->visitor.current()->getState();
      switch( childStatus )
      {
        case NODE_STATUS_EVENTDRIVEN:
          // no process since waiting for event
          this->visitor.moveUp();
          break;
        case NODE_STATUS_EVENTRAISED:
        case NODE_STATUS_UNTOUCH:
        case NODE_STATUS_RUNNING:
          childStatus = this->processNode(tickType);
          if ( childStatus == NODE_STATUS_FAILURE || childStatus == NODE_STATUS_SUCCESS )
          {
            mePtr->setState(childStatus);
          }
          break;
        case NODE_STATUS_FAILURE:
        case NODE_STATUS_SUCCESS:
            mePtr->setState(childStatus);
            this->visitor.moveUp();
            break;
      }
    }
  }
  this->visitor.moveUp();
  return mePtr->getState();
}

byte BehaviorHandler::Tick_Parallel( boolean tickType )
{
  // Start all child, return only when all child end state, Success if all success
    BehaviorTreeNode* mePtr = this->visitor.current();
    mePtr->setState(NODE_STATUS_RUNNING);
    boolean end = false;
    boolean oneRunning = false;
    if ( this->visitor.moveDown() )
    {
      while( !end )
      {
        // if eventdriven nedd proce, if wiatforvent, do nothing, use switch
        if ( this->visitor.current()->getState() == NODE_STATUS_EVENTRAISED || this->visitor.current()->getState() == NODE_STATUS_UNTOUCH || this->visitor.current()->getState() == NODE_STATUS_RUNNING )
        {
          oneRunning = true;
          this->processNode(tickType);
          end = !this->visitor.moveNext();
        }
        else if (this->visitor.current()->getState() == NODE_STATUS_EVENTDRIVEN )
        {
          // don't process node waiting for event but count it has runnning
          oneRunning = true;
          end = !this->visitor.moveNext();
        }
        else
        {
          end = !this->visitor.moveNext();
        }
      }
    }
  if ( !oneRunning ) 
  {
     mePtr->setState(NODE_STATUS_SUCCESS);
  }
  this->visitor.moveUp();
  return mePtr->getState();
}

// TODO: Event suport
byte BehaviorHandler::Tick_Loop( boolean tickType )
{
  BehaviorTreeNode* mePtr = this->visitor.current();
  byte meIdx =  this->visitor.getIndex();
  if ( this->visitor.current()->getState() == NODE_STATUS_UNTOUCH )
  {
    // copy lower part of data in hight part
    int counter = mePtr->data & 0x00FF;
    mePtr->data = (mePtr->data & 0x00FF) |  (counter << 8);
    // set status to running  
    mePtr->setState(NODE_STATUS_RUNNING);
  }
  int nbriter = (mePtr->data & 0xFF00) >> 8;
  if (  mePtr->data != 0 && nbriter == 0 )
  {
    // finish ietration  
    mePtr->setState(NODE_STATUS_SUCCESS);
  }
  else
  {
   // Call child, when end state of child, restart
    this->visitor.moveDown(); 
    // check current state, if can process then process, after set state
    byte childStatus = this->processNode(tickType);
    if ( childStatus == NODE_STATUS_SUCCESS || childStatus == NODE_STATUS_FAILURE )
    {
      // restart?
      if ( mePtr->data != 0 )
      {
        nbriter -= 1;
        if ( nbriter > 0 )
        {
          this->b_tree.fillSubTreeState( meIdx, NODE_STATUS_UNTOUCH );  
        }
        mePtr->data = (mePtr->data & 0x00FF) |  (nbriter << 8);
      }
      else
      {
         // endless loop
        this->b_tree.fillSubTreeState( this->visitor.getIndex(), NODE_STATUS_UNTOUCH );   
      }
      // no need to process, so return to me
      this->visitor.moveUp();
    }
  }
  
  // end stsate when iteration end or never 
  this->visitor.moveUp();
  return mePtr->getState();
}

  // decorator
byte BehaviorHandler::Tick_Succeeder( boolean tickType )
{
  // call child, when end state, set to SUCCESS
  BehaviorTreeNode* mePtr = this->visitor.current();
  if (mePtr->getState() == NODE_STATUS_RUNNING || mePtr->getState() == NODE_STATUS_UNTOUCH)
  {
    mePtr->setState(NODE_STATUS_RUNNING);
    if ( this->visitor.moveDown() )
    {
      // processnode child, if true, set state inverse of child
      byte childStatus = this->visitor.current()->getState();
      switch(childStatus )
      {
        case NODE_STATUS_EVENTDRIVEN:
          this->visitor.moveUp();
          break;
        case NODE_STATUS_EVENTRAISED:
        case NODE_STATUS_RUNNING:
        case NODE_STATUS_UNTOUCH:
          childStatus = this->processNode(tickType);
          if ( childStatus == NODE_STATUS_SUCCESS || childStatus == NODE_STATUS_FAILURE)
          {
            mePtr->setState(NODE_STATUS_SUCCESS);
          }
          break;
        case NODE_STATUS_SUCCESS:
        case NODE_STATUS_FAILURE:
           mePtr->setState(NODE_STATUS_SUCCESS);
           this->visitor.moveUp();
          break;
      }
    }
    else
    {
      mePtr->setState(NODE_STATUS_FAILURE);
    }
  }
  this->visitor.moveUp();
  return mePtr->getState();
}

byte BehaviorHandler::Tick_Inverter( boolean tickType )
{
  // when end state of child, inverte success for failure and failure fo success
  BehaviorTreeNode* mePtr = this->visitor.current();
  if (mePtr->getState() == NODE_STATUS_RUNNING || mePtr->getState() == NODE_STATUS_UNTOUCH)
  {
    mePtr->setState(NODE_STATUS_RUNNING);
    if ( this->visitor.moveDown() )
    {
      // processnode child, if true, set state inverse of child
      byte childStatus = this->visitor.current()->getState();
      switch(childStatus )
      {
        case NODE_STATUS_EVENTDRIVEN:
          this->visitor.moveUp();
          break;
        case NODE_STATUS_EVENTRAISED:
        case NODE_STATUS_RUNNING:
        case NODE_STATUS_UNTOUCH:
          childStatus = this->processNode(tickType);
          if ( childStatus == NODE_STATUS_SUCCESS)
          {
            mePtr->setState(NODE_STATUS_FAILURE);
          }
          else if (childStatus == NODE_STATUS_FAILURE )
          {
            mePtr->setState(NODE_STATUS_SUCCESS);
          }
          break;
        case NODE_STATUS_SUCCESS:
           mePtr->setState(NODE_STATUS_FAILURE);
           this->visitor.moveUp();
          break;
        case NODE_STATUS_FAILURE:
           mePtr->setState(NODE_STATUS_SUCCESS);
           this->visitor.moveUp();
          break;
      }
    }
    else
    {
      mePtr->setState(NODE_STATUS_FAILURE);
    }
  }
  this->visitor.moveUp();
  return mePtr->getState();
}

// TODO: Event suport
byte BehaviorHandler::Tick_Fail( boolean tickType )
{
  // when end state of child, set to fail
  byte ret = NODE_STATUS_UNTOUCH;
  BehaviorTreeNode* mePtr = this->visitor.current();
  if (mePtr->getState() == NODE_STATUS_RUNNING || mePtr->getState() == NODE_STATUS_UNTOUCH )
  {
    mePtr->setState(NODE_STATUS_RUNNING);
    if ( this->visitor.moveDown() )
    {
      // processnode child, if true, set state inverse of child
      byte childStatus = this->processNode(tickType);
      if ( childStatus == NODE_STATUS_SUCCESS || childStatus == NODE_STATUS_FAILURE)
      {
        mePtr->setState(NODE_STATUS_FAILURE);
      }
    }
    else
    {
      mePtr->setState(NODE_STATUS_FAILURE);
    }
  }
  this->visitor.moveUp();
  return mePtr->getState();
}

byte BehaviorHandler::Tick_Delete( boolean tickType )
{
  // When child reach end state, set state with child then delete all child subtree
  this->visitor.moveUp();
  return NODE_STATUS_FAILURE;
}

// TODO: Event suport
byte BehaviorHandler::Tick_Proxy( boolean tickType ) 
{
  boolean ret = false;
  // On start deserialise from data value as ID. When child end, take state and delete child
  BehaviorTreeNode* mePtr = this->visitor.current();
  int meIdx = this->visitor.getIndex();
  if ( mePtr->getState() == NODE_STATUS_UNTOUCH)
  {
    // deserialize
    // require BehaviorBank and index in data
    if ( this->bBank.getNbrNodes(mePtr->data) <= this->b_tree.getFreeNodes() )
    {
      char* behavSer = this->bBank.getDataPtr( mePtr->data );
      if ( this->b_tree.deserialize_flash( this->visitor.getIndex(), behavSer ) == false )
      {
        // fail to deserialise
        mePtr->setState(NODE_STATUS_FAILURE);
      }
      else
      {
        // set to running
        mePtr->setState(NODE_STATUS_RUNNING);
      }
    }
    else
    {
      mePtr->setState(NODE_STATUS_FAILURE);
    }
  }
  if ( mePtr->getState() == NODE_STATUS_RUNNING)
  {
    if ( this->visitor.moveDown() )
    {
      BehaviorTreeNode* childPtr = this->visitor.current();
      byte childIdx = this->visitor.getIndex();
      byte childstatus = this->processNode(tickType);
      // if child success or fail
      if ( childstatus == NODE_STATUS_FAILURE || childstatus == NODE_STATUS_SUCCESS )
      {
          mePtr->setState(childstatus);
          this->b_tree.deleteChildNode(meIdx, childIdx);
      }
    }
  }
  this->visitor.moveUp();
  return mePtr->getState();
}
  
byte BehaviorHandler::Tick_SetBBValue( boolean tickType )
{
  // Set value(data[0]) at key ( data[1] )
  byte* bytedata = (byte*)&(this->visitor.current()->data);
  if ( this->blackboard.set(bytedata[1], bytedata[0]) )
  {
    this->visitor.current()->setState(NODE_STATUS_SUCCESS);
  }
  else
  {
    this->visitor.current()->setState(NODE_STATUS_FAILURE);
  }
  byte ret = this->visitor.current()->getState();
  this->visitor.moveUp();
  return ret;
}

byte BehaviorHandler::Tick_ClearBBValue( boolean tickType )
{
  // Remove from the blackboard the key data[1]
  byte* bytedata = (byte*)&(this->visitor.current()->data);
  this->blackboard.releaseElement(bytedata[1] );
  this->visitor.current()->setState(NODE_STATUS_SUCCESS);
  byte ret = this->visitor.current()->getState();
  this->visitor.moveUp();
  return ret;
}


  byte BehaviorHandler::Tick_WaitForEvent( boolean tickType )
  {
    // should have access to the event ?
    // this will be called when the event is triggered and by the main tree process if is in the rnning state
    BehaviorTreeNode* mePtr = this->visitor.current();
    switch ( mePtr->getState() )
    {
      case NODE_STATUS_UNTOUCH:
         // add to the scheduler
        this->b_tree.addScheduleNode( this->visitor.getIndex() );
        mePtr->setState(NODE_STATUS_EVENTDRIVEN);
        break;
      case NODE_STATUS_EVENTRAISED:
        // triggered by event
         mePtr->setState(NODE_STATUS_RUNNING);
        break;
    }
    if (  mePtr->getState() == NODE_STATUS_RUNNING)
    {
      // if child, process, if not, return SUCCESS
      if ( mePtr->hasChild())
      {
        // process Child 
        if ( this->visitor.moveDown() )
        {
          // processnode child, if true, set state inverse of child
          byte childStatus = this->visitor.current()->getState();
          switch (childStatus )
          {
            case NODE_STATUS_EVENTRAISED:
            case NODE_STATUS_UNTOUCH:
            case NODE_STATUS_RUNNING:
              childStatus = this->processNode(tickType);
              if ( childStatus == NODE_STATUS_SUCCESS || childStatus == NODE_STATUS_FAILURE)
              {
                mePtr->setState(childStatus);
              }
              break;
            case NODE_STATUS_SUCCESS:
            case NODE_STATUS_FAILURE:
              mePtr->setState(childStatus);
              break;
          }
        }
        else
        {
          // unable to go tochild node
          mePtr->setState(NODE_STATUS_FAILURE);
        }
      }
      else
      {
        this->visitor.current()->setState(NODE_STATUS_SUCCESS);
      }
    }
    this->visitor.moveUp();
    return mePtr->getState();
  }

// TODO: Event suport
byte BehaviorHandler::Tick_WaitForEventTimeout( boolean tickType )
{
  // on tick, check if timeout ok, if not return failure
  this->visitor.current()->setState(NODE_STATUS_FAILURE);
  byte ret = this->visitor.current()->getState();
  this->visitor.moveUp();
  return ret;
}

// TODO: Event suport
byte BehaviorHandler::Tick_InRecentEvent( boolean tickType )
{
  // hum, is it really useful
  this->visitor.current()->setState(NODE_STATUS_FAILURE);
  byte ret = this->visitor.current()->getState();
  this->visitor.moveUp();
  return ret;
}
  
// TODO: Event suport
byte BehaviorHandler::Tick_RaiseEvent( boolean tickType )
{
  // add event to b_tree from data ( type, evdata )
  this->visitor.current()->setState(NODE_STATUS_FAILURE);
  byte ret = this->visitor.current()->getState();
  this->visitor.moveUp();
  return ret;
}


byte BehaviorHandler::Tick_DebugPrint( boolean tickType )
{
  // Serial print the data value
  Serial.print(F("Debug print node: "));
  Serial.println(this->visitor.current()->data);
  this->visitor.current()->setState(NODE_STATUS_SUCCESS);
  byte ret = this->visitor.current()->getState();
  this->visitor.moveUp();
  return ret;
}

byte BehaviorHandler::Tick_Delay( boolean tickType )
{
  // dey for the duration of the data value
  delay(this->visitor.current()->data);
  this->visitor.current()->setState(NODE_STATUS_SUCCESS);
  byte ret = this->visitor.current()->getState();
  this->visitor.moveUp();
  return ret;
}



byte BehaviorHandler::Tick_LED( boolean tickType )
{
  led1.setValue(this->visitor.current()->data);
  this->visitor.current()->setState(NODE_STATUS_SUCCESS);
  byte ret = this->visitor.current()->getState();
  this->visitor.moveUp();
  return ret;
}



byte BehaviorHandler::processNode(boolean tickType)
{
  byte ret = NODE_STATUS_UNTOUCH;
  // mega switch on type  
  switch ( this->visitor.current()->type )
  {
    case BEHAVE_NOTYPE:
      ret = this->Tick_NoType(tickType);
      break;
    case BEHAVE_SEQUENCE:
      ret = this->Tick_Sequence(tickType);
      break;
    case BEHAVE_SELECTOR:
      ret = this->Tick_Selector(tickType);
      break;
    case BEHAVE_RANDOM:
      ret = this->Tick_Random(tickType);
      break;
    case BEHAVE_PARALLEL:
      ret = this->Tick_Parallel(tickType);
      break;
    case BEHAVE_LOOP:
      ret = this->Tick_Loop(tickType);
      break;
    case BEHAVE_SUCCEEDER:
      ret = this->Tick_Succeeder(tickType);
      break;
    case BEHAVE_INVERTER:
      ret = this->Tick_Inverter(tickType);
      break;
    case BEHAVE_FAIL:
      ret = this->Tick_Fail(tickType);
      break;
    case BEHAVE_DELETE:
      ret = this->Tick_Delete(tickType);
      break;
    case BEHAVE_PROXY:
      ret = this->Tick_Proxy(tickType);
      break;
    case BEHAVE_SETBBVALUE:
      ret = this->Tick_SetBBValue(tickType);
      break;
    case BEHAVE_CLEARBBVALUE:
      ret = this->Tick_ClearBBValue(tickType);
      break;
    case BEHAVE_WAITFOREVENT:
      ret = this->Tick_WaitForEvent(tickType);
      break;
    case BEHAVE_WAITFOREVENTTIMEOUT:
      ret = this->Tick_WaitForEventTimeout(tickType);
      break;
    case BEHAVE_INRECENTEVENT:
      ret = this->Tick_InRecentEvent(tickType);
      break;
    case BEHAVE_RAISEEVENT:
      ret = this->Tick_RaiseEvent(tickType);
      break;
    case BEHAVE_DEBUGPRINT:
      ret = this->Tick_DebugPrint(tickType);
      break;
    case BEHAVE_DELAY:
      ret = this->Tick_Delay(tickType);
      break;
    // custom values
    case Behavior_SetLED:
      ret = this->Tick_LED(tickType);
      break;
  }
  
  return ret;  
}

BehaviorHandler bt_handler;


void simpleBTreeInit()
{
  // get handler btree
  BehaviorTree* btPtr = bt_handler.getBehaviorTree();
  btPtr->init();
  int root =  btPtr->setRoot( BEHAVE_SEQUENCE, 0);
  int firstprint = btPtr->addChild( root, BEHAVE_DEBUGPRINT, 1122 );
  int secprint = btPtr->addNext( firstprint, BEHAVE_DEBUGPRINT, 2211 );
  int selidx = btPtr->addNext( secprint, BEHAVE_SELECTOR, 0 );
  int invidx = btPtr->addChild( selidx, BEHAVE_INVERTER, 0 );
  int curn = btPtr->addChild( invidx, BEHAVE_DEBUGPRINT, 2233 );
  int succidx = btPtr->addNext( invidx, BEHAVE_SUCCEEDER, 0 );
  int failidx = btPtr->addChild( succidx, BEHAVE_FAIL, 0 );
  curn = btPtr->addChild( failidx, BEHAVE_DEBUGPRINT, 3344 );
  int paralidx = btPtr->addNext( selidx, BEHAVE_PARALLEL, 0 );
  curn = btPtr->addChild( paralidx, BEHAVE_DEBUGPRINT, 1155 );
  curn = btPtr->addNext( curn, BEHAVE_DEBUGPRINT, 2255 );
  curn = btPtr->addNext( curn, BEHAVE_DEBUGPRINT, 3355 );
}

const char serializeBTree[] = { 1, 0, 0, 2, 20, 11, 0, 1, 20, 22, 0, -1, 20, 33, 0, 0 };
const PROGMEM char serializeBTree_flash[] = { 1, 0, 0, 2, 20, 11, 0, 1, 20, 22, 0, -1, 20, 33, 0, 0 };
const PROGMEM char randomBTree_flash[] = { 3, 0, 0, 2, 20, 11, 0, 1, 20, 22, 0, 1, 20, 33, 0, 0 };
const PROGMEM char LoopBTree_flash[] = { 5, 2, 0, 2, 1, 0, 0, 2, 20, 11, 0, 1, 20, 22, 0, 0 };
const PROGMEM char ProxyBTree_flash[] = { 1, 0, 0, 2, 11, 0, 0, 1, 20, 55, 0, 1, 11, 1, 0, 0 };
const PROGMEM char Blackboard_flash[] = { 1, 0, 0, 2, 20, 11, 0, 1, BEHAVE_SETBBVALUE, 10, 3, 1, 20, 22, 0, 1, BEHAVE_CLEARBBVALUE, 0, 3, 0 };

void SimpleDeserializeInit()
{
  BehaviorTree* btPtr = bt_handler.getBehaviorTree();
  btPtr->init();

  btPtr->deserialize(serializeBTree);
}

void SimpleDeserializeInitFlash()
{
  BehaviorTree* btPtr = bt_handler.getBehaviorTree();
  btPtr->init();

  btPtr->deserialize_flash(serializeBTree_flash);
}


void SimpleDeserializeInitRandom()
{
  BehaviorTree* btPtr = bt_handler.getBehaviorTree();
  btPtr->init();

  btPtr->deserialize_flash(randomBTree_flash);
}


void SimpleDeserialiseInit_Loop()
{
  BehaviorTree* btPtr = bt_handler.getBehaviorTree();
  btPtr->init();

  btPtr->deserialize_flash(LoopBTree_flash);
}

void setup() {

  Serial.begin(57600);

  led1.Init(13, 0, false );
  button1.Init(A0, 0, true);
  button2.Init(A1, 0, true);  

  // Init Behavior Tree initial state
  bt_handler.init();


  
  simpleBTreeInit();
}


void testSimpleTree()
{
    // Read input states of buttons
  button1.Update();
  button2.Update();

  simpleBTreeInit();
  // Handle Behavior Tree
  bt_handler.debugPrint();
  delay(4000);
  for ( int iter = 0; iter < 10; ++iter ) 
  {
    bt_handler.ProcessTree(0);
    bt_handler.debugPrint();
    delay(4000);
  }
}


void testDeserialize()
{
    SimpleDeserializeInitFlash();
    bt_handler.debugPrint();
    delay(4000);
     for ( int iter = 0; iter < 10; ++iter ) 
    {
      bt_handler.ProcessTree(0);
      bt_handler.debugPrint();
      delay(4000);
    }
}


void testRandom()
{
  SimpleDeserializeInitRandom();
    bt_handler.debugPrint();
    delay(4000);
     for ( int iter = 0; iter < 10; ++iter ) 
    {
      bt_handler.ProcessTree(0);
      bt_handler.debugPrint();
      delay(4000);
    }
}

void testLoop()
{
  SimpleDeserialiseInit_Loop();
    bt_handler.debugPrint();
    delay(4000);
     for ( int iter = 0; iter < 10; ++iter ) 
    {
      bt_handler.ProcessTree(0);
      bt_handler.debugPrint();
      delay(4000);
    }
}


void testDelete()
{
  BehaviorTree* btPtr = bt_handler.getBehaviorTree();
  btPtr->init();
  int root =  btPtr->setRoot( BEHAVE_SEQUENCE, 0);
  int firstprint = btPtr->addChild( root, BEHAVE_DEBUGPRINT, 1122 );
  int secprint = btPtr->addNext( firstprint, BEHAVE_DEBUGPRINT, 2211 );
  int selidx = btPtr->addNext( secprint, BEHAVE_SELECTOR, 0 );
  int invidx = btPtr->addChild( selidx, BEHAVE_INVERTER, 0 );
  int curn = btPtr->addChild( invidx, BEHAVE_DEBUGPRINT, 2233 );
  int succidx = btPtr->addNext( invidx, BEHAVE_SUCCEEDER, 0 );
  int failidx = btPtr->addChild( succidx, BEHAVE_FAIL, 0 );
  curn = btPtr->addChild( failidx, BEHAVE_DEBUGPRINT, 3344 );
  int paralidx = btPtr->addNext( selidx, BEHAVE_PARALLEL, 0 );
  curn = btPtr->addChild( paralidx, BEHAVE_DEBUGPRINT, 1155 );
  curn = btPtr->addNext( curn, BEHAVE_DEBUGPRINT, 2255 );
  curn = btPtr->addNext( curn, BEHAVE_DEBUGPRINT, 3355 );

  bt_handler.debugPrint();
  delay(4000);
  
  btPtr->deleteChildNode(root, selidx);
  bt_handler.debugPrint();
  delay(4000);

  for ( int iter = 0; iter < 10; ++iter ) 
    {
      bt_handler.ProcessTree(0);
      bt_handler.debugPrint();
      delay(4000);
    }
}


void testProxy()
{
  BehaviorTree* btPtr = bt_handler.getBehaviorTree();
  btPtr->init();

  btPtr->deserialize_flash(ProxyBTree_flash);
  bt_handler.debugPrint();
  delay(4000);
  for ( int iter = 0; iter < 10; ++iter ) 
  {
    bt_handler.ProcessTree(0);
    bt_handler.debugPrint();
    delay(4000);
  }
}

void testBlackBoard()
{
  BlackBoard bb;
  bb.init();
  bb.set( 15, 25 );
  bb.set( 8, 1024 );
  bb.set( 48, 31877);
  bb.debugPrint();
  bb.releaseElement(8);
  bb.debugPrint();
  boolean test = bb.hasKey(8);
  Serial.print("HasKey:");
  Serial.println(test);
  int val = bb.get(15);
  Serial.print("key 15 = ");
  Serial.println(val);
  // test true or false for method
  delay(3000);
}

void testBlackboardBehavior()
{
   BehaviorTree* btPtr = bt_handler.getBehaviorTree();
  btPtr->init();

  btPtr->deserialize_flash(Blackboard_flash);
  bt_handler.debugPrint();
  delay(4000);
  for ( int iter = 0; iter < 10; ++iter ) 
  {
    bt_handler.ProcessTree(0);
    bt_handler.debugPrint();
    delay(4000);
  }
}

void testBlackboardJSON()
{
  SerialJSONWriter writer;
  BehaviorTree* btPtr = bt_handler.getBehaviorTree();
  BlackBoard* bbPtr = bt_handler.getBlackBoard();
  btPtr->init();

  btPtr->deserialize_flash(Blackboard_flash);
  writer.StartWriter();
  btPtr->outputJSON(&writer);
  writer.StopWriter();
  writer.StartWriter();
  bbPtr->outputJSON(&writer);
  writer.StopWriter();
  delay(4000);
  for ( int iter = 0; iter < 10; ++iter ) 
  {
    bt_handler.ProcessTree(0);
    writer.StartWriter();
    btPtr->outputJSON(&writer);
    writer.StopWriter();
    writer.StartWriter();
    bbPtr->outputJSON(&writer);
    writer.StopWriter();
    delay(4000);
  }
}

void testSerialJSON()
{
  SerialJSONWriter writer;

  writer.WriteObjName("TestObject");
  writer.writeStartObject();
  writer.WriteObjName("val1");
  writer.writeNumber(10);
  writer.WriteObjName("val2");
  writer.writeStartArray();
  writer.writeStartObject();
  writer.WriteObjName("v1");
  writer.writeNumber(20);
  writer.WriteObjName("v2");
  writer.writeStringValue("a string");
  writer.writeStopObject();
  writer.writeStartObject();
  writer.WriteObjName("v1");
  writer.writeNumber(30);
  writer.WriteObjName("v2");
  writer.writeStringValue("another string");
  writer.writeStopObject();
  writer.writeStopArray();
  writer.writeStopObject();

  delay(4000);
}

void loop() {
  // testSimpleTree();
  // testDeserialize();
  // testRandom();
  // testLoop();
  // testDelete();
  // testProxy();
  //testBlackBoard();
  testBlackboardBehavior();
  // testBlackboardJSON();
}
