/* serialjsonwriter.cpp
 * by Germain Sauvé
 * 
 * IN DEVELOPMENT - ONLY TEMPORARY
 * 
 */

#include "serialjsonwriter.h"

SerialJSONWriter::SerialJSONWriter()
{
  this->StartWriter();
}

void SerialJSONWriter::StartWriter()
{
  this->arraydepth = 0;
  this->newObject = true;
  this->nameWritten = false;
  this->hadwrittenValue = false;
}

void SerialJSONWriter::StopWriter()
{
  //check for array and object depth
  Serial.println();
}

void SerialJSONWriter::WriteObjName( const char* name)
{
   if (!this->newObject || this->hadwrittenValue)
  {
    Serial.print(',');
  }
  Serial.print('\"');
  Serial.print(name);
  Serial.print("\": ");
  this->hadwrittenValue = true;
  this->nameWritten = true;
}

void SerialJSONWriter::WriteObjName( const __FlashStringHelper* name)
{
  if (!this->newObject || this->hadwrittenValue)
  {
    Serial.print(',');
  }
  Serial.print('\"');
  Serial.print(name);
  Serial.print("\": ");
  this->hadwrittenValue = true;
  this->nameWritten = true;
}

void SerialJSONWriter::writeStartArray()
{
  Serial.print('[');
  this->arraydepth++;
}

void SerialJSONWriter::writeStopArray()
{
  Serial.print(']');
  this->arraydepth--;
  this->nameWritten = false;
}

void SerialJSONWriter::writeStringValue( const char* str)
{
  // should encode escape caracters
  Serial.print('\"');
  Serial.print(str);
  Serial.print('\"');
  this->nameWritten = false;
  this->hadwrittenValue = true;
}

void SerialJSONWriter::writeStringValue( const __FlashStringHelper* str)
{
  // should encode escape caracters
  Serial.print('\"');
  Serial.print(str);
  Serial.print('\"');
  this->nameWritten = false;
  this->hadwrittenValue = true;
}

void SerialJSONWriter::writeNumber( int value )
{
  Serial.print(value);
  this->nameWritten = false;
  this->hadwrittenValue = true;
}

void SerialJSONWriter::writeNumber( byte value )
{
  Serial.print(value);
  this->nameWritten = false;
  this->hadwrittenValue = true;
}

void SerialJSONWriter::writeNumber( float value )
{
  Serial.print(value);
  this->nameWritten = false;
  this->hadwrittenValue = true;
}

void SerialJSONWriter::writeBoolean( bool value )
{
  if ( value )
  {
    Serial.print("true");
  }
  else
  {
    Serial.print("false");
  }
  
  Serial.print(value);
  this->nameWritten = false;
  this->hadwrittenValue = true;
}

void SerialJSONWriter::writeStartObject()
{
   if (!this->nameWritten && !this->newObject )
   {
    Serial.print(',');
   }
   Serial.print('{');
   this->newObject = true;
   this->hadwrittenValue = false;
}

void SerialJSONWriter::writeStopObject()
{
   Serial.print('}');
   this->newObject = false;
   this->hadwrittenValue = true;
   this->nameWritten = false;
}
