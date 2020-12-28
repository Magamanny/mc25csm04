#include <Arduino.h>
#include <SPI.h>
/*
Pin connection
AT25040 | Arduino Uno
SI -> 11(MOSI)
SO -> 12(MISO)
SCK -> 13(SCK)
CS -> 10(GPIO)
Vcc -> +5V
Vss- > GND
WP -> +5V Pull Up
HOLD -> +5V Pull Up

AT25040 SOIC pinout
CS = 1
SO = 2
WP = 3
Vss= 4(Ground)
SI = 5
SCK= 6
HOLD=7
Vcc=8(Suppy)
See the atmel datasheet
SCK max
1.8-5.5V = 5Mhz
2.7-5.5V = 10Mhz
4.5-5.5V = 20Mhz
*/
#define _CS0 10

#define WRSR 0x01
#define WRITE 0x02
#define READ 0x03
#define WRDI 0x04
#define RDSR 0x05
#define WREN 0x06
// Read function is quick
// Write function takes 5ms(this is blocking)
void writeEnable()
{
  digitalWrite(_CS0, LOW);
  SPI.transfer(WREN); // write enable
  digitalWrite(_CS0, HIGH);
}
void writeDisable()
{
  digitalWrite(_CS0, LOW);
  SPI.transfer(WRDI); // write disable instruction
  digitalWrite(_CS0, HIGH);
}
// status is unclear..
void readStatus()
{
  uint8_t response;
  digitalWrite(_CS0, LOW);
  SPI.transfer(RDSR);            // status register
  response = SPI.transfer(0x00); // read response
  //Serial.print("Status Register=");
  //Serial.println(response, HEX);
  digitalWrite(_CS0, HIGH);
}
uint8_t readByte(uint16_t addr)
{
  uint8_t response;
  uint8_t addr_low, addr_high;
  // break the address in 8 bit cunck
  addr_high = 0xff & (addr >> 8);
  addr_low = 0xff & (addr);
  // only single bit in the high address
  addr_high = addr_high & 0x01;
  digitalWrite(_CS0, LOW);
  SPI.transfer(READ | (addr_high<<3));// read instruction
  SPI.transfer(addr_low);            // address
  response = SPI.transfer(0x00); // read
  digitalWrite(_CS0, HIGH);
  return response;
}
void writeByte(uint16_t addr, uint8_t data)
{
  uint8_t addr_low, addr_high;
  // break the address in 8 bit cunck
  addr_high = 0xff & (addr >> 8);
  addr_low = 0xff & (addr);
  // only single bit in the high address
  addr_high = addr_high & 0x01;
  writeEnable();
  digitalWrite(_CS0, LOW);
  SPI.transfer(WRITE | (addr_high << 3)); // write instruction
  SPI.transfer(addr_low);                 // address
  SPI.transfer(data);                     // data
  digitalWrite(_CS0, HIGH);
  delay(5);
}
void setup()
{
  uint8_t data;
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(F("EEPROM POC, for PSO, IC used AT25040"));
  pinMode(_CS0, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(_CS0, HIGH);
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  readStatus();
  // one time test
  writeByte(0x1cc,'X'); // 0x02
  writeByte(0x1cc,'A'); // 0x02
  data = readByte(0x1cc); // 0x03
  Serial.print(F("Data="));
  Serial.println((char)data);
}

void loop()
{
  /*
  writeByte(0x1cc,'X'); // 0x02
  writeByte(0x1cc,'A'); // 0x02
  delay(5);
  readByte(0x1cc); // 0x03
  delayMicroseconds(10);
  */
  // put your main code here, to run repeatedly:
}