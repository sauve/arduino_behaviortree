/* controls.cpp
 * by Germain Sauvé
 * 
 * IN DEVELOPMENT - NOT COMPLETE
 * 
 */

#include "controls.h"



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
