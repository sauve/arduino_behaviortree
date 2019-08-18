/* serialjsonwriter.h
 * by Germain Sauvé
 * 
 * IN DEVELOPMENT - ONLY TEMPORARY
 * 
 */

#ifndef __SERIALJSONWRITER_H__
#define __SERIALJSONWRITER_H__

#include <Arduino.h>

class SerialJSONWriter
{
protected:
  boolean newObject;
  boolean nameWritten;
  int arraydepth;
  boolean hadwrittenValue;
public:
  SerialJSONWriter();

  void StartWriter();
  void StopWriter();
  void WriteObjName( const char* name);
  void writeStartArray();
  void writeStopArray();
  void writeStringValue( const char* str);
  void writeNumber( int value );
  void writeNumber( byte value );
  void writeNumber( float value );
  void writeBoolean( bool value );
  void writeStartObject();
  void writeStopObject();
};


#endif