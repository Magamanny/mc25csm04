#include <Arduino.h>
#include <SPI.h>
/*
Pin connection
AT25040 | Arduino Uno
SI -> 11(MOSI)
SO -> 12(MISO)
SCK -> 13(SCK)
CS -> 10(GPIO)
VCC -> +5V
GND- > GND
*/
#define _CS0 10
void readByte()
{
  uint8_t response;
  digitalWrite(_CS0, LOW);
  delayMicroseconds(1);
  
  SPI.transfer(0x03);            // status register
  SPI.transfer(0x10);            // address
  response = SPI.transfer(0x00); // read

  delayMicroseconds(1);
  digitalWrite(_CS0, HIGH);
}
void writeByte()
{
  digitalWrite(_CS0, LOW);
  delayMicroseconds(1);
  
  SPI.transfer(0x02);            // status register
  SPI.transfer(0x10);            // address
  SPI.transfer(0x55); // data

  delayMicroseconds(1);
  digitalWrite(_CS0, HIGH);
}
void writeEnable()
{
  uint8_t response;
  digitalWrite(_CS0, LOW);
  delayMicroseconds(1);
  
  SPI.transfer(0x06);            // status register

  delayMicroseconds(1);
  digitalWrite(_CS0, HIGH);
}
void readStatus()
{
  uint8_t response;
  digitalWrite(_CS0, LOW);
  delayMicroseconds(1);
  SPI.transfer(0x05);            // status register
  response = SPI.transfer(0x00); // read response
  //Serial.print("Status Register=");
  //Serial.println(response, HEX);
  delayMicroseconds(1);
  digitalWrite(_CS0, HIGH);
}
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(F("EEPROM POC, for PSO, IC used AT25040"));
  pinMode(_CS0, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(_CS0, HIGH);
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
  readStatus();
}

void loop()
{
  readStatus(); // 0x05
  delayMicroseconds(1); // wait
  writeEnable(); // 0x06
  delayMicroseconds(1);
  writeByte(); // 0x02
  delay(5);
  readByte(); // 0x03
  delayMicroseconds(10);
  // put your main code here, to run repeatedly:
}