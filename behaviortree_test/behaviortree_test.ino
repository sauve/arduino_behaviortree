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
  boolean Tick_Failer( boolean tickType );
  boolean Tick_Delete( boolean tickType );
  boolean Tick_Proxy( boolean tickType );

  boolean Tick_DebugPrint( boolean tickType );
  boolean Tick_Delay( boolean tickType );

  // Other custom elements

  boolean processNode();
};

BehaviorTree* BehaviorHandler::getBehaviorTree()
{
  return &b_tree;
}

boolean BehaviorHandler::ProcessTree( int maxTime )
{
  return false;  
}


void BehaviorHandler::debugPrint()
{
  b_tree.debugPrint();
}

boolean BehaviorHandler::Tick_NoType( byte tickType )
{
  // set to SUCCESS
  if (this->visitor.current()->state == NODE_STATUS_RUNNING)
  {
    this->visitor.current()->state = NODE_STATUS_SUCCESS;
    this->visitor.moveUp();
    return true;
  }
  else if (this->visitor.current()->state == NODE_STATUS_UNTOUCH)
  {
    this->visitor.current()->state = NODE_STATUS_RUNNING;
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
  return false;
}

  boolean BehaviorHandler::Tick_Selector( boolean tickType )
{
  // In sequence, start child if not running, continue on ly if Fail, stop on Success
  return false;
}

  boolean BehaviorHandler::Tick_Random( boolean tickType )
{
  // Select one child at random, execute et return end state
  return false;
}

  boolean BehaviorHandler::Tick_Parallel( boolean tickType )
{
  // Start all child, return only when all child end state, Success if all success
  return false;
}

  boolean BehaviorHandler::Tick_Loop( boolean tickType )
{
  // Call child, when end state of child, restart
  // end stsate when iteration end or never 
  return false;
}

  // decorator
  boolean BehaviorHandler::Tick_Succeeder( boolean tickType )
{
  // call child, when end state, set to SUCCESS
  return false;
}

  boolean BehaviorHandler::Tick_Inverter( boolean tickType )
{
  // when end state of child, inverte success for failure and failure fo success
  return false;
}

  boolean BehaviorHandler::Tick_Failer( boolean tickType )
{
  // when end state of child, set to fail
  return false;
}

  boolean BehaviorHandler::Tick_Delete( boolean tickType )
{
  // When child reach end state, set state with child then delete all child subtree
  return false;
}

  boolean BehaviorHandler::Tick_Proxy( boolean tickType ) 
{
  // On start deserialise from data value as ID. When child end, take state and delete child
  return false;
}

  boolean BehaviorHandler::Tick_DebugPrint( boolean tickType )
{
  // Serial print the data value
  return false;
}

  boolean BehaviorHandler::Tick_Delay( boolean tickType )
{
  // dey for the duration of the data value
  return false;
}

boolean BehaviorHandler::processNode()
{
  return false;  
}

BehaviorHandler bt_handler;


void simpleBTreeInit()
{
  // get handler btree
  BehaviorTree* btPtr = bt_handler.getBehaviorTree();
   btPtr->init();
  int root =  btPtr->setRoot( BEHAVE_SEQUENCE, 0);
  int curn = btPtr->addChild( root, BEHAVE_DEBUGPRINT, 1122 );
  btPtr->addNext( curn, BEHAVE_DEBUGPRINT, 2211 );
  btPtr->addNext( root, BEHAVE_DEBUGPRINT, 3333 );
  
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
  
  // Handle Behavior Tree
  bt_handler.debugPrint();
  delay(4000);
}
