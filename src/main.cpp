#include <Arduino.h>
#include <SPI.h>
#include "mc25csm04.h"
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

25CSM04 | Blue pill
SI -> PA7(MOSI)
SO -> PA6(MISO)
SCK -> PA5(SCK)
CS -> A0(GPIO)
Vcc -> +3.3V
Vss- > GND
WP -> +3.3V Pull Up
HOLD -> +3.3V Pull Up

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
// uno
/*
#define CS0 10
#define MOSI 11
#define MISO 12
#define SCK 13
*/
// blue pill
#define CS0 PA4
#define MOSI PA5
#define MISO PA6
#define SCK PA7
// the register name is also similar to at25040
#define WRSR 0x01
#define WRITE 0x02
#define READ 0x03
#define WRDI 0x04
#define RDSR 0x05
#define WREN 0x06
uint8_t SPI_transfer(uint8_t reg)
{
  return SPI.transfer(reg);
}
void delay_ms(uint32_t ms)
{
  delay(ms);
}
void chipSelect()
{
	// CS high time, 200ns
	for(int i=0;i<50;i++)
	{
	}
  digitalWrite(CS0, LOW);
	// CS setup time, 200ns(min for 3.3v), try to perform 1us delay
	for(int i=0;i<50;i++)
	{
	}
}
void chipDeselect()
{
	// CS hold time, 200ns(min for 3.3v), try to perform 1us delay
	for(int i=0;i<50;i++)
	{
	}
  digitalWrite(CS0, HIGH);
}
void test1()
{
  uint8_t data;
  writeByte(0x1cc,'X'); // 0x02
  data = readByte(0x1cc); // 0x03
  Serial.print(F("Data1="));
  Serial.println((char)data);
  writeByte(0x1cc,'A'); // 0x02
  data = readByte(0x1cc); // 0x03
  Serial.print(F("Data2="));
  Serial.println((char)data);
}
// page write test
void test2()
{
  uint32_t addr=200; // this must be a multiple of 8, for page write
  static uint8_t data_write[256]=
  "Take this kiss upon the brow!\r\n"
  "And, in parting from you now,\r\n"
  "Thus much let me avow --\r\n"
  "You are not wrong, who deem\r\n"
  "That my days have been a dream\r\n";
  static uint8_t data_read[256]={0}; // creat and init with zero
  writePage(addr,data_write,256);
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
  writePage(addr,data_write,8);
  readString(addr,data_read,8);
  for(int i=0;i<8;i++)
  {
    if(data_write[i]!=data_read[i])
    {
      fail=true;
      break; // failed
    }
  }
  Serial.print("data print=");
  Serial.print((char*)data_read);
  return fail;
}
int failCount;
const int testCount=1000;
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(F("EEPROM POC, for PSO, IC used 25CSM04"));
  pinMode(CS0, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, OUTPUT);
  pinMode(SCK, OUTPUT);

  chipDeselect();
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  mc25csm04Init(chipDeselect,chipSelect,SPI_transfer,delay_ms);
  readStatus();
  // one time test
}

void loop()
{
  uint8_t data[100];
  readStatus();
  writeStatus(0x80);
  test2();
  test3();
  //test1();
  //test2();
  /*
  delayMicroseconds(1);
  
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
  */
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