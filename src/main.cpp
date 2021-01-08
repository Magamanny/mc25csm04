#include <Arduino.h>
#include <SPI.h>
/*
// IDE used Platform io
Pin connection
25csm04 has same pin out as the at25040
25CSM04 | Arduino Uno
SI -> 11(MOSI)
SO -> 12(MISO)
SCK -> 13(SCK)
CS -> 10(GPIO)
Vcc -> +5V
Vss- > GND
WP -> +5V Pull Up
HOLD -> +5V Pull Up

25CSM04 SOIC pinout
CS = 1
SO = 2
WP = 3
Vss= 4(Ground)
SI = 5
SCK= 6
HOLD=7
Vcc=8(Suppy)
See microchip datasheet
SCK max, tested on 4MHZ on 8Mhz no response(it may be due to external wiring)
if VCC>=2.5 = 5Mhz
if VCC>=3.0 = 8Mhz ,8Mhz ok on 5 volt
*/
#define CS0 10
// the register name is also similar to at25040
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
  digitalWrite(CS0, LOW);
  SPI.transfer(WREN); // write enable
  digitalWrite(CS0, HIGH);
}
void writeDisable()
{
  digitalWrite(CS0, LOW);
  SPI.transfer(WRDI); // write disable instruction
  digitalWrite(CS0, HIGH);
}
// status is unclear..
void readStatus()
{
  uint8_t response;
  digitalWrite(CS0, LOW);
  SPI.transfer(RDSR);            // status register
  response = SPI.transfer(0x00); // read response
  //Serial.print("Status Register=");
  //Serial.println(response, HEX);
  digitalWrite(CS0, HIGH);
}
uint8_t readByte(uint32_t addr)
{
  uint8_t addr8;
  uint8_t response;
  digitalWrite(CS0, LOW);
  SPI.transfer(READ);// read instruction
  // address
  addr8 = addr>>16;
  SPI.transfer(addr8);                 // msb
  addr8 = addr>>8;
  SPI.transfer(addr8);                 // mid
  addr8 = addr;
  SPI.transfer(addr8);                 // lsb
  response = SPI.transfer(0x00); // read
  digitalWrite(CS0, HIGH);
  return response;
}
// address is 19 bit, but 24bit are send via spi, see data  
void writeByte(uint32_t addr, uint8_t data)
{
  uint8_t addr8;
  writeEnable();
  digitalWrite(CS0, LOW);
  SPI.transfer(WRITE); // write instruction
  addr8 = addr>>16;
  SPI.transfer(addr8);                 // msb
  addr8 = addr>>8;
  SPI.transfer(addr8);                 // mid
  addr8 = addr;
  SPI.transfer(addr8);                 // lsb
  SPI.transfer(data);                  // data
  digitalWrite(CS0, HIGH);
  delay(5);
}
// read string of data for specific length
void readString(uint32_t addr, uint8_t *data, uint16_t len)
{
  uint8_t addr8;
  digitalWrite(CS0, LOW);
  SPI.transfer(READ);// read instruction
  addr8 = addr>>16;
  SPI.transfer(addr8);                 // msb
  addr8 = addr>>8;
  SPI.transfer(addr8);                 // mid
  addr8 = addr;
  SPI.transfer(addr8);                 // lsb
  // read the sting
  for(uint16_t i=0;i<len;i++)
  {
    data[i] = SPI.transfer(0x00); // read
  }
  digitalWrite(CS0, HIGH);
}
// 8 byte for data 1 for the null terminator
// use block of 4bytes and write in cunch of 256bytes(as page size is 256bytes)
void writePage(uint32_t addr, uint8_t data[256])
{
  uint8_t addr8;
  writeEnable();
  digitalWrite(CS0, LOW);
  SPI.transfer(WRITE); // write instruction
  addr8 = addr>>16;
  SPI.transfer(addr8);                 // msb
  addr8 = addr>>8;
  SPI.transfer(addr8);                 // mid
  addr8 = addr;
  SPI.transfer(addr8);                 // lsb
  // the internal address is increased automatically, the lower 3 bit
  // this means that the address must be a multiple of 8, of all 8 bytes to be writen in 1 go
  for(int i=0;i<256;i++)
  {
    SPI.transfer(data[i]);                     // data
  }
  digitalWrite(CS0, HIGH);
  delay(5);
}
void test1()
{
  uint8_t data;
  writeByte(0x1cc,'X'); // 0x02
  data = readByte(0x1cc); // 0x03
  Serial.println((char)data);
  writeByte(0x1cc,'A'); // 0x02
  data = readByte(0x1cc); // 0x03
  Serial.print(F("Data="));
  Serial.println((char)data);
}
// page write test
void test2()
{
  uint32_t addr=262144; // this must be a multiple of 8, for page write
  static uint8_t data_write[256]=
  "Take this kiss upon the brow!"
  "And, in parting from you now,"
  "Thus much let me avow --"
  "You are not wrong, who deem"
  "That my days have been a dream";
  static uint8_t data_read[256]={0}; // creat and init with zero
  writePage(addr,data_write);
  readString(addr,data_read,256);
  Serial.print(F("Data="));
  Serial.println((char*)data_read);
}
int test3()
{
  uint8_t fail=false;
  uint16_t addr=8; // this must be a multiple of 8, for page write
  uint8_t data_write[]={'A','L','I','-','R','@','D','*'};
  uint8_t data_read[10]={0}; // creat and init with zero
  data_write[7] = rand()%255;
  writePage(addr,data_write);
  readString(addr,data_read,8);
  for(int i=0;i<8;i++)
  {
    if(data_write[i]!=data_read[i])
    {
      fail=true;
      break; // failed
    }
  }
  return fail;
}
int failCount;
const int testCount=1000;
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(F("EEPROM POC, for PSO, IC used AT25040"));
  pinMode(CS0, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(CS0, HIGH);
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  readStatus();
  // one time test
}

void loop()
{
  test2();
  //test2();
  //delayMicroseconds(1);
  /*
  failCount=0;
  for(int i=0;i<testCount;i++)
  {
    if(test3()) // if test failed
    {
      failCount++;
    }
  }
  Serial.println(F("-------Result-----"));
  Serial.print(F("Total Test="));
  Serial.println(testCount);
  Serial.print(F("Failed Test="));
  Serial.println(failCount);
  Serial.print(F("Error %="));
  */
  // Serial.println((failCount/100.0)); // this line for some resone causes the code to hold
  delay(1000); // 1 sec delay
  /*
  writeByte(0x1cc,'X'); // 0x02
  writeByte(0x1cc,'A'); // 0x02
  delay(5);
  readByte(0x1cc); // 0x03
  delayMicroseconds(10);
  */
  // put your main code here, to run repeatedly:
}