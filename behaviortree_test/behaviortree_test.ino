/* behaviortree_tess
 * by Germain Sauvé
 * 
 * 
 * IN DEVELOPMENT - NOT COMPLETE
 */


// Use led on pin 13 and push buttons on pin A0 and A1
#include "behaviortree.h"

class LED
{
  byte pin;
  byte value;
  boolean linear;
public:
  void Init( byte pin, byte initalValue, boolean linSupport);
  void setValue(byte value);
};

void LED::Init( byte pin, byte initalValue, boolean linSupport)
{
  
  this->pin = pin;
  pinMode(pin, OUTPUT);
  this->linear = linSupport;
  this->setValue(initalValue);
}

void LED::setValue(byte value)
{
  this->value = value;
  if (this->linear )
  {
    analogWrite(this->pin, this->value);
  }
  else
  {
    digitalWrite(this->pin, this->value == 0 ? LOW : HIGH);
  }
}

class Button
{
  byte pin;
  int lastValue;
  int curValue;
  boolean usePullUp;
public:
  void Init(byte pin, int tolerance, boolean pullup);
  void Update();
  boolean hasChanged();
  boolean IsPressed();
};

void Button::Init(byte pin, int tolerance, boolean pullup)
{
  this->pin = pin;
  this->lastValue = 0;
  this->curValue = 0;
  this->usePullUp = pullup;
  // set le INPUT_PULLUP par defaut, devrait avoir option si pullup est externe
  if (this->usePullUp)
  {
    pinMode(pin, INPUT_PULLUP);
  }
  else
  {
    pinMode(pin, INPUT);
  }
}

void Button::Update()
{
  this->lastValue = this->curValue;
  this->curValue = digitalRead(pin);
}

boolean Button::hasChanged()
{
  return this->lastValue != this->curValue;
}

boolean Button::IsPressed()
{
  if (this->usePullUp)
  {
    return this->curValue != LOW;
  }
  else
  {
    return this->curValue == LOW;
  }
}


class Alarms
{
  unsigned long timers[4];
public:  
  Alarms();
  void setTimer( int idx, long delay );
  boolean IsTimerReach( int idx );
};

Alarms::Alarms()
 {
   timers[0] = 0;
   timers[1] = 0;
   timers[2] = 0;
   timers[3] = 0;
 }
 
void Alarms::setTimer( int idx, long delay )
{
  if ( idx >=0 && idx <= 3 )
  {
    timers[idx] = millis() + delay;
  }
}

boolean Alarms::IsTimerReach( int idx )
{
  if ( idx >=0 && idx <= 3 )
  {
    if (  millis() >= timers[idx])
    {
      return true;  
    }
  }
  return false;
}


Button button1, button2;
LED led1;




#define Behavior_SetTimer 40
#define Behavior_WaitTimer 41

class BehaviorHandler
{
  BehaviorTree b_tree;
  BehaviorTreeVisitor visitor;
public:
  BehaviorTree* getBehaviorTree();
  boolean ProcessTree( int maxTime );  
  void debugPrint();


  boolean Tick_NoType( byte tickType );
  boolean Tick_Sequence( boolean tickType );
  boolean Tick_Selector( boolean tickType );
  boolean Tick_Random( boolean tickType );
  boolean Tick_Parallel( boolean tickType );
  boolean Tick_Loop( boolean tickType );

  // decorator
  boolean Tick_Succeeder( boolean tickType );
  boolean Tick_Inverter( boolean tickType );
  boolean Tick_Fail( boolean tickType );
  boolean Tick_Delete( boolean tickType );
  boolean Tick_Proxy( boolean tickType );

  boolean Tick_DebugPrint( boolean tickType );
  boolean Tick_Delay( boolean tickType );

  // Other custom elements

  byte processNode(boolean tickType);
};

BehaviorTree* BehaviorHandler::getBehaviorTree()
{
  return &b_tree;
}

boolean BehaviorHandler::ProcessTree( int maxTime )
{
  b_tree.initVisitor(visitor);
  boolean canNext = true;
  while( canNext )
  {
    if ( visitor.current()->getState() == NODE_STATUS_UNTOUCH || visitor.current()->getState() == NODE_STATUS_RUNNING )
    {
      this->processNode( false );
    }
    canNext = visitor.moveNext();
  }
  return false;
}


void BehaviorHandler::debugPrint()
{
  b_tree.debugPrint();
}

boolean BehaviorHandler::Tick_NoType( byte tickType )
{
  // set to SUCCESS
  if (this->visitor.current()->getState() == NODE_STATUS_RUNNING)
  {
    this->visitor.current()->setState(NODE_STATUS_SUCCESS);
    this->visitor.moveUp();
    return true;
  }
  else if (this->visitor.current()->getState() == NODE_STATUS_UNTOUCH)
  {
    this->visitor.current()->setState(NODE_STATUS_RUNNING);
    this->visitor.moveUp();
    return false;
  }
  // heu?!
  this->visitor.moveUp();
  return false;
}

  boolean BehaviorHandler::Tick_Sequence( boolean tickType )
{
  // In sequence, start child if not running, continue on ly if Succes
  if (this->visitor.current()->state == NODE_STATUS_UNTOUCH || this->visitor.current()->state == NODE_STATUS_RUNNING)
  {
    BehaviorTreeNode* mePtr = this->visitor.current();
    mePtr->setState(NODE_STATUS_RUNNING);
    boolean end = false;
    if ( this->visitor.moveDown() )
    {
      while( !end )
      {
        if ( this->visitor.current()->getState() == NODE_STATUS_UNTOUCH || this->visitor.current()->getState() == NODE_STATUS_RUNNING )
        {
          this->processNode(tickType);
          end = true;
        }
        else if (this->visitor.current()->getState() == NODE_STATUS_SUCCESS)
        {
          end = !this->visitor.moveNext();
          if ( end )
          {
            // if seuqnece at end and all success, set state success
            mePtr->setState(NODE_STATUS_SUCCESS);
          }
        }
        else
        {
          end = true;
          mePtr->setState(NODE_STATUS_FAILURE);
        }
      }
    }
  this->visitor.moveUp();
  return false;
  }
}

  boolean BehaviorHandler::Tick_Selector( boolean tickType )
{
  // In sequence, start child if not running, continue on ly if Fail, stop on Success
  if (this->visitor.current()->state == NODE_STATUS_UNTOUCH || this->visitor.current()->state == NODE_STATUS_RUNNING)
  {
    BehaviorTreeNode* mePtr = this->visitor.current();
    mePtr->setState(NODE_STATUS_RUNNING);
    boolean end = false;
    if ( this->visitor.moveDown() )
    {
      while( !end )
      {
        if ( this->visitor.current()->getState() == NODE_STATUS_UNTOUCH || this->visitor.current()->getState() == NODE_STATUS_RUNNING )
        {
          this->processNode(tickType);
          end = true;
        }
        else if (this->visitor.current()->getState() == NODE_STATUS_FAILURE)
        {
          end = !this->visitor.moveNext();
          if ( end )
          {
            // if seuqnece at end and all success, set state success
            mePtr->setState(NODE_STATUS_FAILURE);
          }
        }
        else
        {
          mePtr->setState(NODE_STATUS_SUCCESS);
          end = true;
        }
      }
    }
  this->visitor.moveUp();
  // 
  return false;
  }
}

boolean BehaviorHandler::Tick_Random( boolean tickType )
{
  // Select one child at random, execute et return end state
  this->visitor.moveUp();
  return false;
}

  boolean BehaviorHandler::Tick_Parallel( boolean tickType )
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
        if ( this->visitor.current()->getState() == NODE_STATUS_UNTOUCH || this->visitor.current()->getState() == NODE_STATUS_RUNNING )
        {
          oneRunning = true;
          this->processNode(tickType);
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
  return false;
}

boolean BehaviorHandler::Tick_Loop( boolean tickType )
{
  // Call child, when end state of child, restart
  // end stsate when iteration end or never 
  this->visitor.moveUp();
  return false;
}

  // decorator
boolean BehaviorHandler::Tick_Succeeder( boolean tickType )
{
  // call child, when end state, set to SUCCESS
 boolean ret = false;
  BehaviorTreeNode* mePtr = this->visitor.current();
  if (mePtr->getState() == NODE_STATUS_RUNNING || mePtr->getState() == NODE_STATUS_UNTOUCH)
  {
    mePtr->setState(NODE_STATUS_RUNNING);
    if ( this->visitor.moveDown() )
    {
      // processnode child, if true, set state inverse of child
      if ( this->visitor.current()->getState() == NODE_STATUS_SUCCESS || this->visitor.current()->getState() == NODE_STATUS_FAILURE)
      {
        mePtr->setState(NODE_STATUS_SUCCESS);
        ret = true;
      }
      else if (this->visitor.current()->getState() == NODE_STATUS_RUNNING || this->visitor.current()->getState() == NODE_STATUS_UNTOUCH)
      {
        this->processNode(tickType);
      }
    }
    else
    {
      mePtr->setState(NODE_STATUS_FAILURE);
      ret = true;
    }
  }
  this->visitor.moveUp();
  return ret;
}

boolean BehaviorHandler::Tick_Inverter( boolean tickType )
{
  boolean ret = false;
  // when end state of child, inverte success for failure and failure fo success
  BehaviorTreeNode* mePtr = this->visitor.current();
  if (mePtr->getState() == NODE_STATUS_RUNNING || mePtr->getState() == NODE_STATUS_UNTOUCH)
  {
    mePtr->setState(NODE_STATUS_RUNNING);
    if ( this->visitor.moveDown() )
    {
      // processnode child, if true, set state inverse of child
      if ( this->visitor.current()->getState() == NODE_STATUS_SUCCESS)
      {
        mePtr->setState(NODE_STATUS_FAILURE);
        ret = true;
      }
      else if (this->visitor.current()->getState() == NODE_STATUS_FAILURE )
      {
        mePtr->setState(NODE_STATUS_SUCCESS);
        ret = true;
      }
      else if (this->visitor.current()->getState() == NODE_STATUS_RUNNING || this->visitor.current()->getState() == NODE_STATUS_UNTOUCH)
      {
        this->processNode(tickType);
      }
    }
    else
    {
      mePtr->setState(NODE_STATUS_FAILURE);
      ret = true;
    }
  }
  this->visitor.moveUp();
  return ret;
}

boolean BehaviorHandler::Tick_Fail( boolean tickType )
{
  // when end state of child, set to fail
  boolean ret = false;
  BehaviorTreeNode* mePtr = this->visitor.current();
  if (mePtr->getState() == NODE_STATUS_RUNNING || mePtr->getState() == NODE_STATUS_UNTOUCH)
  {
    mePtr->setState(NODE_STATUS_RUNNING);
    if ( this->visitor.moveDown() )
    {
      // processnode child, if true, set state inverse of child
      if ( this->visitor.current()->getState() == NODE_STATUS_SUCCESS || this->visitor.current()->getState() == NODE_STATUS_FAILURE)
      {
        mePtr->setState(NODE_STATUS_FAILURE);
        ret = true;
      }
     else if (this->visitor.current()->getState() == NODE_STATUS_RUNNING || this->visitor.current()->getState() == NODE_STATUS_UNTOUCH)
      {
        this->processNode(tickType);
      }
    }
    else
    {
      mePtr->setState(NODE_STATUS_FAILURE);
      ret = true;
    }
  }
  this->visitor.moveUp();
  return ret;
}

boolean BehaviorHandler::Tick_Delete( boolean tickType )
{
  // When child reach end state, set state with child then delete all child subtree
  this->visitor.moveUp();
  return false;
}

boolean BehaviorHandler::Tick_Proxy( boolean tickType ) 
{
  // On start deserialise from data value as ID. When child end, take state and delete child
  this->visitor.moveUp();
  return false;
}

boolean BehaviorHandler::Tick_DebugPrint( boolean tickType )
{
  // Serial print the data value
  Serial.print(F("Debug print node: "));
  Serial.println(this->visitor.current()->data);
  this->visitor.current()->setState(NODE_STATUS_SUCCESS);
  this->visitor.moveUp();
  return false;
}

boolean BehaviorHandler::Tick_Delay( boolean tickType )
{
  // dey for the duration of the data value
  delay(this->visitor.current()->data);
  this->visitor.current()->setState(NODE_STATUS_SUCCESS);
  this->visitor.moveUp();
  return false;
}

byte BehaviorHandler::processNode(boolean tickType)
{
  byte ret = NODE_STATUS_UNTOUCH;
  // mega switch on type  
  switch ( this->visitor.current()->type )
  {
    case BEHAVE_NOTYPE:
      this->Tick_NoType(tickType);
      break;
    case BEHAVE_SEQUENCE:
      this->Tick_Sequence(tickType);
      break;
    case BEHAVE_SELECTOR:
      this->Tick_Selector(tickType);
      break;
    case BEHAVE_RANDOM:
      this->Tick_Random(tickType);
      break;
    case BEHAVE_PARALLEL:
      this->Tick_Parallel(tickType);
      break;
    case BEHAVE_LOOP:
      this->Tick_Loop(tickType);
      break;
    case BEHAVE_SUCCEEDER:
      this->Tick_Succeeder(tickType);
      break;
    case BEHAVE_INVERTER:
      this->Tick_Inverter(tickType);
      break;
    case BEHAVE_FAIL:
      this->Tick_Fail(tickType);
      break;
    case BEHAVE_DELETE:
      this->Tick_Delete(tickType);
      break;
    case BEHAVE_PROXY:
      this->Tick_Proxy(tickType);
      break;
    case BEHAVE_DEBUGPRINT:
      this->Tick_DebugPrint(tickType);
      break;
    case BEHAVE_DELAY:
      this->Tick_Delay(tickType);
      break;
    // custom value
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
  curn = btPtr->addNext( invidx, BEHAVE_DEBUGPRINT, 3344 );
  curn = btPtr->addNext( selidx, BEHAVE_DEBUGPRINT, 4455 );
}

void setup() {

  Serial.begin(57600);

  led1.Init(13, 0, false );
  button1.Init(A0, 0, true);
  button2.Init(A1, 0, true);  

  // Init Behavior Tree initial state
  simpleBTreeInit();
}

void loop() {
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
