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
  if (!this->newObject)
  {
    Serial.print(",");
  }
  if (this->hadwrittenValue)
  {
    Serial.print(",");
  }
  Serial.print("\"");
  Serial.print(name);
  Serial.print("\": ");
  this->hadwrittenValue = true;
  this->nameWritten = true;
}

void SerialJSONWriter::writeStartArray()
{
  Serial.print("[");
  this->arraydepth++;
}

void SerialJSONWriter::writeStopArray()
{
  Serial.print("]");
  this->arraydepth--;
  this->nameWritten = false;
}

void SerialJSONWriter::writeStringValue( const char* str)
{
  // should encode escape caracters
  Serial.print("\"");
  Serial.print(str);
  Serial.print("\"");
  this->nameWritten = false;
}

void SerialJSONWriter::writeNumber( int value )
{
  Serial.print(value);
  this->nameWritten = false;
}

void SerialJSONWriter::writeNumber( byte value )
{
  Serial.print(value);
  this->nameWritten = false;
}

void SerialJSONWriter::writeNumber( float value )
{
  Serial.print(value);
  this->nameWritten = false;
}

void SerialJSONWriter::writeBoolean( bool value )
{
  Serial.print(value);
  this->nameWritten = false;
}

void SerialJSONWriter::writeStartObject()
{
   if (!this->nameWritten && !this->newObject )
   {
    Serial.print(",");
   }
   Serial.print("{");
   this->newObject = true;
   this->hadwrittenValue = false;
}

void SerialJSONWriter::writeStopObject()
{
   Serial.print("}");
   this->newObject = false;
   this->hadwrittenValue = true;
   this->nameWritten = false;
}
