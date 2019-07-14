/* controls.h
 * by Germain Sauvé
 * 
 * IN DEVELOPMENT - NOT COMPLETE
 * 
 */

#ifndef __CONTROLS_H__
#define __CONTROLS_H__

#include <Arduino.h>

class LED
{
  byte pin;
  byte value;
  boolean linear;
public:
  void Init( byte pin, byte initalValue, boolean linSupport);
  void setValue(byte value);
};

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


class Alarms
{
  unsigned long timers[4];
public:  
  Alarms();
  void setTimer( int idx, long delay );
  boolean IsTimerReach( int idx );
};



#endif
