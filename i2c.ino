#include <Wire.h>
#include <EEPROM.h>
#define dc_servo1 43
#define dc_servo2 39

long current_pos[4]

void setup()
{
  Serial.begin(9600);
  Wire.begin();// join i2c bus (address optional for master)

  SetMaxSpeed(dc_servo1, 255);
  SetDamping(dc_servo1, 0);
  SpeedRun(dc_servo1, 0);
  
  SetMaxSpeed(dc_servo2, 255);
  SetDamping(dc_servo2, 0);
  SpeedRun(dc_servo2, 0);

  for(i=0; i<4; i++)
  {
    current_pos[i] = Read_EEPROM(i);
  }
  
  Serial.println("Motor Stopped");
  delay(1000);
}



void loop()
{
  MoveAbs(dc_servo1, 0);
  MoveAbs(dc_servo2, 450);
  delay(10000);
  Write_EEPROM(0, ReadPos(dc_servo1));
  delay(50);
  Write_EEPROM(1, ReadPos(dc_servo2));
  delay(50);
  
  Serial.print("Motor 106 1");
  Serial.print("\t");
  Serial.print(Read_EEPROM(0));
  Serial.println();

  Serial.print("Motor 78 1");
  Serial.print("\t");
  Serial.print(Read_EEPROM(1));
  Serial.println();

  MoveAbs(dc_servo1, 450);
  MoveAbs(dc_servo2, 0);
  delay(10000);
  Write_EEPROM(0, ReadPos(dc_servo1));
  delay(50);
  Write_EEPROM(1, ReadPos(dc_servo2));
  delay(50);

  Serial.print("Motor 106 2");
  Serial.print("\t");
  Serial.print(Read_EEPROM(0));
  Serial.println();

  Serial.print("Motor 78 2");
  Serial.print("\t");
  Serial.print(Read_EEPROM(1));
  Serial.println();
}

void SpeedRun(byte Address, int Speed)
{
  Wire.beginTransmission(Address);
  Wire.write(1);
  Wire.write(Speed         & 0xff);
  Wire.write((Speed >>  8) & 0xff);
  Wire.endTransmission();    // stop transmitting
}

void SetMaxSpeed(byte Address, byte Speed)
{
  Wire.beginTransmission(Address);
  Wire.write(0);
  Wire.write(Speed);
  Wire.write(0);
  Wire.endTransmission();
}

void SetDamping(byte Address, byte Damping)
{
  Wire.beginTransmission(Address);
  Wire.write(2);
  Wire.write(Damping);
  Wire.write(0);
  Wire.endTransmission();
  delay(50);    //Required to store settings
}

void MoveAbs(byte Address, long Position)
{
  Wire.beginTransmission(Address);
  Wire.write(4);
  Wire.write(Position         & 0xff);
  Wire.write((Position >>  8) & 0xff);
  Wire.write((Position >> 16) & 0xff);
  Wire.write((Position >> 24) & 0xff);
  Wire.endTransmission();
}

void MoveRel(byte Address, long Position)
{
  Wire.beginTransmission(Address);
  Wire.write(8);
  Wire.write(Position         & 0xff);
  Wire.write((Position >>  8) & 0xff);
  Wire.write((Position >> 16) & 0xff);
  Wire.write((Position >> 24) & 0xff);
  Wire.endTransmission();
}

void SetPos(byte Address, long Position)
{
  Wire.beginTransmission(Address);
  Wire.write(3);
  Wire.write(Position         & 0xff);
  Wire.write((Position >>  8) & 0xff);
  Wire.write((Position >> 16) & 0xff);
  Wire.write((Position >> 24) & 0xff);
  Wire.endTransmission();
}

long ReadPos(int Address)
{
  int num_bytes;
  byte num[4];
  long Pos = 0;
  //Wire.beginTransmission(Address + 0); // send the slave address of the RMCS-220x and write bit 0

  Wire.beginTransmission(Address + 1); // send I2C address with repeated start and 1 to read
  num_bytes = Wire.requestFrom(Address, 4);

  for (int i = 0; i < num_bytes; i++)
  {
    num[i] = Wire.read();
    Pos = Pos + (num[i] << (8 * i)) & 0xffffffff;
  }

  Wire.endTransmission(); // send I2C stop
  return Pos;
}

long Read_EEPROM(int Motor_Number)
{
  int byte_beg = Motor_Number * 4;

  long four = EEPROM.read(byte_beg);
  long three = EEPROM.read(byte_beg + 1);
  long two = EEPROM.read(byte_beg + 2);
  long one = EEPROM.read(byte_beg + 3);

  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void Write_EEPROM(int Motor_Number, long Position)
{
  int byte_beg = Motor_Number * 4;

  byte four = (Position & 0xFF);
  byte three = ((Position >> 8) & 0xFF);
  byte two = ((Position >> 16) & 0xFF);
  byte one = ((Position >> 24) & 0xFF);

  EEPROM.write(byte_beg, four);
  EEPROM.write(byte_beg + 1, three);
  EEPROM.write(byte_beg + 2, two);
  EEPROM.write(byte_beg + 3, one);
}
